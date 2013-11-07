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

unsigned short cpu_map[MAX_THREADS];
struct rusage usage; //Page table monitoring (Big brother is watching you)
extern thread_private_t thread_state[MAX_THREADS];

/* Register a function name inside the runtime. Could be done automatically but it sometimes segfault... */
static sl_mutex_t lock_EH_name;
static std::map<void*, char*> eh_name;
void print_EH_name(void *EH) {
   sl_mutex_lock(&lock_EH_name);
   printf("%-30s",eh_name[EH]);
   sl_mutex_unlock(&lock_EH_name);
}
void register_EH_name(void* EH, char* name){
   sl_mutex_lock(&lock_EH_name);
   eh_name[EH] = name;
   sl_mutex_unlock(&lock_EH_name);
}

/* Bench time, to compute stats... */
unsigned long bench_time = 0;
unsigned long get_bench_time() {
   return bench_time;
}
void save_bench_time(unsigned long bt){
   bench_time = bt;
}

/*Signal handler. */
void task_print_queue_nolock(int i);
void dumpEventStates(int s){
        printf("=========================================================\n");
        int i;
        for(i=0; i<nthreads; i++){
                task_print_queue_nolock(i);
        }
        printf("=========================================================\n");
        exit(EXIT_SUCCESS);
}

void _dump_current_state(bool separation);
void monitor_yourself(bool separation){
   read_hw_counters(_thread_no);
   DEBUG("(%s,%d) Thread %d get hwc, sending answer to : 0\n", __FUNCTION__,__LINE__,_thread_no);
   cpucb(cpwrap(_dump_current_state,separation, -1, 65530));
}
void dump_current_state(bool separation){
   for(int i = 0; i< nthreads; i++){
      cpucb( cpwrap(monitor_yourself, separation, 0-(i+1), 65535) );
   }
}
int nb_dump_ok_msg = 0;
void _dump_current_state(bool separation) {
   nb_dump_ok_msg ++;
   if(nb_dump_ok_msg == nthreads) {
      printf("%.6f ", (double)bench_time/1000000);

      if (getrusage(RUSAGE_SELF, &usage) <0 ) {
         perror("getrusage error");
      }
      printf("PAGE_FAULTS_MIN %ld PAGE_FAULTS_MAJ %ld ",usage.ru_minflt,usage.ru_majflt);

#ifdef HARDWARE_COUNTERS
      print_hwc_results ();
#endif //HARDWARE_COUNTERS
      printf("\n");
      nb_dump_ok_msg = 0;

      if(separation){
         printf("---\n");
      }
   }
}


/* PAPI */
#ifdef HARDWARE_COUNTERS
int *EventSet;
long long **hwc_per_thread;
#define NB_EVENTS 2
int events[NB_EVENTS]={
                /*L1*/
                /*PAPI_L1_ICA, PAPI_L1_ICM,
                PAPI_L1_DCA, PAPI_L1_DCM,*/

                /*L2*/
                //PAPI_L2_ICA, PAPI_L2_ICM,
                PAPI_L2_TCA, PAPI_L2_TCM,

                /*TLB*/
                //PAPI_TLB_DM, PAPI_TLB_IM
};

void print_hwc_results (){
   for(int i = 0; i<NB_EVENTS;i++) {
      char event_name[PAPI_MAX_STR_LEN];
      if( PAPI_event_code_to_name(events[i], event_name) != PAPI_OK ){
         printf("cant PAPI_event_code_to_name\n");
         exit(0);
      }
      printf("%s ", event_name);
   }
   for(int i = 0; i<nthreads;i++){
      long long * result_thread = hwc_per_thread[i];
      printf("T%d ", i);
      for(int j = 0; j<NB_EVENTS; j++){
         printf("%lld ", result_thread[j]);
      }
   }
}

void start_hwc(int _thread_no) {
   start_hardware_counter(_thread_no,(EventSet+_thread_no),events,NB_EVENTS);
}

void stop_hwc(int _thread_no) {
   stop_hardware_counters(_thread_no,EventSet+_thread_no,NB_EVENTS,hwc_per_thread[_thread_no]);
}

void read_hw_counters(int which_thread) {
   read_hardware_counters(which_thread,EventSet +which_thread,NB_EVENTS,hwc_per_thread[which_thread]);
}

void init_hwc() {
   int nthreads = task_get_nthreads();
   EventSet = (int *) malloc(nthreads * sizeof (int));
   for (int i = 0; i < nthreads; i++) {
      EventSet[i] = PAPI_NULL;
   }

   hwc_per_thread = (long long **) malloc(nthreads * sizeof (long long **));
   assert(hwc_per_thread != NULL);

   for (int i = 0; i < nthreads; i++) {
      hwc_per_thread[i] = (long long *) malloc(sizeof (long long) * NB_EVENTS);
      assert(hwc_per_thread[i] != NULL);
   }

   init_hardware_counters();
}
#else
#  define read_hardware_counters(...)
#  define read_hw_counters(...)
#  define stop_hwc(...)
#endif //HARDWARE_COUNTERS

void stop_hw_counters(){
   DEBUG("Thread %d (%lud) - Stoping hwc\n",_thread_no,pthread_self());
   stop_hwc(_thread_no);

   if (getrusage(RUSAGE_SELF, &usage) <0 ){
      perror("getrusage error");
   }
   pthread_exit(0);
}


/**
 * This function is used to print runtime configuration
 * Maxfd is supplied by core.C
 **/
void dump_runtime_parameters(int maxfd){
   printf("*** Runtime parameters ***\n");
   printf("Runtime :\n");
   printf("\tNthreads = %d\n", task_get_nthreads());
   printf("\tMAX_THREADS = %d\n",MAX_THREADS);
   printf("\tUSE_ONE_LIST_PER_COLOR = true\n");
   printf("\t\tNB_TASKS_FROM_SAME_COLOR_THRES = %d\n", NB_TASKS_FROM_SAME_COLOR_THRES);


   printf("\nSynchro : \n");
   printf("\tWAIT METHOD = %s\n", (REMOVE_EPOLL_TIMEOUT)?("SPINLOOP"):("EPOLL_WAIT"));

   printf("\nMemory :\n");
   printf("\tMAX_FREETASKS = %d\n",MAX_FREETASKS);

   printf("\nEpoll / Select :\n");
   printf("\tNET_FLAGS = -ONE_EPOLL_PER_CORE");
   printf("\n\tREMOVE_EPOLL_TIMEOUT = %d\n", REMOVE_EPOLL_TIMEOUT);
   printf("\tMax fd size = %d\n",maxfd);

   printf("\nProfiling :\n");
   printf("\tDEBUG_FLAGS =");
#ifdef SYSTEM_INFO
   printf(" -DSYSTEM_INFO");
#endif
#ifdef RUNTIME_INFO
   printf(" -DRUNTIME_INFO");
#endif
#ifdef TRACE_WORKSTEALING
   printf(" -DTRACE_WORKSTEALING");
#endif
#ifdef TRACE_MAPPING
   printf(" -DTRACE_MAPPING");
#endif
#ifdef TRACE_CONTENTION
   printf(" -DTRACE_CONTENTION");
#endif
#ifdef PROFILE_CORE_LOCK
   printf(" -DPROFILE_CORE_LOCK");
#endif
#ifdef PROFILE_CORE_EPOLL
   printf(" -DPROFILE_CORE_EPOLL");
#endif
#ifdef TRACE_REGISTER_TASK
   printf(" -DTRACE_REGISTER_TASK");
#endif
#ifdef TRACE_THREAD_WAKEUP
   printf(" -DTRACE_THREAD_WAKEUP");
#endif

printf("\n\tHWC_FLAGS =");
#ifdef HARDWARE_COUNTERS
   printf(" -DHARDWARE_COUNTERS");
#endif

   printf("\n\nSTEALING = %d\n",STEALING);
#if STEALING
   printf("\tWS_FUNCTION = %s\n","steal_work");
   printf("\tCE_WS = %d\n",CE_WS);
   printf("\tUSE_BATCH_WS = %d\n",USE_BATCH_WS);
   USE_BATCH_WS && printf("\t\tBATCH_TASK_WS = %2.2f\n",BATCH_TASK_WS);
   printf("\tTIME_LEFT_WORKSTEALING = %d\n",TIME_LEFT_WORKSTEALING);
#if TIME_LEFT_WORKSTEALING
   printf("\t\tMUST_STEAL_THRESHOLD = %d\n",MUST_STEAL_THRESHOLD);
   printf("\t\tSORT_STEAL_LIST = %d\n",SORT_STEAL_LIST);
#if SORT_STEAL_LIST
   printf("\t\t\tMEDIUM_THRES = %d\n",MEDIUM_THRES);
   printf("\t\t\tHIGH_THRES = %d\n",HIGH_THRES);
#endif
#endif

   printf("\tRESET_COLOR_ON_EMPTY_QUEUE = %d\n", RESET_COLOR_ON_EMPTY_QUEUE);
#endif //STEALING

   printf("\n**************************\n\n");
}

/***********************************
 * Stuff to print the state of the runtime
 ***********************************/
/**
 * Print the state of the thread
 * Don't use lock (could be partially wrong and segfault...)
 **/
void _print_task(Task_List* tl){
   fprintf(stderr,"Color %d, prio %d (default %d), nb_elem %d, address %p : ",
            tl->color,tl->current_prio,tl->default_prio,tl->nb_callbacks, tl);

   Task* t = tl->first_task;
   while (t) {
      if (t == tl->first_task)
         fprintf(stderr," <FT>");
      else if (t == tl->last_head_task)
         fprintf(stderr," <LH>");
      else if (t == tl->last_task)
         fprintf(stderr," <LT>");
      else
         fprintf(stderr," %p [handler %p, dummy=%s, color=%d]",
                  t,
                  t->cb->getfaddr(),
                  t->dummy ? "true" : "false",
                           t->color);

      t = t->next;
   }
   fprintf(stderr,"\n");
}
void task_print_steal_queue(int which_thread) {
   fprintf(stderr, "\n Thread %d - Steal list : \n", which_thread);
   Task_List * tl = THREAD_STATE(which_thread).steal_color_list;
   while (tl) {
      #if TIME_LEFT_WORKSTEALING
      fprintf(stderr, "- Color %d (%llu cycles)\n", tl->color, (long long unsigned)tl->total_processing_duration);
      #else
      fprintf(stderr, "- Color %d (?? cycles)\n", tl->color);
      #endif
      tl = tl->steal_next;
   }
}
void task_print_queue_nolock(int which_thread) {
   fprintf(stderr,"\nThread %d - Tasks : \n", which_thread);

   Task_List * tl = THREAD_STATE(which_thread).color_list;
   while(tl != NULL){
      _print_task(tl);
      tl = tl->next;
   }
   fprintf(stderr,"Current active color is %d\n", get_current_color());
   fprintf(stderr,"Num unique color is %d\n",THREAD_STATE(which_thread).num_unique_colors);
   fprintf(stderr,"Nb tasks = %d\n",TASK_COUNT(which_thread));
}
/**
 * Print the state of the thread
 * Use the thread mutex - Could be costly
 **/
void task_print_queue(int which_thread) {
        LOCK(which_thread);
        task_print_queue_nolock(which_thread);
        UNLOCK(which_thread);
}



#if CE_WS
sibling** shared_cache_vector;
int init_shared_cache_vector() {
   shared_cache_vector = (sibling**) calloc(nthreads, sizeof(*shared_cache_vector));

   /*
   int order[][15] = {
	   { 4, 8, 12,   3, 7, 11, 15,    2, 6, 10, 14,    1, 5, 9, 13 },
	   { 5, 9, 13,   2, 6, 10, 14,    3, 7, 11, 15,    0, 4, 8, 12 },
	   { 6, 10, 14,  1, 5, 9, 13,     0, 4, 8, 12,     3, 7, 11, 15 },
	   { 7, 11, 15,  0, 4, 8, 12,     1, 5, 9, 13,     2, 6, 10, 14 },

	   { 8, 12, 0,   7, 11, 15, 3,    6, 10, 14, 2,    5, 9, 13, 1 },
	   { 9, 13, 1,   6, 10, 14, 2,    7, 11, 15, 3,    4, 8, 12, 0 },
	   { 10, 14, 2,  5, 9, 13, 1,     4, 8, 12, 0,     7, 11, 15, 3 },
	   { 11, 15, 3,  4, 8, 12, 0,     5, 9, 13, 1,     6, 10, 14, 2 },

	   { 12, 0, 4,   11, 15, 3, 7,    10, 14, 2, 6,    9, 13, 1, 5 },
	   { 13, 1, 5,   10, 14, 2, 6,    11, 15, 3, 7,    8, 12, 0, 4 },
	   { 14, 2, 6,   9, 13, 1, 5,     8, 12, 0, 4,     11, 15, 3, 7 },
	   { 15, 3, 7,   8, 12, 0, 4,     9, 13, 1, 5,     10, 14, 2, 6 },

	   { 0, 4, 8,    15, 3, 7, 11,    14, 2, 6, 10,    13, 1, 5, 9 },
	   { 1, 5, 9,    14, 2, 6, 10,    15, 3, 7, 11,    12, 0, 4, 8 },
	   { 2, 6, 10,   13, 1, 5, 9,     12, 0, 4, 8,     15, 3, 7, 11 },
	   { 3, 7, 11,   12, 0, 4, 8,     13, 1, 5, 9,     14, 2, 6, 10 },
   };
   */
   int order[][3] = {
            { 1, 2, 3 },
            { 2, 3, 0 },
            { 3, 0, 1 },
            { 0, 1, 2 },
   };
   if(nthreads != 4) {
      PANIC("ERROR: the CE_WS neighbors array is currently hard coded for 4 cores. Either update the function or create a more omniscient sibling finder.");
   }
   for(int j = 0; j < nthreads; j++) {
      sibling* current_sibling = (sibling*) malloc(sizeof(*current_sibling));
      shared_cache_vector[j] = current_sibling;
      for(int k=0; k < nthreads-1; k++) {
	      current_sibling->core = order[j][k];
	      if(k!=nthreads-2) {
		      current_sibling->next = (sibling*) malloc(sizeof(*current_sibling));
		      current_sibling = current_sibling->next;
	      } else {
		      current_sibling->next = NULL;
	      }
      }
   }
   return 0;
}
#endif

/** creates the cpu map from which we map each thread on his cpu **/
void task_set_cpu_map (int nthreads) {
   cpu_set_t set;
   CPU_ZERO(&set);
   int cpu = 0;
   int ret = sched_getaffinity(getpid(),sizeof(cpu_set_t), &set);
   assert(ret != -1);
   DEBUG("CREATING CPU_MAP\n");

   for (unsigned int i=0;i<sizeof(cpu_set_t);i++) {
      if ( CPU_ISSET(i,&set) ) {
         cpu_map[cpu] = i;
         cpu++;
         if (cpu == nthreads)
            break;
      }
   }
}
