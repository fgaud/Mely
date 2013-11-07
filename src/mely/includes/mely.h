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

#ifndef _ASYNC_ASYNC_H_
#define _ASYNC_ASYNC_H_ 1

#define MAX_COLORS                                      32764
#define MAX_THREADS      8

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <netinet/in.h>
#include <assert.h>
#include "callback_norefcount.h"                         /* Definitions of various wrap() functions */



struct timecb_t;
void amain ();                                             /* Start libasync */
void cpucb (CBV_PTR_TYPE);                                 /* Enqueue high priority task */
void cpucb_tail (CBV_PTR_TYPE);                            /* Enqueue task */
enum selop { selread = 0, selwrite = 1 };
void fdcb (int, selop, CBV_PTR_TYPE);                      /* Associate fd and callback */
void fdcb_finished(bool finished);                         /* Work to be done in a fd callback is done */
                                                           /* Allows to get to the next write cb. */
timecb_t *timecb (const timespec &ts, cbv cb);             /* Time -> callback */
timecb_t *delaycb (time_t sec, u_int32_t nsec, cbv cb);    /* Now + sec -> callback */
void timecb_remove (timecb_t *);                           /* Remove callback */

int get_current_color();
unsigned int get_current_proc();
int task_get_nthreads();
#define async_get_nthreads task_get_nthreads /*Legacy*/

void save_bench_time(unsigned long bench_time);
void dump_current_state(bool separation);
void register_EH_name(void* EH, char* name);

void register_atexit_handler(void (*fun)(void));            /* Register an handler which will be called at end */
void register_atempty_handler(void (*fun)(void));           /* Register an handler which will be called before sleeping */

void reset_runtime_stats();                                 /** Reset runtime stats (for all threads) **/
void stop_hw_counters();                                    /** Papi counter interface **/

void register_task_avg_duration(int avg_duration);          /** Tweak for steal **/


#define DEBUG(...)
#define DEBUG_TMP(msg, args...) \
   fprintf(stderr,"(%s,%d) " msg, __FUNCTION__, __LINE__, ##args)
#define PRINT_ALERT(msg, args...) \
   fprintf(stderr,"-------\t(%s,%d) " msg, __FUNCTION__, __LINE__, ##args);
#define PANIC(msg, args...) {\
   fprintf(stderr,"*******\t(%s,%d) " msg, __FUNCTION__, __LINE__, ##args); \
   exit(EXIT_FAILURE); \
}

#ifdef __x86_64__
#define rdtscll(val) { \
    unsigned int __a,__d;                                        \
    asm volatile("rdtsc" : "=a" (__a), "=d" (__d));              \
    (val) = ((unsigned long)__a) | (((unsigned long)__d)<<32);   \
}
#else
   #define rdtscll(val) __asm__ __volatile__("rdtsc" : "=A" (val))
#endif

#define reset_threads_queue() PANIC("Not implemented in MELY");
/** Async init, to avoid calling a init_async() in main... */
static class async_init {
  static int count;
  static int &cnt () { return count; }
  static void start ();
  static void stop ();
public:                 \
	async_init () {if (!cnt ()++) start ();}
  ~async_init () {if (!--cnt ()) stop ();}
} async_init;
#endif /* !_ASYNC_ASYNC_H_ */
