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

#ifndef CORE_FDWATCHER_H_
#define CORE_FDWATCHER_H_

#include "mely.h"
#include "task.lbc.h"
#include "itree.h"
#define PRIVATE __thread

void fdcb_fdwatcher_check();
void init_fdwatcher();
void ainit_fdwatcher();

typedef union __attribute__((__packed__,  __aligned__(CACHE_LINE_SIZE))) __timeval__t {
   timeval val;
   char __p[CACHE_LINE_SIZE + (sizeof(timeval) / CACHE_LINE_SIZE) * CACHE_LINE_SIZE]; \
} timeval_tt;
#define fdwatcher_wait(thread) _fdwatcher_wait[thread].val

extern int maxfd;
extern int fdwatcher_gotany[MAX_THREADS];
extern timeval_tt _fdwatcher_wait[MAX_THREADS];

#define CORE_STATS                    (thread_stats[get_current_proc()].val)

#ifdef PROFILE_CORE_LOCK
#define LOCK(lock_addr) \
   { \
      uint64_t lock_start, lock_stop; \
      rdtscll(lock_start); \
      \
      /** Executes the locking function **/ \
      sl_mutex_lock(lock_addr); \
      \
      rdtscll(lock_stop); \
      CORE_STATS.core_epoll_lock_cost += (lock_stop - lock_start); \
   }

#define UNLOCK(lock_addr)      \
   { \
      /** Executes the unlocking function **/ \
      sl_mutex_unlock(lock_addr); \
   }
#else
#define LOCK(lock_addr)        sl_mutex_lock(lock_addr)
#define UNLOCK(lock_addr)      sl_mutex_unlock(lock_addr)
#endif

struct timecb_t {
        timespec ts;
        cbv cb;
        itree_entry<timecb_t> link;
        int cancel;
        timecb_t(const timespec &t, cbv c) : ts(t), cb(c), cancel(0)  {
        }
};

typedef struct fdcb_list {
   struct fdcb_list* next;
   CBV_PTR_TYPE cb;
} fdcb_list_t;

typedef struct fdcol_list {
	struct fdcol_list* next;
	int fd;

}fdclo_list_t;

typedef struct fdbc_list_container {
   struct fdcb_list* head;
   struct fdcb_list* tail;
} fdbc_list_container_t;

typedef struct fd_list_container {
	struct fdcol_list* head;
	struct fdcol_list* tail;
}fd_list_container_t;
#endif /* CORE_FDWATCHER_H_ */
