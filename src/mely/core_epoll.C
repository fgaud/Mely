/*
 *
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

/**
 * Core Epoll: takes care of fdcb internals.
 *
 * Good order to understand the logic:
 * - fdcb: register or remove a callback for a specific fd.
 * - fdcb_fdwatcher_check: use epoll to check for events on the registered fds.
 *      This function may block if it is the only function in the system (ie. incoming events can only come from the network).
 *      The timeout of fdcb_fdwatcher_check is specified in the _fdwatcher_wait[#core] array.
 * - wakeup_fdwatcher: wakeup a thread blocked in fdcb_fdwatcher_check.
 * - start_fd_poll_check: post callbacks after network events, called by fdcb_fdwatcher_check.
 * - do_fd_check: execute the callbacks posted by start_fd_poll_check.
 */

#include "amisc.h"
#include "core_fdwatcher.h"
#include "fdlim.h"

void start_fd_poll_check(int fd, selop op);

/*
 * Shared structures. May be manipulated concurrently.
 */
static const int fdsn = 2; /* fdcb(... read|write) => _2_ possibilities */
//static PRIVATE int epoll_fd = -1;                       /* epoll needs a fd. This is it. */
static int epoll_fd[MAX_THREADS];
static PRIVATE bool epoll_fd_initialized = false;
int maxfd; /* Max number of FDs; filled using fdlim.h */
int fdwatcher_gotany[MAX_THREADS]; /* 1 if the epoll of the core got something on an fd. */
/* If so, core.C will prioritize the next call to acheck_task */
timeval_tt _fdwatcher_wait[MAX_THREADS]; /* Timeout for epoll */
/*
 * Explaination: epoll may block. The blocked thread may be woken up on:
 * - network events (nothing to do: the epoll call returns)
 * - an event is posted by another thread on the blocked thread => need to wake up => done by writing in a pipe monitored by epoll
 */
static int selpipes[MAX_THREADS][2]; /* Used for waking up select */
static PAD(volatile int) _epoll_nowait[MAX_THREADS]; /* MANIPULATED BY ALL THREADS */
/* Tell a core that it should not wait on the epoll call. Makes the core discard _fdwatcher_wait */
#define epoll_nowait(thread) _epoll_nowait[thread].val  /* (easy access to _epoll_nowait) */
static PAD(volatile int) _epoll_active[MAX_THREADS]; /* MANIPULATED BY ALL THREADS */
/* Hack: avoid useless writes in selpipes */
#define epoll_active(thread) _epoll_active[thread].val  /* (easy access to _epoll_active) */
static sl_mutex_t wakeup_select_lock[MAX_THREADS]; /* Protects _epoll_nowait */

static sl_mutex_t epoll_queued_lock[fdsn]; /* Protects _epoll_queued */

//TODO: PAD these structures?
static fdbc_list_container_t *fdcbs[fdsn]; /* op,fd -> callback list */

static fd_list_container_t fdcol[MAX_COLORS][fdsn]; /* col-> file descriptor list */
static int* fd_to_core; /* file descriptor -> core */

static int * epoll_queued[fdsn]; /* fd -> callback already posted */

int PRIVATE init_private_stuff_done = 0;
void init_private_stuff()
{
  if (init_private_stuff_done)
    return;
  init_private_stuff_done = 1;

  int current_proc = get_current_proc();
  fdwatcher_wait(current_proc).tv_sec = 86400;
  fdwatcher_gotany[current_proc] = 0;
  epoll_nowait(current_proc) = 0;
  epoll_active(current_proc) = 0;
  epoll_fd[current_proc] = epoll_create(maxfd);
  epoll_fd_initialized = true;
}

#if !REMOVE_EPOLL_TIMEOUT
void wakeup_fdwatcher(int watcher)
{
  int write_in_the_pipe = 0;
  LOG_WAKEUP(
      sl_mutex_lock(&wakeup_select_lock[watcher]);
      epoll_nowait(watcher) = 1;
      if (epoll_active(watcher))
      {
        write_in_the_pipe = 1;
        epoll_active(watcher) = 0;
      }
      sl_mutex_unlock(&wakeup_select_lock[watcher]);
      if(write_in_the_pipe)
      {
        LOG_WAKEUP_NOTIFY(
            if(write(selpipes[watcher][1], "", 1))
            {};
        )
      }
  )
}
#endif

void _epoll_remove(int fd, selop op, int thread_no)
{
  LOG_EPOLL_REMOVE(
      struct epoll_event event;
      event.data.fd = fd;

      if(fdcbs[(op+1)%2][fd].head && !epoll_queued[(op+1)%2][fd])
      {
        if(op == selread)
        {
          event.events = EPOLLOUT;
        }
        else
        {
          event.events = EPOLLIN;
        }
        int ret = epoll_ctl(epoll_fd[thread_no],EPOLL_CTL_MOD,fd,&event);
        if(ret)
        {
          PANIC("Error when modifying fd %d op %d to the watched fd_set (errno=%d)\n",fd,op,errno);
        }
      }
      else
      {
        int ret = epoll_ctl(epoll_fd[thread_no],EPOLL_CTL_DEL,fd,NULL);
        if(ret)
        {
          PANIC("Error when removing fd %d op %d to the watched fd_set (errno=%d)\n",fd,op,errno);
        }
      }
  )
}

void fdcol_rm(int fd, selop op, int color)
{
  if (color >= 0 && fdcol[color][op].head)
  {
    fdcol_list* fdl = fdcol[color][op].head;
    fdcol_list* fdl_prev = NULL;
    do
    {
      if (fdl->fd != fd)
      {
        fdl_prev = fdl;
        fdl = fdl->next;
      }
      else
      {
        if (fdl == fdcol[color][op].head)
        {
          fdcol[color][op].head = fdl->next;
        }
        else
        {
          fdl_prev->next = fdl->next;
        }
        if (fdl == fdcol[color][op].tail)
        {
          fdcol[color][op].tail = fdl_prev;
        }
        free(fdl);
        break;
      }
    } while (fdl != NULL);
  }
}
void _epoll_add(int fd, selop op)
{
  LOG_EPOLL_ADD(
      struct epoll_event event;
      event.data.fd = fd;

      int add_or_mod_epoll = EPOLL_CTL_ADD;

      if(op == selread)
      {
        if(fdcbs[selwrite][fd].head && !epoll_queued[selwrite][fd])
        {
          event.events = EPOLLIN | EPOLLOUT;
        }
        else
        {
          event.events = EPOLLIN;
        }
      }
      else
      {
        if(fdcbs[selread][fd].head && !epoll_queued[selread][fd])
        {
          event.events = EPOLLIN | EPOLLOUT;
        }
        else
        {
          event.events = EPOLLOUT;
        }
      }
      int ret = epoll_ctl(epoll_fd[_thread_no], add_or_mod_epoll, fd, &event);
      if(ret)
      {
        add_or_mod_epoll = EPOLL_CTL_MOD;
        ret = epoll_ctl(epoll_fd[_thread_no], add_or_mod_epoll, fd, &event);
        if(ret)
        {
          PANIC("Error whith fd %d to the watched fd_set (error is %d)\n", fd, errno);
        }
      }

  )
}

void fdcol_add(int fd, selop op, int color)
{
  if (color >= 0)
  {
    fdcol_list* fdl = (fdcol_list*) malloc(sizeof(fdcol_list));
    fdl->fd = fd;
    fdl->next = NULL;

    if (fdcol[color][op].head)
    {
      fdcol_list* fdl_i = fdcol[color][op].head;
      bool is_present = false;
      do
      {
        if (fdl_i->fd == fd)
        {
          is_present = true;
          break;
        }
        fdl_i = fdl_i->next;
      } while (fdl_i != NULL);
      if (!is_present)
      {
        fdcol[color][op].tail->next = fdl;
        fdcol[color][op].tail = fdl;
      }
      else
      {
        free(fdl);
      }
    }
    else
    {
      fdcol[color][op].head = fdl;
      fdcol[color][op].tail = fdl;
    }
  }
}

void _epoll_steal(int color, int victim_number)
{
  sl_mutex_lock(&epoll_queued_lock[selread]);
  if (fdcol[color][selread].head)
  {

    fdcol_list *fdc = fdcol[color][selread].head;
    do
    {
      int fd = fdc->fd;
      fdc = fdc->next;
      if (!epoll_queued[selread][fd])
      { // pas besoin, si couleur queued couleur pas dans liste
        _epoll_remove(fd, selread, victim_number);
        _epoll_add(fd, selread);
      }
      fd_to_core[fd] = _thread_no;
    } while (fdc != NULL);

  }
  sl_mutex_unlock(&epoll_queued_lock[selread]);
  sl_mutex_lock(&epoll_queued_lock[selwrite]);
  if (fdcol[color][selwrite].head)
  {

    fdcol_list* fdc = fdcol[color][selwrite].head;
    do
    {
      int fd = fdc->fd;
      fdc = fdc->next;
      if (!epoll_queued[selread][fd])
      {
        _epoll_remove(fd, selwrite, victim_number);
        _epoll_add(fd, selwrite);
      }
      fd_to_core[fd] = _thread_no;
    } while (fdc != NULL);

  }
  sl_mutex_unlock(&epoll_queued_lock[selwrite]);
}
/**
 * Add or remove a fd.
 */
void fdcb(int fd, selop op, CBV_PTR_TYPE cb)
{
  if (!epoll_fd_initialized)
  {
    init_private_stuff();
  }
  assert (fd >= 0);
  assert (fd < maxfd);

  if (fdcbs[op][fd].head && fdcbs[op][fd].head->cb == cb)
  {
    PANIC("Reposting the same callback twice is not authorized.\n");
  }

  if (cb)
  {
    fdcb_list_t *new_task = (fdcb_list_t *) malloc(sizeof(*new_task));
    new_task->next = NULL;
    new_task->cb = cb;
    if (!fdcbs[op][fd].head)
    {
      fdcbs[op][fd].head = fdcbs[op][fd].tail = new_task;
    }
    else
    {
      fdcbs[op][fd].tail->next = new_task;
      fdcbs[op][fd].tail = new_task;
    }
    if (!epoll_queued[op][fd])
    {
      if (fd == selpipes[_thread_no][0])
      {
        _epoll_add(fd, op);
      }
      else
      {
        _epoll_add(fd, op);
        fdcol_add(fd, op, cb->getcolor());
      }
#if STEALING
      fd_to_core[fd] = _thread_no;
#endif
    }
  }
  else
  {
    if (fdcbs[op][fd].tail && !epoll_queued[op][fd])
    {
      _epoll_remove(fd, op, _thread_no);
#if STEALING
      fdcol_rm(fd, op, fdcbs[op][fd].tail->cb->getcolor());
#endif
    }
    while (fdcbs[op][fd].head)
    {
      free_callback(fdcbs[op][fd].head->cb);
      void *to_free = fdcbs[op][fd].head;
      fdcbs[op][fd].head = fdcbs[op][fd].head->next;
      free(to_free);
    }
    fdcbs[op][fd].tail = NULL;
  }
}

/*
 * Execute a callback posted by start_fd_poll_check on the correct core.
 * Reactivate the epoll event if needed.
 */
static PRIVATE int reenqueue_cb;
void do_fd_check(int fd, selop op, CBV_PTR_TYPE cb)
{
  if (!cb)
  {
    PANIC("Empty callback !\n");
  }
  reenqueue_cb = 1;

  LOG_MAPPING(
      (*cb)(); //at this point cb can be deleted so don't use it anymore
      //if cb calls fdcb(NULL)...
  )

  epoll_queued[op][fd] = 0;
  if (!reenqueue_cb)
  {
    free_callback(cb);
    void *to_free = fdcbs[op][fd].head;
    fdcbs[op][fd].head = fdcbs[op][fd].head->next;
    free(to_free);
    if (!fdcbs[op][fd].head)
      fdcbs[op][fd].tail = NULL;
  }
  if (fdcbs[op][fd].head)
  { /* Re-add the fd */
#if STEALING
    fd_to_core[fd] = _thread_no;
#endif
    _epoll_add(fd, op);
    fdcol_add(fd, op, fdcbs[op][fd].head->cb->getcolor());
  }
#if STEALING
  else if ((!fdcbs[selread][fd].head) && (!fdcbs[selwrite][fd].head))
  {
    fd_to_core[fd] = -1;
  }
#endif
}
void fdcb_finished(bool finished)
{
  reenqueue_cb = !finished;
}

/*
 * Post the callback attached to the fd who just got out of epoll in fdcb_fdwatcher_check.
 * Suppress the callback event to avoid having the callback posted twice.
 */
void start_fd_poll_check(int fd, selop op)
{
  LOG_START_FD_POLL_CHECK(
#if STEALING
      sl_mutex_lock(&epoll_queued_lock[op]);
      if(fd_to_core[fd]==_thread_no)
      {
#endif
        if(fdcbs[op][fd].head != NULL)
        {
          epoll_queued[op][fd] = 1;
          CBV_PTR_TYPE cb = fdcbs[op][fd].head->cb;
          LOG_REMOVE_COST(
              _epoll_remove(fd,op, _thread_no);
#if STEALING
              fdcol_rm(fd,op,cb->getcolor());
#endif
          )
#if STEALING
          sl_mutex_unlock(&epoll_queued_lock[op]);
#endif
#if TRACE_REGISTER_TASK
          THREAD_STATS.register_task_call_from_epoll++;
#endif
          register_task(cpwrap(do_fd_check, fd, op, cb, cb->getcolor(), cb->getprio()));

        }
        else
        {
          PANIC("%d start_fd_poll_check on a removed fd %d op %d\n", _thread_no, fd, op);
        }
#if STEALING
      }
      else
      {
        sl_mutex_unlock(&epoll_queued_lock[op]);
      }
#endif
  )
}

/*
 * Wait for events in epoll_wait
 */
void fdcb_fdwatcher_check(void)
{
  LOG_FDWATCHER_CHECK(
      int current_proc = get_current_proc();

#if !REMOVE_EPOLL_TIMEOUT
      int epoll_timeout = -1;
      sl_mutex_lock(&wakeup_select_lock[current_proc]);
      epoll_active(current_proc) = 1;

      if (epoll_nowait(current_proc))
      {
        epoll_timeout = 0;
        epoll_nowait(current_proc) = 0;
      }
      else
      {
        epoll_timeout = (fdwatcher_wait(current_proc).tv_sec * 1000) + (fdwatcher_wait(current_proc).tv_usec / 1000);
      }
      sl_mutex_unlock(&wakeup_select_lock[current_proc]);
#endif //!REMOVE_EPOLL_TIMEOUT
      int n; /* Don't change the name, it's used by LOG_EPOLL_WAIT_TIME... */
      struct epoll_event events[maxfd];
      while(1){
         LOG_EPOLL_WAIT_TIME(
   #if REMOVE_EPOLL_TIMEOUT
             n = epoll_wait(epoll_fd[_thread_no],events,maxfd,0);
   #else //REMOVE_EPOLL_TIMEOUT
             n = epoll_wait(epoll_fd[_thread_no],events,maxfd,epoll_timeout);
   #endif //REMOVE_EPOLL_TIMEOUT
         )

         //Handle debug signal
         if(n<0 && errno==EINTR){
               DEBUG("epoll_wait interrupted by EINTR\n");
               continue;
         }else if(n<0){
            perror("epoll_wait");
            exit(EXIT_FAILURE);
         }
         else{
             break;
         }
      }//end while(1)

      if(n>0)
      {
        fdwatcher_gotany[get_current_proc()] = 1;
      }
      else
      {
        fdwatcher_gotany[get_current_proc()] = 0;
      }

      epoll_active(current_proc) = 0;

      LOG_PIPE_CLEANING(
#if !REMOVE_EPOLL_TIMEOUT
          char buf[64];
          while( read( selpipes[current_proc][0], buf, sizeof(buf) ) > 0 ); //Cleaning select wakeup pipe
#endif
      )

      for (int i = 0; i < n; i++)
      {
        int fd = events[i].data.fd;

        DEBUG("Found activity on socket %d\n", fd);

#if !REMOVE_EPOLL_TIMEOUT
        if (fd != selpipes[current_proc][0])
        { /* The wakeup pipe has no event. */
#endif
          int err = events[i].events & (EPOLLERR);
          events[i].events = events[i].events & (EPOLLIN | EPOLLOUT);

          if(events[i].events == EPOLLIN)
          {
            start_fd_poll_check(fd,selread);
          }
          else if(events[i].events == EPOLLOUT)
          {
            start_fd_poll_check(fd,selwrite);
          }
          else if (events[i].events == (EPOLLOUT | EPOLLIN))
          {
            start_fd_poll_check(fd,selread);
            start_fd_poll_check(fd,selwrite);
          }
          else if(err)
          {
            // Notify all owners
            if(fdcbs[selwrite][fd].head)
            {
              start_fd_poll_check(fd,selwrite);
            }
            if(fdcbs[selread][fd].head)
            {
              start_fd_poll_check(fd,selread);
            }
          }
          else
          {
            PRINT_ALERT("EPOLLIN: %d\nEPOLLPRI: %d\nEPOLLOUT: %d\nEPOLLERR: %d\nEPOLLHUP: %d\n",
                EPOLLIN,EPOLLPRI,EPOLLOUT,EPOLLERR,EPOLLHUP);
            PRINT_ALERT("Unknow EPOLL state : %d\n",events[i].events);
            exit(EXIT_FAILURE);
          }

          //We wake up select here because register_task_affinity is in task.C and can't wake up select.
          //wakeup_select();
#if !REMOVE_EPOLL_TIMEOUT
        }
#endif
      }
  )
}

void ignore_void()
{
}
;
callback<void> * cbv_null(wrap(ignore_void));

void ainit_fdwatcher()
{
  init_private_stuff();
#if !REMOVE_EPOLL_TIMEOUT
  int i = get_current_proc();
  if (pipe(selpipes[i]) < 0)
  {
    PANIC("Could not create selpipes\n");
  }
  _make_async(selpipes[i][0]);
  _make_async(selpipes[i][1]);
  close_on_exec(selpipes[i][0]);
  close_on_exec(selpipes[i][1]);
  register_EH_name((void*) ignore_void, "[core.C] cbv_null");
  fdcb(selpipes[i][0], selread, cbv_null);
#endif //!REMOVE_EPOLL_TIMEOUT
  register_EH_name((void*) do_fd_check, "[core.C] do_fd_check");
}

void init_fdwatcher()
{
  maxfd = fdlim_get(0);
  maxfd = (maxfd / 2 < 1024) ? maxfd : (maxfd / 2);
#if STEALING
  fd_to_core = (int*) calloc(maxfd, sizeof(int));
#endif
  for (int i = 0; i < fdsn; i++)
  {
    fdcbs[i] = (fdbc_list_container_t *) calloc(maxfd, sizeof(*fdcbs[i]));
    assert(fdcbs[i]);

    epoll_queued[i] = (int *) calloc(maxfd, sizeof(int));
    assert(epoll_queued[i]);
  }
}

