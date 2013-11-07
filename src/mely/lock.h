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
#ifndef __LIBASYNC__LOCK_H
#define __LIBASYNC__LOCK_H

#define TIMED_MUTEX	0
#define LOCK_BUS        "lock"
#include "atomic.h"
#include "pad.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
   PAD(lock_t) l;
} sl_mutex_t;
#define LOCK_CONTENT lock->l.val

typedef struct {
  volatile int v;
} sl_atomic_t;


static inline void sl_mutex_init (sl_mutex_t *lock);
static inline void sl_mutex_lock (sl_mutex_t *lock);
static inline void sl_mutex_unlock (sl_mutex_t *lock);
static inline int sl_mutex_trylock (sl_mutex_t *lock);

static inline  void
sl_mutex_init(sl_mutex_t *lock)
{
  LOCK_CONTENT = 0;
}

static inline void
sl_mutex_lock(sl_mutex_t *lock)
{
  while (test_and_set(&LOCK_CONTENT));
}

static inline int
sl_mutex_trylock(sl_mutex_t *lock)
{
  return test_and_set(&LOCK_CONTENT) ? 0 : 1;
}

static inline void
sl_mutex_unlock(sl_mutex_t *lock)
{
  LOCK_CONTENT = 0;
}

#ifdef __cplusplus
}
#endif

#endif /* __LIBASYNC__LOCK_H */
