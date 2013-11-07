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

#ifndef _TASK_COMMON_H
#define	_TASK_COMMON_H

#include <pthread.h>
#include <assert.h>
#include <poll.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <vector>
#include <map>
#include <sys/resource.h>
#include <errno.h>
#include <execinfo.h>
#include <sys/time.h>
using namespace std;

#include "mely.h"
#include "runtime_config.h"
#include "runtime_stats.h"
#include "ws_config.lbc.h"
#include "amisc.h"
#include "core.h"

extern int nthreads;
extern unsigned short cpu_map[MAX_THREADS];
extern struct rusage usage;

#if CE_WS
typedef struct sibling {
        int core;
        struct sibling *next;
} sibling;
extern sibling** shared_cache_vector;
#endif

#ifdef PROFILING_SUPPORT
static uint64_t start_cycle;
static uint64_t stop_cycle;
#endif


#define THREAD_STATE(thread_id)         (thread_state[thread_id].val)
#define FIRST_TASK(threadid)            (thread_state[threadid].val.first_task)
#define LAST_HEAD_TASK(threadid)        (thread_state[threadid].val.last_head_task)
#define LAST_TASK(threadid)             (thread_state[threadid].val.last_task)
#define FREETASK(threadid)              (thread_state[threadid].val.free_task)
#define FREETASK_COUNT(threadid)        (thread_state[threadid].val.freetask_count)
#define TASK_COUNT(threadid)            (THREAD_STATE(threadid).task_count)
#define COLOR_TO_QUEUE(color)           color_to_queue[color].val
#define ACOLOR(thread_no)               acolor[thread_no].val
#define SLEEPING(i)                     (THREAD_STATE(i).sleeping)

static sl_mutex_t task_mu[MAX_THREADS];
#define TASK_MU(threadid)               (task_mu[threadid])

#if TRACE_CONTENTION
#define LOCK(which_thread) \
   { \
      uint64_t lock_start, lock_stop; \
      rdtscll(lock_start); \
      \
      /** Executes the locking function **/ \
      sl_mutex_lock(&TASK_MU(which_thread)); \
      \
      rdtscll(lock_stop); \
      THREAD_STATS.time_in_contention += lock_stop - lock_start; \
      THREAD_STATS.lock_nb_calls ++; \
   }
#define LOCK_AND_REG_COST(which_thread, where_to_register){ \
   uint64_t lock_start, lock_stop; \
   rdtscll(lock_start); \
   \
   /** Executes the locking function **/ \
   sl_mutex_lock(&TASK_MU(which_thread)); \
   \
   rdtscll(lock_stop); \
   THREAD_STATS.time_in_contention += lock_stop - lock_start; \
   where_to_register += lock_stop - lock_start; \
   THREAD_STATS.lock_nb_calls ++; \
}
#define UNLOCK(which_thread) \
   { \
      /** Executes the unlocking function **/ \
      sl_mutex_unlock(&TASK_MU(which_thread)); \
   }
#else
#define LOCK(which_thread) \
   { \
      /** Executes the locking function **/ \
      sl_mutex_lock(&TASK_MU(which_thread)); \
   }
#define LOCK_AND_REG_COST(which_thread, where_to_register){ \
   uint64_t lock_start, lock_stop; \
   rdtscll(lock_start); \
   \
   /** Executes the locking function **/ \
   sl_mutex_lock(&TASK_MU(which_thread)); \
   \
   rdtscll(lock_stop); \
   where_to_register += lock_stop - lock_start; \
}
#define UNLOCK(which_thread) \
   { \
      /** Executes the unlocking function **/ \
      sl_mutex_unlock(&TASK_MU(which_thread)); \
   }
#endif


__attribute__ ((unused)) static void (*atexit_handler)(void) = (void(*)(void)) 0;
__attribute__ ((unused)) static void (*atempty_handler)(void) = (void(*)(void)) 0;


#ifdef HARDWARE_COUNTERS
   void start_hwc(int _thread_no);
   void init_hwc();
   void print_hwc_results();
   void read_hw_counters(int which_thread);
#else
#  define init_hwc()
#  define start_hwc(...)
#  define print_hwc_results()
#  define read_hw_counters(...)
#endif

extern unsigned long bench_time;
unsigned long get_bench_time();
void task_print_queue(int which_thread);
void dumpEventStates(int s);
void print_EH_name(void *EH);

#if CE_WS
int init_shared_cache_vector();
#endif
#endif	/* _TASK_COMMON_H */

