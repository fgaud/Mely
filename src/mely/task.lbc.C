/*
 *
 * Copyright (C) 1998 David Mazieres (dm@uun.org)
 * Copyright (C) 2010 Sardes Project INRIA France
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *
 */
#include "task.common.h"
#include "task.lbc.h"

/**
 * Global view:
 * - Each thread treats a different color (acolor).
 * - Each thread have a list of colors he needs to handle (in thread_state).
 * - A color can be stolen.
 * - Tasks are always posted on the core that handles the task's color.
 */

/******************************************************************************
 * Variables
 ******************************************************************************/
int nthreads;                                      /* nb total threads                             */
thread_private_t thread_state[MAX_THREADS];        /* Global struct containing private thread data */
static pthread_t thread_obj[MAX_THREADS];          /* pthread_t object, so that we can join on it */
__thread unsigned int _thread_no= MAX_THREADS;                  /* Current thread number */


static PAD(volatile char) color_to_queue[MAX_COLORS]; /* SHARED - color -> thread */
static PAD(Task_List*) acolor[MAX_THREADS+1];           /* thread -> current color */
static Task_List* tl_array[MAX_COLORS];               /* SHARED - Task list, per color */
static Task_List* negative_tl_array[MAX_THREADS];     /* SHARED - Task lists for negative colors */


#if PROFILING_SUPPORT
thread_stats_t thread_stats[MAX_THREADS+1];
#endif

#define TL_ARRAY(color) tl_array[color]
#define NEG_TL_ARRAY(tn) negative_tl_array[tn]

/******************************************************************************
 * Functions
 * 1/ General purpose functions
 * 2/ Taks inserting/removing from task lists :
 *    - stealing lists, used by stealers. Only contains colors worth stealing.
 *    - task lists (tl), used by the owner threads.
 * 3/ Stealing function
 * 4/ Thread loop and choose_task.
 *       while(...) {
 *          t = choose_task();
 *          if(!t) steal();
 *       }
 * 5/ Initialization
 ******************************************************************************/
void task_print_queue_nolock(int which_thread);       /* Print all tasks of thread#which_thread */

/******************************************************************************
 * 1/ General purpose functions
 ******************************************************************************/
void task_set_nthreads(int n) {
   assert (n > 0 && n <= MAX_THREADS);
   nthreads = n;
}

int task_get_nthreads (){
   return nthreads;
}

int get_current_color(){
   if(ACOLOR(_thread_no))
      return ACOLOR(_thread_no)->color;
   else
      return -42;
}

int color_to_thread(int color) {
   return color_to_queue[color].val;
}

unsigned int get_current_proc(){
   return _thread_no;
}

/******************************************************************************
 * 2/ Task inserting and wakeup-ing
 * - rt_wakeup & try_to_wakeup: wakeup a thread so that he can execute a task or steal.
 * - insert/remove_in_xxx_list: insert a task in the steal list or classic task list (tl)
 ******************************************************************************/
#if STEALING
static inline bool is_stealable(int which_thread){
   int nb_stealable = THREAD_STATE(which_thread).steal_color_list_count;
   if(ACOLOR(which_thread) && ACOLOR(which_thread)->is_stealable)
      nb_stealable--;
   return nb_stealable > 1;
}

/** Try to wakeup a core so that this core can steal us. Only called is is_stealable is true.*/
static void try_to_wakeup(int thismany, int close_to_which_thread) {
#if !REMOVE_EPOLL_TIMEOUT
#if CE_WS
   if(shared_cache_vector[close_to_which_thread] == NULL){
      PANIC("Warning thread %d has no neighbors\n", close_to_which_thread);
   }

   sibling *start_idx = shared_cache_vector[close_to_which_thread];
   sibling *i = start_idx;
   int woken = 0;
   do{
      if(THREAD_STATE(i->core).sleeping){
         wakeup_fdwatcher(i->core);
         woken ++;
      }
      i = i->next;
   } while(i && woken <= thismany);
#else
   int start_idx = 0;
   int i = start_idx;
   int woken = 0;
   do{
      if(THREAD_STATE(i).sleeping){
         wakeup_fdwatcher(i);
         woken ++;
      }
      i = (i+1) % nthreads;
   } while(i != start_idx  && woken <= thismany);
#endif
#endif
}
#endif

/* Wakeup the thread which is supposed to handle the task. */
/* Call try_to_wakeup to wakeup neighbors                  *
 * and wakeup_fdwatcher to wakeup the thread.              */
static inline void _rt_wakeup(CBV_PTR_TYPE cb, unsigned int thread_dest){
   LOG_REGISTER_TASK_WAKEUP_COST(
#if STEALING
	   // Optimize the conditions on which to wakeup somebody to steal us: we have new tasks to be stolen
      // and other cores might be willing to steal us (has_been_stolen = 1).
	   if (is_stealable(thread_dest)){
	      if(THREAD_STATE(thread_dest).was_stealable && !THREAD_STATE(thread_dest).has_been_stolen) {
	         // Nothing to do : we haven't been stolen !
	      } else {
	         THREAD_STATE(thread_dest).has_been_stolen = 0;
	         try_to_wakeup(THREAD_STATE(thread_dest).num_unique_colors - 1, thread_dest);
	      }
	      THREAD_STATE(thread_dest).was_stealable = 1;
	   } else {
	      THREAD_STATE(thread_dest).was_stealable = 0;
	   }
#endif

#if !REMOVE_EPOLL_TIMEOUT
	   if(thread_dest != _thread_no && (cb == NULL || cb->getfaddr()!= acheck_task)) {
	      wakeup_fdwatcher(thread_dest);
	   }
#endif //!REMOVE_EPOLL_TIMEOUT
   )
}

/* Steal list manipulation: insert */
static inline void insert_in_steal_list(int which_thread, Task_List* tl){
   #if STEALING
   if(tl->color < 0)
      return;
   #if TIME_LEFT_WORKSTEALING
   uint64_t total_processing_duration = tl->total_processing_duration;
   if(total_processing_duration < MUST_STEAL_THRESHOLD)
      return;
   #endif

   #if SORT_STEAL_LIST
   Task_List *tail;
   int do_something;
   if(total_processing_duration < MEDIUM_THRES) {
      tail = THREAD_STATE(which_thread).steal_last_of_color_list_low;
      do_something = 1;
   } else if(total_processing_duration < HIGH_THRES) {
      tail = THREAD_STATE(which_thread).steal_last_of_color_list_medium;
      do_something = 2;
   } else {
      tail = THREAD_STATE(which_thread).steal_last_of_color_list_high;
      do_something = 3;
   }
   if(tl->is_stealable == do_something)
      return;

   if (tl->is_stealable) { // Need to over-dequeue.
      if (tl->steal_prev)
         tl->steal_prev->steal_next = tl->steal_next;
      if (tl->steal_next)
         tl->steal_next->steal_prev = tl->steal_prev;
   } else {
      THREAD_STATE(which_thread).steal_color_list_count++;
   }
   tl->is_stealable = do_something;

   tl->steal_next = tail->steal_next;
   tl->steal_prev = tail;
   tail->steal_next = tl;

   if(tl->steal_next == NULL && do_something != 1){
      PANIC("Bug ? Tl is %p, tl->steal_next = %p (do_something = %d)\n",
            tl, tl->steal_next, do_something);
   }

   if(tl->steal_next != NULL){
      tl->steal_next->steal_prev = tl;
   }

   #else
   if(tl->is_stealable) // Already here
      return;
   tl->is_stealable = 1;

   // Enqueue
   tl->steal_prev = THREAD_STATE(which_thread).steal_last_of_color_list;
   tl->steal_next = NULL;
   THREAD_STATE(which_thread).steal_last_of_color_list = tl;
   if(!tl->steal_prev)
      THREAD_STATE(which_thread).steal_color_list = tl;
   else
      tl->steal_prev->steal_next = tl;

   THREAD_STATE(which_thread).steal_color_list_count++;

   #endif

   #endif
}

/* ... remove */
static inline void remove_from_steal_list(int which_thread, Task_List* tl) {
   #if STEALING
   tl->is_stealable = 0;
   if (tl->steal_prev)
      tl->steal_prev->steal_next = tl->steal_next;
   if (tl->steal_next)
      tl->steal_next->steal_prev = tl->steal_prev;
   #if !SORT_STEAL_LIST
   if (tl == THREAD_STATE(which_thread).steal_last_of_color_list)
      THREAD_STATE(which_thread).steal_last_of_color_list = tl->steal_prev;
   if (tl == THREAD_STATE(which_thread).steal_color_list)
      THREAD_STATE(which_thread).steal_color_list = tl->steal_next;
   #endif
   THREAD_STATE(which_thread).steal_color_list_count--;

   //DEBUG_TMP("Remove : %d\n", tl->color);
   //task_print_steal_queue(which_thread);
   #endif
}

/* Insert a task list in the global color list. */
static inline void insert_in_tl(int which_thread, Task_List* tl, int increase_prio_num){
   if(tl == NULL){
      PANIC("Cannot insert a null task\n");
   }

   tl->current_prio = tl->default_prio;
   tl->prev = NULL;
   tl->next = NULL;

   /** Find where to insert **/
   Task_List* wti;
   for(wti = THREAD_STATE(which_thread).last_of_color_list; wti != NULL; wti = wti->prev){
      if((wti->current_prio + increase_prio_num) >= tl->default_prio
               || (ACOLOR(which_thread) == NULL && wti == THREAD_STATE(which_thread).color_list)){
         // Increase all others priorities
         break;
      }
      else{
         wti->current_prio += increase_prio_num;
      }
   }

   if(wti != NULL){
      // Reinsert
      tl->prev = wti;
      tl->next = wti->next;

      wti->next = tl;

      if(tl->next != NULL){
         tl->next->prev = tl;
      }else{
         THREAD_STATE(which_thread).last_of_color_list = tl;
      }
   }else{
      if(THREAD_STATE(which_thread).color_list != NULL){
         tl->next = THREAD_STATE(which_thread).color_list;
         tl->next->prev = tl;
      }
      else{
         THREAD_STATE(which_thread).last_of_color_list = tl;
      }

      THREAD_STATE(which_thread).color_list = tl;
   }
}

/* Suppress a whole color from a thread (not just a task) */
static inline void unlink_tl(int which_thread, Task_List* tl){
   if(tl->next != NULL){
      tl->next->prev = tl->prev;
   }

   if(tl->prev != NULL){
      tl->prev->next = tl->next;
   }

   if(tl == THREAD_STATE(which_thread).last_of_color_list){
      assert(tl->next == NULL);
      THREAD_STATE(which_thread).last_of_color_list = tl->prev;
   }

   if(tl == THREAD_STATE(which_thread).color_list){
      assert(tl->prev == NULL);
      THREAD_STATE(which_thread).color_list = tl->next;
   }

   tl->prev = NULL;
   tl->next = NULL;
}

/* Create a task list (only used at init time) */
Task_List* _create_tl(int color, int prio){
   assert(color < MAX_COLORS);
   Task_List* tl = TL_ARRAY(color);
   if(tl == NULL){
      tl = new Task_List();

      tl->dummy = 0;

      tl->first_task = new Task (color);
      tl->last_head_task = new Task (color);
      tl->last_task = new Task (color);

      tl->first_task->dummy = true;
      tl->last_head_task->dummy = true;
      tl->last_task->dummy = true;

      /** Make the initial link **/
      tl->first_task->next = tl->last_head_task;
      tl->last_head_task->prev = tl->first_task;
      tl->last_head_task->next = tl->last_task;
      tl->last_task->prev = tl->last_head_task;

      tl->next = NULL;
      tl->prev = NULL;

      tl->steal_next = NULL;
      tl->steal_prev = NULL;

      tl->color = color;
      tl->default_prio = prio;
      tl->current_prio = prio;

      tl->nb_callbacks = 0;

#if TIME_LEFT_WORKSTEALING
      tl->total_processing_duration = 0;
#endif

      TL_ARRAY(color) = tl;
   }
   else{
      PRINT_ALERT("Color %d already exists ...\n",color);
      exit(EXIT_FAILURE);
   }
   return tl;
}

/* Get a task from a thread freelist or allocate it */
static inline Task* _rt_get_task(unsigned int from_thread, CBV_PTR_TYPE cb){
   Task *t = FREETASK(from_thread);

   if (t) {
      FREETASK(from_thread) = t->next;
      t->setcb(cb);
      FREETASK_COUNT(from_thread) --;
   } else {
      assert (t = new Task(cb));
   }

   return t;
}

/*
 * Real stuff: insert a callback in a task list.
 * - basic list insertion using insert_in_tl
 * - maybe insertion in the steal list.
 */
static inline void _register_cb(CBV_PTR_TYPE cb, int thread_dest, char last){
   Task *p;

#if TRACE_REGISTER_TASK
   uint64_t t_start, t_stop;
   rdtscll(t_start);
#endif

   /** Here we have elected a thread and taken a lock on its queue **/
   Task_List * tl = NULL;

   int color = cb->getcolor();

   if(color < 0){
      tl = NEG_TL_ARRAY(thread_dest);
   } else {
      color = color % MAX_COLORS;
      tl = TL_ARRAY(color);
   }

   if(tl->nb_callbacks == 0
      && (ACOLOR(thread_dest) == NULL || (ACOLOR(thread_dest)->color != color))) {
      tl->default_prio = cb->getprio();
      if(tl->color >= 0)
         THREAD_STATE(thread_dest).num_unique_colors++;
      insert_in_tl(thread_dest, tl, 0);
   }


   Task* t = _rt_get_task(thread_dest, cb);

#if TRACE_REGISTER_TASK
   rdtscll(t_stop);
   THREAD_STATS.register_task_find_task_costs += t_stop - t_start;
   t_start = t_stop;
#endif

   if (last) {
      p = tl->last_task;
   } else {
      p = tl->last_head_task;
   }

   t->next = p;
   t->prev = p->prev;
   t->prev->next = t;
   p->prev = t;

   tl->default_prio = t->prio;

   tl->nb_callbacks++;
   TASK_COUNT(thread_dest)++;
#if TIME_LEFT_WORKSTEALING
   tl->total_processing_duration += cb->get_timeleft();
#endif
   insert_in_steal_list(thread_dest, tl);

#ifdef TRACE_REGISTER_TASK
   rdtscll(t_stop);
   THREAD_STATS.register_task_insert_task_costs += t_stop - t_start;
   t_start = t_stop;
#endif

   DEBUG("[%d] Registering task %p [Handler %p, color %d] on thread %d. "
            "Number of unique colors: %d, task count = %d\n",
            _thread_no,
            t,
            cb->getfaddr(),
            cb->getcolor(),
            thread_dest,
            THREAD_STATE(thread_dest).num_unique_colors,
            TASK_COUNT(thread_dest));
}

/** Register a callback
 * 0 - head
 * 1 - back
 **/
static int register_task(CBV_PTR_TYPE cb, char last) {
   int thread_dest = 1;

#ifdef PROFILING_SUPPORT
   uint64_t global_start = 0;
   uint64_t t_stop = 0;

   rdtscll(global_start);
#endif

#ifdef TRACE_REGISTER_TASK
   THREAD_STATS.register_task_nb_calls++;
   uint64_t t_start = global_start;

   if(cb->getcolor() + 42 > 0) { // Always true; used to monitor the latency of the first access to the callback
      rdtscll(t_stop);
      THREAD_STATS.register_task_cb_first_access += t_stop - t_start;
      t_start = t_stop;
   }
#endif

   /** Firstly we need to choose a thread
    * If stealing is not active, the corresponding thread is chosen by color % nthread
    * If stealing is active, we need to find the corresponding
    * thread in the mapping array (mutual exclusion inside)
    **/
   int color = cb->getcolor();
   if(color >= 0){

#if STEALING
      // ensure that we're adding to the correct queue.
      while (1) {
         /** We need to do that because locks are associated with a thread
          * Advantage: no global locking
          **/
         thread_dest = COLOR_TO_QUEUE(cb->getcolor() % MAX_COLORS);
         if(thread_dest == -1){
            continue;
         }

         LOCK(thread_dest);

         if (thread_dest == COLOR_TO_QUEUE(cb->getcolor() % MAX_COLORS))
            break;
         else {
            UNLOCK(thread_dest);
         }
      }
#else
      int on_nthread = nthreads;
      thread_dest = color% on_nthread;
#if TRACE_REGISTER_TASK
      LOCK_AND_REG_COST(thread_dest, THREAD_STATS.register_task_lock_costs);
#else
      LOCK(thread_dest);
#endif

#endif /** STEALING **/
   } else{
      thread_dest = -color -1;
      if(!(thread_dest >=0 && thread_dest < nthreads)) {
         PANIC("%s:%d : Incorrect color %d when trying to post %p\n", __FILE__, __LINE__, cb->getcolor(), cb->getfaddr());
      }

#if TRACE_REGISTER_TASK
      LOCK_AND_REG_COST(thread_dest, THREAD_STATS.register_task_lock_costs);
#else
      LOCK(thread_dest);
#endif

      DEBUG("Received a negative color %d (adding to processor %d)\n",
               cb->getcolor(),thread_dest);
   }

   _register_cb(cb, thread_dest, last);

   _rt_wakeup(cb,thread_dest);

   UNLOCK(thread_dest);

#ifdef PROFILING_SUPPORT
   rdtscll(t_stop);
#endif

#ifdef TRACE_REGISTER_TASK
   THREAD_STATS.register_task_costs += (t_stop - global_start);
#endif // Trace_WS

#ifdef PROFILING_SUPPORT
   THREAD_STATS.register_task_time += (t_stop - global_start);
#endif

   return thread_dest;
}

int register_task_head(CBV_PTR_TYPE cb) {
   int n = register_task(cb, 0);
   return n;
}

int register_task(CBV_PTR_TYPE cb) {
   int n = register_task(cb, 1);
   return n;
}

/******************************************************************************
 * 3/ Stealing function !
 ******************************************************************************/
#if STEALING
#if CE_WS
#define NEXT_VICTIM() \
	(({ start_idx = start_idx->next; if(!start_idx) break; start_idx->core; }))
#define END_STEAL() \
	0 /* The steal ends on the NEXT_VICTIM call */
#else
#define NEXT_VICTIM() \
	((victim_number+1)%nthreads)
#define END_STEAL() \
	(victim_number == start_idx)
#endif
/** Return a list of stolen work **/
static Task_List * steal_work() {
#ifdef TRACE_WORKSTEALING
   long long tg;
   long long tl_start = 0;
   long long col_start = 0;
   long long vic_start = 0;
   long long vic_stop = 0;
   long long t0 = 0;

   rdtscll(tg);

   THREAD_STATS.workstealing_try_steal++;
#endif

   DEBUG("Thread %d want to steal work ... \n",_thread_no);
   unsigned int victim_number;

   Task_List * stolen = NULL;
   Task_List * stolen_last = NULL;

#if CE_WS
   if(shared_cache_vector[_thread_no] == NULL){
      PANIC("Thread %d has no neighbors\n", _thread_no);
   }

   sibling *start_idx = shared_cache_vector[_thread_no];
   victim_number = start_idx->core;
#else
   unsigned int start_idx = 0;
   victim_number = start_idx;
#endif

   do {
#ifdef TRACE_WORKSTEALING
      rdtscll(vic_start);
#endif
      /** Check is the victim is stealable **/
      if (victim_number == _thread_no || !is_stealable(victim_number)) {
         DEBUG("[%d -> %d] We don't want to steal ourselves or a thread which has no work to give \n", get_current_proc(), victim_number);

#ifdef TRACE_WORKSTEALING
         rdtscll(vic_stop);
         THREAD_STATS.workstealing_choose_victim_cost += vic_stop - vic_start;
#endif

         victim_number = NEXT_VICTIM();
         continue;
      }

      /** Here we have chosen a victim; lock it **/
#ifdef TRACE_WORKSTEALING
      rdtscll(tl_start);
      THREAD_STATS.workstealing_choose_victim_cost += tl_start - vic_start;
#endif

      LOCK(victim_number);

#ifdef TRACE_WORKSTEALING
      rdtscll(col_start);
      THREAD_STATS.workstealing_lock_cost += col_start - tl_start;
#endif

      /** How many colors to steal **/
#if USE_BATCH_WS
      int nb_colors_to_steal = (int) (BATCH_TASK_WS * THREAD_STATE(victim_number).steal_color_list_count);
      if(!nb_colors_to_steal)
         nb_colors_to_steal = 1;
#else
      int nb_colors_to_steal = 1;
#endif

      // Since previous check doesn't use locks we need to recheck now
      if (is_stealable(victim_number)) {
         THREAD_STATE(_thread_no).has_been_stolen = 1;
         int nb_tasks_really_stolen = 0;
         int nb_color_really_stolen = 0;

#ifdef TRACE_WORKSTEALING
         rdtscll(t0);
         THREAD_STATS.workstealing_choose_color_cost += t0 - col_start;
         THREAD_STATS.nb_colors_seen_when_stealing += THREAD_STATE(victim_number).num_unique_colors;
         THREAD_STATS.nb_tasks_seen_when_stealing += TASK_COUNT(victim_number);
#endif

         Task_List* tl = THREAD_STATE(victim_number).steal_color_list;
#define NEXT(tl) tl->steal_next

         while (tl != NULL && nb_color_really_stolen < nb_colors_to_steal) {
            if (ACOLOR(victim_number) && tl->color == ACOLOR(victim_number)->color) {
               tl = NEXT(tl);
               continue;
            }
            #if SORT_STEAL_LIST
            if(tl->dummy) {
               tl = NEXT(tl);
               continue;
            }
            #endif
            if(tl->nb_callbacks <= 0){
               task_print_queue_nolock(victim_number);
               PANIC("tl->nb_callbacks <= 0, color = %d\n", tl->color);
            }

            Task_List * to_steal = tl;
            nb_color_really_stolen++;
            nb_tasks_really_stolen += to_steal->nb_callbacks;

            /** Updating who's the owner of this color **/
            COLOR_TO_QUEUE(to_steal->color) = -1;

            _epoll_steal(to_steal->color,victim_number);

#if TRACE_MAPPING_WS
            Task * t = to_steal->first_task;
            while (t) {
               if(t->dummy){
                  t=t->next;
                  continue;
               }

               void* cb_p = t->cb->getfaddr();
               if((*(THREAD_STATS.eh_stats))[cb_p] == NULL){
                  (*(THREAD_STATS.eh_stats))[cb_p] = (_eh_stats_t*)calloc(sizeof(_eh_stats_t),1);
               }
               (*(THREAD_STATS.eh_stats))[cb_p]->nb_steals++;
               t=t->next;
            }
#endif

            // Saving the next
            tl = NEXT(tl);


            // Done after : we batch that !
            unlink_tl(victim_number, to_steal);
            remove_from_steal_list(victim_number, to_steal);

            // Add tl to stolen
            if(!stolen){
               stolen = to_steal;
               stolen_last = to_steal;
            } else {
               stolen_last->next = to_steal;
               stolen_last = to_steal;
            }

            DEBUG("*** Thread %d, stealing color %d on thread %d\n",
                     _thread_no,to_steal->color,victim_number);
         }

         if(nb_color_really_stolen > 0){
            // Decrease the number of unique color of the victim
            THREAD_STATE(victim_number).num_unique_colors -= nb_color_really_stolen;

            DEBUG("Setting unique color of thread %d to %d\n",victim_number,THREAD_STATE(victim_number).num_unique_colors);

            // Adjusting victim info
            TASK_COUNT(victim_number) -= nb_tasks_really_stolen;

            /** Releasing lock on the victim **/
            UNLOCK(victim_number);

#ifdef TRACE_WORKSTEALING
            long long t1;
            rdtscll(t1);

            THREAD_STATS.workstealing_count++;
            THREAD_STATS.workstealing_per_thread_count[victim_number]++;
            THREAD_STATS.workstealing_internal_cost += t1 - t0;

            THREAD_STATS.nb_colors_stolen_per_steal += nb_color_really_stolen;
            THREAD_STATS.nb_tasks_stolen_per_steal += nb_tasks_really_stolen;
#endif


            break;
         } else {
            PRINT_ALERT("Thread %d found no stealable color on thread %d (nb_colors_to_steal = %d). Must never happened since stealable is true...\n",
                     _thread_no,
                     victim_number,
                     nb_colors_to_steal);
            exit(EXIT_FAILURE);
         }
      } else{
         DEBUG("Victim %d has no work to give to thread %d.\n",
                  victim_number,_thread_no);

#ifdef TRACE_WORKSTEALING
         rdtscll(t0);
         THREAD_STATS.workstealing_choose_color_cost += t0 - col_start;
#endif
         UNLOCK(victim_number);

         // Continuing
         // WHY : start_idx = victim_number; ??
         victim_number = NEXT_VICTIM();
      }
   } while(!END_STEAL());

#ifdef TRACE_WORKSTEALING
   long long t1;
   rdtscll(t1);
   if(!stolen){
      THREAD_STATS.workstealing_lose_stealing_cost += t1 - tg;
      THREAD_STATS.workstealing_lose_stealing_count++;
   }
   else{
      THREAD_STATS.workstealing_success_cost += t1 - tg;
   }
   THREAD_STATS.workstealing_global_cost += t1 - tg;
#endif

   return stolen;
}
#endif //STEALING


/******************************************************************************
 * 4/ Choose task and main loop.
 ******************************************************************************/
/** This function is the one which choose the task to execute
 * Parameters taken into account are :
 * - the last color executed
 * - the weight assocated with the callback
 **/
static Task * choose_task() {
   LOG_CHOOSE_TASK(
	   Task_List * tl;

	   /* Just take the first color */
	   tl = THREAD_STATE(_thread_no).color_list;
	   if(!tl){
	      return NULL;
	   }

	   /* Here we have a color */
	   Task *t;
	   for (t = tl->first_task->next; t != tl->last_task; t = t->next) {
	      if(!t->dummy){
	         break;
	      }
	   }

	   /* Sanity checks... */
	   if(t == NULL || t->dummy){
	      LOCK(0);
	      if(t == NULL){
	         PRINT_ALERT("t is NULL [chosen thread %d, chosen color %d]. Must never happen\n",
	                  _thread_no,tl->color);
	      } else{
	         PRINT_ALERT("t is dummy [chosen thread %d, chosen color %d]. Must never happen (nb_callbacks_from_color = %d) \n",
	                  _thread_no,tl->color,tl->nb_callbacks);
	      }
	      task_print_queue_nolock(_thread_no);
	      UNLOCK(0);
	      exit(EXIT_FAILURE);
	   }
	   if((t->color % MAX_COLORS) != tl->color && t->color >= 0){
	       PRINT_ALERT("Color chosen %d is containing another color %d\n", tl->color, t->color);
	       task_print_queue_nolock(_thread_no);
	       exit(EXIT_FAILURE);
	    }

	   /* Removing task from color list */
	   t->prev->next = t->next;
	   t->next->prev = t->prev;

	   tl->nb_callbacks--;
	   if(tl->nb_callbacks < 0){
	      PANIC("Big bug. Tl->nb_callbacks = %d\n", tl->nb_callbacks);
	   }
#if TIME_LEFT_WORKSTEALING
	   tl->total_processing_duration -= t->cb->get_timeleft();
	   bool do_remove_from_steal_list = (tl->total_processing_duration <= MUST_STEAL_THRESHOLD);
#else
	   bool do_remove_from_steal_list = (tl->nb_callbacks == 0);
#endif
	   if(do_remove_from_steal_list && tl->is_stealable)
	      remove_from_steal_list(_thread_no, tl);

	   THREAD_STATE(_thread_no).nb_tasks_from_color_executed ++;
	   return t;
	)
}


/** The main function executed by all threads
 * xxx is the thread number
 **/
static void * task_thread_loop(void *xxx) {
   _thread_no = *((int*)xxx);
   Task *t= NULL;

#if PROFILING_SUPPORT
   THREAD_STATS.tasks_done = 0;
#endif

   /** If only one thread is used let the system to place thread ...
    * Else fix the thread on a given processor
    **/
   if(nthreads > 1){
      cpu_set_t  mask ;
     cpu_set_t  verif_mask ;
     CPU_ZERO (&mask);
     CPU_SET (cpu_map[_thread_no],&mask);
      if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) <0) {
         perror("pthread_setaffinity_np");
      } else {
         if ( pthread_getaffinity_np (pthread_self(),sizeof(verif_mask),&verif_mask) < 0 ){
            perror("pthread_attr_getaffinity_np failed\n");
         } else {
            unsigned int i;
            for (i=0;i<sizeof(verif_mask);i++){
               if(CPU_ISSET(i,&verif_mask)){
                  DEBUG("thread %d mapped onto processor %d\n",_thread_no, i);
                  break;
               }
            }
            assert(i<sizeof(verif_mask));
            if(i != _thread_no){
               PRINT_ALERT("******** WARNING, thread %d mapped on processor %d (instead of %d) *****",
                        _thread_no,
                        i,
                        cpu_map[_thread_no]);
            }
         }
      }
   }

   start_hwc(_thread_no);

   do {
#if STEALING
      /** If we have no work we try to steal it **/
      Task_List *stolen= NULL;
#endif

      LOCK(_thread_no);

#if STEALING
      if (TASK_COUNT(_thread_no) == 1 && nthreads > 1)
      {
         UNLOCK(_thread_no);
         stolen = WS_FUNCTION();
         LOCK(_thread_no);
	      if (stolen) { // insert stolen tasks
	         Task_List* tl = stolen;
	         while(tl != NULL){
	            Task_List* to_ins = tl;

	            /** Goes to the next **/
	            tl = tl->next;

	            int color = to_ins->color;

	            /** Increase the number of unique color of the stealer **/
	            THREAD_STATE(_thread_no).num_unique_colors ++;

	            /** Update Task_Count **/
	            TASK_COUNT(_thread_no) += to_ins->nb_callbacks;

	            /** Insert to_ins **/
	            insert_in_tl(_thread_no,to_ins,0);
	            insert_in_steal_list(_thread_no, to_ins);

	            /** Updating who's the owner of this color **/
	            COLOR_TO_QUEUE(color) = _thread_no;
	         }
	      }
      }
#endif /** STEALING **/

      Task_List* tl = ACOLOR(_thread_no);
      if(tl){
         int color = tl->color;

         if (tl->nb_callbacks == 0){
            if(color >= 0){
               THREAD_STATE(_thread_no).num_unique_colors --;
#if RESET_COLOR_ON_EMPTY_QUEUE
               int on_nthread = nthreads;
               COLOR_TO_QUEUE(color) = color % on_nthread;
#endif
            }
            // Reset batch factor
            THREAD_STATE(_thread_no).nb_tasks_from_color_executed = 0;
            unlink_tl(_thread_no, tl);
         } else{
            if(THREAD_STATE(_thread_no).nb_tasks_from_color_executed >= NB_TASKS_FROM_SAME_COLOR_THRES) {
               // Remove color from front
               if(tl != THREAD_STATE(_thread_no).last_of_color_list){
                  // Put this color at the end and set the next color
                  unlink_tl(_thread_no, tl);
                  insert_in_tl(_thread_no,tl,THREAD_STATE(_thread_no).nb_tasks_from_color_executed);
               }
               THREAD_STATE(_thread_no).nb_tasks_from_color_executed = 0;
            }
         }
      }

      t = choose_task();
      if (t) {
         /** The thread now execute this color **/
         if(t->color >= 0){
            ACOLOR(_thread_no) = TL_ARRAY(t->color%MAX_COLORS);
         } else {
            ACOLOR(_thread_no) = NEG_TL_ARRAY(_thread_no);
         }

         TASK_COUNT(_thread_no) --;
         CBV_PTR_TYPE tcb = t->cb;
         t->clear();

#if PROFILING_SUPPORT
         THREAD_STATS.tasks_done++;
#endif


#if !REMOVE_EPOLL_TIMEOUT
         if(tcb->getfaddr() == acheck_task && TASK_COUNT(_thread_no) >= 1){
            DEBUG("[Core %d] Something in the queue, need to execute a non-blocking select\n", _thread_no);
            wakeup_fdwatcher(_thread_no);
         }
#endif

         /** Add the structure to FREETASK **/
         if (FREETASK_COUNT(_thread_no) < MAX_FREETASKS) { // these are bounced when stealing work?
            t->next = FREETASK(_thread_no);
            FREETASK(_thread_no) = t;
            FREETASK_COUNT(_thread_no) ++;
         } else {
            delete(t);
         }
         UNLOCK(_thread_no);

#ifdef PROFILING_SUPPORT
         THREAD_STATS.register_task_time = 0;
         long long tb, ta;
         rdtscll(tb);
#endif

#ifdef TRACE_MAPPING
         uint64_t tts, tte;
         rdtscll(tts);
#endif


         (*tcb)();


#ifdef TRACE_MAPPING
         rdtscll(tte);

         if((*(THREAD_STATS.eh_stats))[tcb->getfaddr()] == NULL){
            (*(THREAD_STATS.eh_stats))[tcb->getfaddr()] = (_eh_stats_t*)calloc(sizeof(_eh_stats_t),1);
            (*(THREAD_STATS.eh_stats))[tcb->getfaddr()]->nb_calls = 0;
            DEBUG("Thread %d, cb = %p\n",_thread_no, tcb->getfaddr());
         }

         (*(THREAD_STATS.eh_stats))[tcb->getfaddr()]->nb_calls++;
         (*(THREAD_STATS.eh_stats))[tcb->getfaddr()]->length += (tte - tts);
#endif

         free_callback(tcb);

#ifdef PROFILING_SUPPORT
         rdtscll(ta);
         //Removes the register_task time done in the callback from cb_exec_time, because it is runtime time.
         THREAD_STATS.cb_exec_time+= (ta -tb) - THREAD_STATS.register_task_time;
#endif

      } else { // no task to do
#if !STEALING
         PANIC("BUG ???\n");
#else
         UNLOCK(_thread_no);
#endif
      }


   } while (true);

   return (0);
}

#ifdef TRACE_MAPPING

void print_trace_mapping(long long total_time){
   int i;
   map<void*,_eh_stats_t*>::const_iterator iter;

   /** Building stats **/
   //Map which associates a table to an event_handler_address
   //The table  has nthreads elements. Element n is _handler_time for thread n.
   map<void*,_eh_stats_t* > tmp_map;

   //Fill tmp_map which the data of each thread for each event_handler:
   for(i = 0; i< nthreads; i++){
      map<void*,_eh_stats_t*>* m = STATS(i).eh_stats;

      for (iter=((*m).begin()); iter != ((*m).end()); ++iter) {
         _eh_stats_t * v = tmp_map[iter->first];
         if(!v){
            //Create the table associated to the EH if it doesn't exit.
            v = (_eh_stats_t *)calloc(nthreads, sizeof(_eh_stats_t));
            tmp_map[iter->first] = v;
         }
         //insert the value associated to the thread in the EH table.
         v[i] = *iter->second;
      }
   }


   /** Printing **/
   printf("\n%30s","");
   for(i = 0; i< nthreads; i++){
      printf("\t\t%7s%2d","Thread",i);
   }
   printf("\n");

   printf("\nEH Mapping:\n");
   for (iter=tmp_map.begin(); iter != tmp_map.end(); ++iter) {
      print_EH_name(iter->first);
      _eh_stats_t* v = tmp_map[iter->first];

      for(int j = 0; j<nthreads;j++){
         uint64_t value = v[j].nb_calls;

         if(value == 0){
            printf("\t%15d (  %3.2g %%)",0,0.);
         }
         else{
            double r = ((double) value * 100.) / ((double)STATS(j).tasks_done);
            printf("\t%15llu (%3.02f %%)", (long long unsigned) value,r);
         }
      }

      printf("\n");
   }

   printf("\nEH total length:\n");
   for (iter=tmp_map.begin(); iter != tmp_map.end(); ++iter) {
      print_EH_name(iter->first);
      _eh_stats_t * v = tmp_map[iter->first];

      for(int j = 0; j<nthreads;j++){
         if(v[j].length == 0){
            printf("\t%15d (  %3.2g %%)",0,0.);
         }
         else{
            long double r = ((long double) v[j].length * 100.) / ((long double)total_time);

            printf("\t%15llu (%3.02Lf %%)", (long long unsigned)v[j].length, r);
         }
      }

      printf("\n");
   }

   printf("\nEH avg length:\n");
   for (iter=tmp_map.begin(); iter != tmp_map.end(); ++iter) {
      print_EH_name(iter->first);
      _eh_stats_t * v = tmp_map[iter->first];

      for(int j = 0; j<nthreads;j++){
         if(v[j].nb_calls == 0){
            printf("\t%18.2g cycles",0.);
         }
         else{
            long double avg_length = (long double) v[j].length / (long double) v[j].nb_calls;
            printf("\t%18.2Lf cycles", avg_length);
         }
      }

      printf("\n");
   }

#if TRACE_MAPPING_WS
   printf("\nEH stealed by core:\n");
   for (iter=tmp_map.begin(); iter != tmp_map.end(); ++iter) {
      print_EH_name(iter->first);
      _eh_stats_t * v = tmp_map[iter->first];

      for(int j = 0; j<nthreads;j++){
         printf("\t%15llu",(long long unsigned) v[j].nb_steals);
      }

      printf("\n");
   }
#endif //TRACE_MAPPING_WS
}
#endif //TRACE_MAPPING


/** Start all threads -1 executing task_thread_loop
 * and become a thread that execute it
 **/

void dump_runtime_state(int s){
   fprintf(stderr,"=========================================================\n");
   for(int i=0; i < nthreads; i++){
      pthread_cancel(thread_obj[i]);
   }
   for(int i=0; i < nthreads; i++){
#if PROFILING_SUPPORT
      fprintf(stderr,"\t- Nb event processed: %llu\n",
               (long long unsigned)STATS(i).tasks_done);
#endif

      task_print_queue_nolock(i);
      //dump_queue_state(i);

#if TRACE_REGISTER_TASK
      fprintf(stderr,"Register task calls = %llu\n",
               (long long unsigned) STATS(i).register_task_nb_calls);
#endif

   }

   exit(EXIT_FAILURE);
}



void task_go() {
      assert( signal(SIGINT, dump_runtime_state) != SIG_ERR );

      /** Set maximum priority for process **/
#if SET_MAXIMUM_PRIORITY
      int which = PRIO_PROCESS;
      id_t pid = getpid();
      int ret = setpriority(which, pid, -20);
      if(ret == -1){
         perror("## ERROR : Cannot set process priority to -20");
      }
      /** ret = getpriority(which, pid);
      DEBUG("Default priority %d for pid %d\n", ret, pid); **/
#endif

      for (int i = 0; i < nthreads; i++)
         ACOLOR(i) = NULL;

#ifdef SIGNAL_DUMP_FLAGS
      assert( signal(SIGINT, dumpEventStates) != SIG_ERR );
#endif //SIGNAL_DUMP_FLAGS

      init_hwc();

#ifdef PROFILING_SUPPORT
      rdtscll(start_cycle);
#endif


      int *tn;
      for (int i = 0; i < nthreads; i++){
         DEBUG("Creating thread %d\n",i);
         tn = (int*)malloc(sizeof(int));
         *tn = i;
         assert(pthread_create(&thread_obj[i], NULL, task_thread_loop, tn) == 0);
      }

      for(int i = 0; i< nthreads; i++) {
         pthread_join(thread_obj[i],NULL);
      }


#ifdef PROFILING_SUPPORT
      rdtscll(stop_cycle);
#endif

      if(get_bench_time())
         printf("%.6f ", (double)get_bench_time()/1000000);
      printf("PAGE_FAULTS_MIN %ld PAGE_FAULTS_MAJ %ld ",usage.ru_minflt,usage.ru_majflt);

      print_hwc_results ();

#ifdef PROFILING_SUPPORT
      uint64_t total_time = stop_cycle - start_cycle;

      if(total_time <= 0){
         PRINT_ALERT("Error stop (%llu) <= start (%llu)\n",
                  (unsigned long long) stop_cycle,
                  (unsigned long long) start_cycle);
         exit(EXIT_FAILURE);
      }

      printf("\n\n");
      uint64_t total_msg_consummed = 0;
      uint64_t cb_exec_time = 0;
      for(int i = 0; i < nthreads; i ++){
         total_msg_consummed += STATS(i).tasks_done;
         printf("Thread %d: nb message consummed: %llu (%.2Lf Kevents/s)\n",
                  i,
                  (long long unsigned) STATS(i).tasks_done,
                  (long double) STATS(i).tasks_done / (long double) get_bench_time() * 1000.);

         printf("\tcb_exec_time : %3.02Lf %% (%llu/%llu)\n",
                  ((long double) STATS(i).cb_exec_time/(long double)total_time)*100.,
                  (long long unsigned) STATS(i).cb_exec_time,
                  (long long unsigned) total_time);

         printf("\truntime_exec_time : %3.02Lf %% (%llu/%llu)\n\n",
                  ((long double) (total_time - STATS(i).cb_exec_time)/(long double)total_time)*100.,
                  (long long unsigned) total_time - STATS(i).cb_exec_time,
                  (long long unsigned) total_time);

         cb_exec_time += STATS(i).cb_exec_time;
      }

      cb_exec_time = cb_exec_time / nthreads;
      printf("Total message consummed: %llu (%.2Lf Kevents/s)\n",
               (long long unsigned) total_msg_consummed,
               (long double) total_msg_consummed / (long double) get_bench_time() * 1000.);

      printf("\tcb_exec_time : %3.02Lf %% (%llu/%llu)\n",
               ((long double) cb_exec_time/(long double)total_time)*100.,
               (long long unsigned) cb_exec_time,
               (long long unsigned) total_time);

      printf("\truntime_exec_time : %3.02Lf %% (%llu/%llu)\n",
               ((long double) (total_time - cb_exec_time)/(long double)total_time)*100.,
               (long long unsigned) total_time - cb_exec_time,
               (long long unsigned) total_time);
#endif

#if TRACE_WORKSTEALING && STEALING
      int global_workstealing_count = 0;

      printf("\n\n*** Workstealing info ***\n");
      for(int i = 0; i< nthreads; i++){
         if(STATS(i).workstealing_try_steal){
            global_workstealing_count += STATS(i).workstealing_count;
            printf("\t- Thread %d : %llu steals done (%llu try %3.02Lf%%) (fails: %3.02Lf%%)\n",
                     i,
                     STATS(i).workstealing_count,STATS(i).workstealing_try_steal,
                     ((long double)STATS(i).workstealing_count/(long double)STATS(i).workstealing_try_steal*100.),
                     (((long double) STATS(i).workstealing_lose_stealing_count/(long double)STATS(i).workstealing_try_steal)*100.)
            );

            int j;
            for(j=0;j<nthreads;j++){
               printf("\t\t* %llu from thread %d\n",
                        (long long unsigned) STATS(i).workstealing_per_thread_count[j],j);
            }

            printf("\t  avg nb tasks stolen: %3.02Lf\n",
                     ((long double) STATS(i).nb_tasks_stolen_per_steal/(long double)STATS(i).workstealing_count));
            printf("\t  avg nb colors stolen: %3.02Lf\n",
                     ((long double) STATS(i).nb_colors_stolen_per_steal/(long double)STATS(i).workstealing_count));
            printf("\t  avg nb colors seen on steal: %3.02Lf\n",
                     ((long double) STATS(i).nb_colors_seen_when_stealing/(long double)STATS(i).workstealing_count));
            printf("\t  avg nb tasks seen on steal: %3.02Lf\n",
                     ((long double) STATS(i).nb_tasks_seen_when_stealing/(long double)STATS(i).workstealing_count));

            printf("\t  ws global cost: %3.03Lf %%, %llu cycles per steal (successed or not)\n",
                     ((long double) STATS(i).workstealing_global_cost/(long double)total_time)*100.,
                     (long long unsigned)STATS(i).workstealing_global_cost / STATS(i).workstealing_try_steal);

            printf("\t  ws sucess cost: %3.03Lf %%, %llu cycles per steal (successed or not)\n",
                     ((long double) STATS(i).workstealing_success_cost/(long double)total_time)*100.,
                     STATS(i).workstealing_count
                     ? (long long unsigned)STATS(i).workstealing_success_cost / STATS(i).workstealing_count
                     : 0);

            printf("\t  lose stealing costs: %3.03Lf %%\n",
                     ((long double) STATS(i).workstealing_lose_stealing_cost/(long double)total_time)*100.);

            printf("\t  find victim cost: %3.03Lf %%\n",
                     ((long double) STATS(i).workstealing_choose_victim_cost/(long double)total_time)*100.);
            printf("\t  find color cost: %3.03Lf %%\n",
                     ((long double) STATS(i).workstealing_choose_color_cost/(long double)total_time)*100.);
            printf("\t  internal cost: %3.03Lf %%\n",
                     ((long double) STATS(i).workstealing_internal_cost/(long double)total_time)*100.);
            printf("\t  workstealing locks cost: : %3.03Lf %%\n\n",
                     ((long double) STATS(i).workstealing_lock_cost/(long double)total_time)*100.);


            printf("\n");
         }
      }

      printf("\t  Global : %d colors stolen\n",global_workstealing_count);
      printf("\n");
#endif

#ifdef TRACE_MAPPING
      print_trace_mapping(total_time);
#endif

#ifdef TRACE_CONTENTION
      printf( "\nTime spent in runtime locks:\n");
      for(int i = 0; i< nthreads; i++){
         if(STATS(i).lock_nb_calls){
            printf("\tThread %d: total %llu cycles (%3.02Lf %%), nb calls %llu, per call %llu cycles \n",
                     i,
                     (long long unsigned)STATS(i).time_in_contention,
                     ( (((long double)STATS(i).time_in_contention)/(long double)total_time)*100.),
                     (long long unsigned) STATS(i).lock_nb_calls,
                     (long long unsigned) STATS(i).time_in_contention / STATS(i).lock_nb_calls);
         }
      }
#endif //TRACE_CONTENTION


#ifdef TRACE_CHOOSE_TASK
      printf( "\nChoose task profiling:\n");
      for(int i=0 ; i< nthreads; i++){
         if(STATS(i).choose_task_nb_calls){
            printf("* Thread %d:\n",i);
            printf("\t  choose task nb calls: %llu\n",
                     (long long unsigned) STATS(i).choose_task_nb_calls);
            printf("\t  choose task cost: %llu (%3.03Lf %%, %llu cycles)\n",
                     (long long unsigned) STATS(i).choose_task_cost,
                     ((long double) STATS(i).choose_task_cost/(long double)total_time)*100.,
                     (long long unsigned) STATS(i).choose_task_cost / STATS(i).choose_task_nb_calls);
         }
      }
#endif//TRACE_CHOOSE_TASK

#ifdef TRACE_REGISTER_TASK
      printf( "\nRegister task profiling:\n");
      for(int i=0 ; i< nthreads; i++){
         if(STATS(i).register_task_nb_calls){
            printf("* Thread %d:\n",i);
            printf("\t  register tasks nb calls: %llu (%.2Lf Kcalls/s)\n",
                     (long long unsigned) STATS(i).register_task_nb_calls,
                     ((long double) STATS(i).register_task_nb_calls * 1000.) / (double)bench_time);
            printf("\t  register tasks total cost: %llu (%3.03Lf %%)\n",
                     (long long unsigned) STATS(i).register_task_costs,
                     ((long double) STATS(i).register_task_costs/(long double)total_time)*100.);

            printf("\t  register tasks lock cost: %llu (%3.03Lf%%, relative %3.03Lf%%, %llu cycles)\n",
                     (long long unsigned) STATS(i).register_task_lock_costs,
                     ((long double) STATS(i).register_task_lock_costs/(long double)total_time)*100.,
                     ((long double) STATS(i).register_task_lock_costs/(long double)STATS(i).register_task_costs)*100.,
                     (long long unsigned) STATS(i).register_task_lock_costs / STATS(i).register_task_nb_calls);

            printf("\t  register tasks find task cost: %llu (%3.03Lf%%, relative %3.03Lf%%, %llu cycles)\n",
                     (long long unsigned) STATS(i).register_task_find_task_costs,
                     ((long double) STATS(i).register_task_find_task_costs/(long double)total_time)*100.,
                     ((long double) STATS(i).register_task_find_task_costs/(long double)STATS(i).register_task_costs)*100.,
                     (long long unsigned) STATS(i).register_task_find_task_costs / STATS(i).register_task_nb_calls);

            printf("\t  register tasks insert task cost: %llu (%3.03Lf %%, relative %3.03Lf%%, %llu cycles)\n",
                     (long long unsigned) STATS(i).register_task_insert_task_costs,
                     ((long double) STATS(i).register_task_insert_task_costs/(long double)total_time)*100.,
                     ((long double) STATS(i).register_task_insert_task_costs/(long double)STATS(i).register_task_costs)*100.,
                     (long long unsigned) STATS(i).register_task_insert_task_costs / STATS(i).register_task_nb_calls);

            printf("\t  register tasks wake up cost: %llu (%3.03Lf %%, relative %3.03Lf %%, %llu cycles)\n",
                     (long long unsigned) STATS(i).register_task_wake_up_costs,
                     ((long double) STATS(i).register_task_wake_up_costs/(long double)total_time)*100.,
                     ((long double) STATS(i).register_task_wake_up_costs/(long double)STATS(i).register_task_costs)*100.,
                     (long long unsigned) STATS(i).register_task_wake_up_costs / STATS(i).register_task_nb_calls);

            printf("\t  register tasks first cb access cost: %llu (%3.03Lf %%, relative %3.03Lf %%, %llu cycles)\n",
                     (long long unsigned) STATS(i).register_task_cb_first_access,
                     ((long double) STATS(i).register_task_cb_first_access/(long double)total_time)*100.,
                     ((long double) STATS(i).register_task_cb_first_access/(long double)STATS(i).register_task_costs)*100.,
                     (long long unsigned) STATS(i).register_task_cb_first_access / STATS(i).register_task_nb_calls);

            printf("\t  register tasks average duration: %llu cycles\n",
                     (long long unsigned) STATS(i).register_task_costs/(long long unsigned) STATS(i).register_task_nb_calls);
         }
      }
#endif

#ifdef TRACE_THREAD_WAKEUP
      printf( "\nWakeUp profiling:\n");
      for(int i=0 ; i< nthreads; i++){
         printf("* Thread %d:\n",i);

         printf("\t  Thread wake up nb calls: %llu\n",
                  (long long unsigned) STATS(i).thread_wakeup_calls);

         if(STATS(i).thread_wakeup_calls > 0){
            printf("\t  Thread wake up cost: %llu cycles (%3.03Lf %%, avg %llu cycles)\n",
                     (long long unsigned) STATS(i).thread_wakeup_costs,
                     ((long double) STATS(i).thread_wakeup_costs/(long double)total_time)*100.,
                     (long long unsigned) STATS(i).thread_wakeup_costs/STATS(i).thread_wakeup_calls);

            printf("\t  Notify calls: %llu\n",
                                 (long long unsigned) STATS(i).thread_wakeup_notify_calls);
            if(STATS(i).thread_wakeup_notify_calls){
               printf("\t  Notify cost: %llu cycles (%3.03Lf %%, avg %llu cycles)\n",
                        (long long unsigned) STATS(i).thread_wakeup_notify_costs,
                        ((long double) STATS(i).thread_wakeup_notify_costs/(long double)total_time)*100.,
                        (long long unsigned) STATS(i).thread_wakeup_notify_costs/STATS(i).thread_wakeup_notify_calls);
            }
         }
      }
#endif

#ifdef PROFILE_CORE_LOCK
      printf( "\nTime spent in core locks:\n");
      for(int i=0 ; i< nthreads; i++){
         printf( "\tThread %d: %llu (%3.02Lf %%)\n", i,
                  (long long unsigned) STATS(i).core_epoll_lock_cost,
                  ((long double) STATS(i).core_epoll_lock_cost/(long double)total_time)*100.);
      }
#endif

#ifdef PROFILE_CORE_EPOLL
      printf( "\nEpoll profiling:\n");
      for(int i=0 ; i< nthreads; i++){
         printf( "* Thread %d:\n", i);
         printf( "\tNb calls: %llu\n",
                  (long long unsigned) STATS(i).core_epoll_calls);

         if(STATS(i).core_epoll_calls){
            printf( "\tTime spent in epoll callback: %llu (%3.02Lf %%, avg %.02Lf)\n",
                     (long long unsigned) STATS(i).core_epoll_callback_cost,
                     ((long double) STATS(i).core_epoll_callback_cost/(long double)total_time)*100.,
                     (long double) STATS(i).core_epoll_callback_cost/(long double)STATS(i).core_epoll_calls);
            printf( "\tTime spent in epoll_wait (including sleep time): %llu (%.02Lf %%, avg %3.02Lf)\n",
                     (long long unsigned) STATS(i).core_epoll_wait_cost,
                     ((long double) STATS(i).core_epoll_wait_cost/(long double)total_time)*100.,
                     (long double) STATS(i).core_epoll_wait_cost/(long double)STATS(i).core_epoll_calls);
            printf( "\tTime spent in flushing waitpipe: %llu (%.02Lf %%, avg %3.02Lf)\n",
                     (long long unsigned) STATS(i).core_epoll_cleanpipe_cost,
                     ((long double) STATS(i).core_epoll_cleanpipe_cost/(long double)total_time)*100.,
                     (long double) STATS(i).core_epoll_cleanpipe_cost/(long double)STATS(i).core_epoll_calls);
            printf( "\tAvg epoll return: %3.02Lf\n",
                     (long double) STATS(i).core_epoll_avg_ret/(long double)STATS(i).core_epoll_calls);

            printf( "\tTime spent in epoll_add: %llu (%.02Lf %%)\n",
                     (long long unsigned) STATS(i).core_epoll_internal_add_cost,
                     ((long double) STATS(i).core_epoll_internal_add_cost/(long double)total_time)*100.);
            printf( "\tTime spent in epoll_remove: %llu (%.02Lf %%)\n",
                     (long long unsigned) STATS(i).core_epoll_internal_remove_cost,
                     ((long double) STATS(i).core_epoll_internal_remove_cost/(long double)total_time)*100.);
            printf( "\tTime spent in epoll_remove from fd_poll_check: %llu (%.02Lf %%)\n",
                                 (long long unsigned) STATS(i).core_epoll_reg_epoll_remove_cost,
                                 ((long double) STATS(i).core_epoll_reg_epoll_remove_cost/(long double)total_time)*100.);
            printf( "\tAverage time between two epoll callbacks: %.02Lf\n",
                                 (long double) STATS(i).core_epoll_time_between_two_calls/(long double)STATS(i).core_epoll_calls);
         }
      }
#endif

      if(get_bench_time())
         printf("\n");

      if(atexit_handler){
         atexit_handler();
      } else {
         exit(0);
      }
}

void register_atexit_handler(void (*fun)(void)){
   atexit_handler = fun;
}

/***********************************
 * 5/ Initialization of a thread
 ***********************************/
/** Initialize the queue of the thread **/
static void task_init_queue(int which_thread) {

   LOCK(which_thread);

   /** Creating the control list **/
   Task_List* cl = new Task_List();
   cl->dummy = 0;

   cl->first_task = new Task (-which_thread-1);
   cl->last_head_task = new Task (-which_thread-1);
   cl->last_task = new Task (-which_thread-1);

   cl->first_task->dummy = true;
   cl->last_head_task->dummy = true;
   cl->last_task->dummy = true;

   /** Make the initial link **/
   cl->first_task->next = cl->last_head_task;
   cl->last_head_task->prev = cl->first_task;
   cl->last_head_task->next = cl->last_task;
   cl->last_task->prev = cl->last_head_task;

   cl->next = NULL;
   cl->prev = NULL;

   cl->color = -which_thread-1;
   cl->default_prio = 0;
   cl->current_prio = 0;

   NEG_TL_ARRAY(which_thread) = cl;

   /** A the beginning, there's no task in the queue **/
   TASK_COUNT(which_thread) = 0;


   #if SORT_STEAL_LIST
   Task_List* high = new Task_List();
   high->dummy = 1;
   THREAD_STATE(which_thread).steal_color_list = high;
   THREAD_STATE(which_thread).steal_last_of_color_list_high = high;
   high->steal_prev = NULL;

   Task_List *med = new Task_List();
   med->dummy = 1;
   THREAD_STATE(which_thread).steal_last_of_color_list_medium = med;
   med->steal_prev = high;
   high->steal_next = med;

   Task_List *low = new Task_List();
   low->dummy = 1;
   THREAD_STATE(which_thread).steal_last_of_color_list_low = low;
   low->steal_next = NULL;
   low->steal_prev = med;
   med->steal_next = low;
   #endif

   UNLOCK(which_thread);
}

/** Initialize all thread and different mappings**/
void init_task_state() {
#if CE_WS
   init_shared_cache_vector();
#endif
   memset(thread_state,0,sizeof(thread_private_t)*MAX_THREADS);

#if PROFILING_SUPPORT
   memset(thread_stats,0,sizeof(thread_stats_t)*MAX_THREADS);
   printf("Size of thread_stats %lu bytes (x%d threads = %lu)\n",
              (unsigned long) sizeof(thread_stats_t),
              MAX_THREADS, (unsigned long) sizeof(thread_stats_t)*MAX_THREADS );
#endif

   printf("Size of thread_private %lu bytes (x%d threads = %lu)\n",
            (unsigned long) sizeof(thread_private_t),
            MAX_THREADS, (unsigned long) sizeof(thread_private_t)*MAX_THREADS );

   for (int i=0; i<MAX_THREADS; i++) {
      FREETASK_COUNT(i) = 0;
   }

   for (int i=0; i<MAX_COLORS; i++){
      int on_nthread = nthreads;
      COLOR_TO_QUEUE(i) = i % on_nthread;
      TL_ARRAY(i) = _create_tl(i, 0);
   }

   for (int i=0; i<nthreads; i++) {
      THREAD_STATE(i).num_unique_colors = 0;
   }

   for (int i=0; i<nthreads; i++) {
      task_init_queue(i);

#ifdef TRACE_MAPPING
      STATS(i).eh_stats = new std::map<void*,_eh_stats_t*>();
#endif
   }
}

/***********************************
 * Profiling stuff
 ***********************************/
#if PROFILING_SUPPORT
void _reset_stats(){
   DEBUG("Reseting stats of core %d\n", _thread_no);

   int how_many_maps = 0;

#ifdef TRACE_MAPPING
   how_many_maps++;
   THREAD_STATS.eh_stats->clear();
#endif

   uint64_t sizeto_memset = sizeof(_thread_stats) - (how_many_maps * sizeof(void*));
   memset(&THREAD_STATS, 0, sizeto_memset);

   DEBUG("TS: %lu bytes, Size to memset: %lu bytes\n", sizeof(_thread_stats), sizeto_memset);
}
#endif
void reset_runtime_stats(){
   #if PROFILING_SUPPORT
   rdtscll(start_cycle);

   for(int i = 0; i< nthreads; i++){
      if((unsigned int) i != _thread_no){
         DEBUG("Enqueuing monitor request to thread %d\n",i);
         // Max priority
         cpucb( cpwrap(_reset_stats, 0-(i+1), 65535) );
      }
      else{
         _reset_stats();
      }
   }
#endif
}

#if PROFILING_SUPPORT
void print_tasks_done(int seconds) {
   int i;
   unsigned int total = 0;
   for (i=0; i<nthreads; i++) {
      printf( "thread %d: %llu tasks\n", i, (long long unsigned) STATS(i).tasks_done);
      total += STATS(i).tasks_done;
   }

   printf( "total: %d\n", total);
   if (seconds)
      printf( "tasks/sec: %f\n", (float)total/(float)seconds);
}
#endif
