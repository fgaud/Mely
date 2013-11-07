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


#include "amisc.h"
#include "core_fdwatcher.h"
#include "task.lbc.h"

/*******************************************************************
 * Variables
 *******************************************************************/
#define fdwatcher_w fdwatcher_wait(current_proc)
timespec tsnow;                            /* Current time, updated by acheck_task */
const time_t &timenow = tsnow.tv_sec;
static itree<timespec, timecb_t, &timecb_t::ts, &timecb_t::link> timecbs, timecbs_active; /* RB tree containing delaycb-ed callbacks */
static bool timecbs_altered;               /* timecbs changed since last check (optim) */
static unsigned int nb_timecbs = 0;        /* Number of inserted callbacks in tree */

static sl_mutex_t cb_mu_global;            /* GLOBAL AND UGLY LOCK. AVOID IF POSSIBLE. */
static bool tasks_initialized = false;     /* initialization done? */

/*********************************************************************
 * Functions seen by userland
 *********************************************************************/
void cpucb(CBV_PTR_TYPE cb) {
   register_task_head(cb);
}

void cpucb_tail(CBV_PTR_TYPE cb) {
   register_task(cb);
}

/** [BACKPORTED] **/
int _make_async (int s) {
  int n;
  if ((n = fcntl (s, F_GETFL)) < 0
      || fcntl (s, F_SETFL, n | O_NONBLOCK) < 0)
    return -1;
  return 0;
}

void close_on_exec (int s) {
  if (fcntl (s, F_SETFD, 1) < 0){
    PANIC("F_SETFD: %s\n", strerror (errno));
  }
}
/** End [BACKPORTED] **/

/** Used by delay cb to memorize events **/
timecb_t * timecb(const timespec &ts, cbv cb) {
   timecb_t *to= new timecb_t (ts, cb);
   LOCK(&cb_mu_global);
   timecbs.insert(to);
   nb_timecbs++;
   // timecbs_altered = true;
   UNLOCK(&cb_mu_global);

#if !REMOVE_EPOLL_TIMEOUT
   wakeup_fdwatcher(color_to_thread(cb->getcolor()));
#endif //REMOVE_EPOLL_TIMEOUT

   return to;
}

/** Associate a timer and a callback **/
timecb_t * delaycb(time_t sec, u_int32_t nsec, cbv cb) {
   timespec ts;
   clock_gettime(CLOCK_REALTIME, &ts);
   ts.tv_sec += sec;
   ts.tv_nsec += nsec;
   if (ts.tv_nsec >= 1000000000) {
      ts.tv_nsec -= 1000000000;
      ts.tv_sec++;
   }
   return timecb(ts, cb);
}

/** Removed the timer previously inserted **/
void timecb_remove(timecb_t *to) {
   if (!to)
      return;
   LOCK(&cb_mu_global);
   timecb_t *tp;

   for (tp = timecbs_active[to->ts]; tp && tp != to && tp->ts == to->ts; tp= timecbs_active.next(tp));

   if (tp != to) {
      for (tp = timecbs[to->ts]; tp != to; tp = timecbs.next(tp))
         if (!tp || tp->ts != to->ts)
            PANIC ("timecb_remove: invalid timecb_t\n");

      timecbs_altered = true;
      timecbs.remove(to);
      free_callback(to->cb);
      delete to;
   } else {
      timecbs_active.remove(to);
      to->cancel = 1;
   }
   UNLOCK(&cb_mu_global);
}

/*********************************************************************
 * INTERNALS
 *********************************************************************
 * 1/ DELAYCB / TIMECB checks.
 * - do_timecb = execute callback
 * - the rest = look in  the RBtree for new cb to execute.
 *********************************************************************/
/** When a timer expired, it wraps this function which call the associated callback **/
static void do_timecb(struct timecb_t *tp) {
   LOCK(&cb_mu_global);
   if (!tp->cancel)
      timecbs_active.remove(tp);
   UNLOCK(&cb_mu_global);

   if (!tp->cancel)
      (*tp->cb)();

   free_callback(tp->cb);
   delete tp;
}

/** Check if a timer has expired.
 * If yes then wrap an event on do_timecb function **/
void timecb_check() {
   clock_gettime(CLOCK_REALTIME, &tsnow);
   timecb_t *tp, *ntp, *otp;
   struct timespec ots = {0,0};
   bool queued = false;

   LOCK(&cb_mu_global);
   otp = timecbs.first();
   for (tp = otp; tp && tp->ts <= tsnow; tp = timecbs_altered ? timecbs.first() : ntp) {
      ntp = timecbs.next(tp);
      timecbs.remove(tp);
      nb_timecbs--;

      timecbs_active.insert(tp);
      timecbs_altered = false;
      DEBUG("Registering a new task on color %d\n", tp->cb->getcolor());
      register_task(cpwrap(do_timecb, tp, tp->cb->getcolor(),
               tp->cb->getprio()));
      queued = true;
   }
   tp = otp;
   if(tp)
      ots = tp->ts;
   UNLOCK(&cb_mu_global);

#if !REMOVE_EPOLL_TIMEOUT
   int current_proc = get_current_proc();
   fdwatcher_w.tv_usec = 0;
   if (!tp)
      fdwatcher_w.tv_sec = 86400;
   else {
      clock_gettime(CLOCK_REALTIME, &tsnow);

      if (ots < tsnow){
         fdwatcher_w.tv_sec = 0;
      } else if (ots.tv_nsec >= tsnow.tv_nsec) {
         fdwatcher_w.tv_sec = ots.tv_sec - tsnow.tv_sec;
         fdwatcher_w.tv_usec = (ots.tv_nsec - tsnow.tv_nsec) / 1000;
      } else {
         fdwatcher_w.tv_sec = ots.tv_sec - tsnow.tv_sec - 1;
         fdwatcher_w.tv_usec = (1000000000 + ots.tv_nsec - tsnow.tv_nsec) / 1000;
      }
      DEBUG("[Core %d] ots: %ld.%ld ; tsnow = %ld.%ld => wait %ld.%ld \n", get_current_proc(),
               ots.tv_sec, ots.tv_nsec, tsnow.tv_sec, tsnow.tv_nsec, fdwatcher_w.tv_sec, fdwatcher_w.tv_usec);

   }
#endif //!REMOVE_EPOLL_TIMEOUT
}

/**
 * Call all the check methods to check if there is a timer event
 * a socket event or a signal event
 **/
static inline void _acheck() {
   int current_proc = get_current_proc();
   if(nb_timecbs){
      timecb_check();
   } else{
      //If no delaycb in tree, just put epoll timeout high
      fdwatcher_w.tv_sec = 86400;
   }
   fdcb_fdwatcher_check();
}

/**
 * This task check if there is new events (using _acheck)
 * and wrap a new event (acheck_task) to loopback
 **/
void acheck_task() {
   int current_proc = get_current_proc();
   _acheck();
   if (fdwatcher_gotany[current_proc]) {
#if !REMOVE_EPOLL_TIMEOUT
      wakeup_fdwatcher(current_proc);
#endif
      register_task(cwrap(acheck_task, -current_proc - 1));
   } else {
      register_task(cpwrap(acheck_task, -current_proc - 1, -100));
   }
}

/*********************************************************************
 * 2/ Runtime initialization
 *********************************************************************/
void dump_relevant_linux_parameters();

/** Initialize select pipe and add an handler for SIGCHLD */
static void ainit() {
   DEBUG("Core %d initializing his pipe !\n", get_current_proc());
   ainit_fdwatcher();
}

/** Initialize all threads **/
void ainitialize() {
   assert(!tasks_initialized);
   init_task_state();
   tasks_initialized = true;
}

/**
 * Main function
 * Must not be called recursively
 * Intializes pipe and other things thanks to ainit
 * register an initial task acheck_task
 * and start all threads
 **/
void amain() {
   static bool amain_called = false;
   if (amain_called) {
      for (int i = 0; i < task_get_nthreads(); i++) {
         register_task_head(cwrap(ainit, -i - 1)); //NEED HEAD
      }
   } else {
      amain_called = true;

      for (int i = 0; i < task_get_nthreads(); i++) {
         register_task_head(cwrap(ainit, -i - 1)); //NEED HEAD
         register_task(cwrap(acheck_task, -i - 1));
      }

      register_EH_name((void*) do_timecb, "[core.C] do_timecb");
      register_EH_name((void*) acheck_task, "[core.C] acheck_task");
      register_EH_name((void*) ainit, "[core.C] ainit");
   }

   task_go();
}


/**
 * Called before main !
 **/
int async_init::count;
void async_init::start() {
   static bool initialized;
   if (initialized)
      PANIC ("async_init called twice\n");
   initialized = true;

   int nb_procs;

#ifndef NB_THREADS
   nb_procs = get_nprocs();
#else
   nb_procs = NB_THREADS;
#endif

   assert( nb_procs > 0 );

   if(nb_procs != get_nprocs()){
      PRINT_ALERT("*** WARNING (%s,%d), nb_procs (%d) != get_nprocs() (%d)\n",__FILE__,__LINE__,nb_procs,get_nprocs());
   }

   task_set_nthreads(nb_procs);

   task_set_cpu_map(nb_procs);

   /* Ignore SIGPIPE, since we may get a lot of these */
   struct sigaction sa;
   bzero (&sa, sizeof (sa));
   sa.sa_handler = SIG_IGN;
   sigaction(SIGPIPE, &sa, NULL);

   init_fdwatcher();

#ifdef SYSTEM_INFO
   dump_relevant_linux_parameters();
#endif

#ifdef RUNTIME_INFO
   dump_runtime_parameters(maxfd);
#endif

   if (!tasks_initialized)
      ainitialize();

}

void async_init::stop() {
}

/*********************************************************************
 * 3/ Dump runtime information.
 * Should probably be declared elsewhere but who cares?
 *********************************************************************/
void do_proc_info(char *proc_file) {
   char proc_info[256];
   bzero(proc_info, 256);
   int fp;
   if ((fp = open(proc_file,O_RDONLY)) == NULL){
      PANIC("Error while opening %s\n",proc_file);
   }

   assert(read(fp, proc_info, 256)>0);

   // Per proc_file rules
   if(strncmp(proc_file,"/proc/sys/net/core/somaxconn",strlen(proc_file)) == 0 ){
      int value = atoi(proc_info);
      assert(value>0);
      if(value <= 1024){
         printf("* %-50s\t\e[01;31m%d\e[m\n", proc_file, value);
      } else{
         printf("* %-50s\t\e[01;34m%d\e[m\n", proc_file, value);
      }
   } else if(strncmp(proc_file,"/proc/sys/net/ipv4/tcp_max_syn_backlog",strlen(proc_file)) == 0){
      int value = atoi(proc_info);
      assert(value>0);
      if(value <= 1024){
         printf("* %-50s\t\e[01;31m%d\e[m\n", proc_file, value);
      } else{
         printf("* %-50s\t\e[01;34m%d\e[m\n", proc_file, value);
      }
   } else{
      printf("* %-50s\t%s", proc_file, proc_info);
   }

   close(fp);
}

void dump_relevant_linux_parameters(){
   printf( "\n### SYSTEM INFO ###\n");
   printf("* %-50s\trev-","svn revision"); fflush(stdout);
   int r = system("svn info | grep Revision |cut -c11-");


   printf("* %-50s\t%s\n","Glibc version",gnu_get_libc_version());
   printf("* %-50s\t%s\n","Glibc release",gnu_get_libc_release());

   printf("* %-50s\t%u.%u.%u\n","GCC version",__GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__);

#ifdef __x86_64__
   printf("* %-50s\t%s\n","Instruction set", "64 bits");
#else
   printf("* %-50s\t%s\n","Instruction set", "32 bits");
#endif

   printf("* %-50s\t","Debian version:"); fflush(stdout);
   r = system("cat /etc/debian_version 2>/dev/null");


   printf("* %-50s\t","cpu(s):"); fflush(stdout);
   r = system("cat /proc/cpuinfo 2>/dev/null | grep \"model name\" | head -n 1 | cut -c14-");

   printf("* %-50s\t","used cores"); fflush(stdout);
   r = system("cat /proc/interrupts 2>/dev/null | head -n 1 | cut -c12-");

   printf("* %-50s\t","uname"); fflush(stdout);
   r = system("uname -a 2>/dev/null");

   printf("* %-50s\t","date"); fflush(stdout);
   r = system("date 2>/dev/null");

   printf("* %-50s\t","ulimit -n"); fflush(stdout);
   r = system("ulimit -n 2>/dev/null");

   printf("\n");

   do_proc_info("/proc/sys/fs/file-max");
   do_proc_info("/proc/sys/net/core/netdev_max_backlog");
   do_proc_info("/proc/sys/net/core/optmem_max");
   do_proc_info("/proc/sys/net/core/rmem_default");
   do_proc_info("/proc/sys/net/core/rmem_max");
   do_proc_info("/proc/sys/net/core/somaxconn");
   do_proc_info("/proc/sys/net/core/wmem_default");
   do_proc_info("/proc/sys/net/core/wmem_max");
   do_proc_info("/proc/sys/net/ipv4/tcp_congestion_control");
   do_proc_info("/proc/sys/net/ipv4/tcp_ecn");
   do_proc_info("/proc/sys/net/ipv4/tcp_max_syn_backlog");
   do_proc_info("/proc/sys/net/ipv4/tcp_max_tw_buckets");
   do_proc_info("/proc/sys/net/ipv4/tcp_mem");
   do_proc_info("/proc/sys/net/ipv4/tcp_rmem");
   do_proc_info("/proc/sys/net/ipv4/tcp_wmem");
   do_proc_info("/proc/sys/net/ipv4/tcp_sack");
   do_proc_info("/proc/sys/net/ipv4/tcp_syncookies");
   do_proc_info("/proc/sys/net/ipv4/tcp_timestamps");

   printf( "###################\n\n");
}
