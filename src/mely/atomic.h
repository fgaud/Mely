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

#ifndef SPINLOCK_H_
#define SPINLOCK_H_

#include <stdint.h>
#include <assert.h>

typedef volatile unsigned int lock_t;

inline unsigned int test_and_set(lock_t *addr) {
   register unsigned int _res = 1;

   __asm__ __volatile__(
            " xchg   %0, %1 \n"
            : "=q" (_res), "=m" (*addr)
            : "0" (_res));
   return _res;
}

inline void spinlock_lock(lock_t *addr) {
   while (*addr || test_and_set(addr));
}

inline void spinlock_unlock(lock_t *addr) {
   *addr = 0;
}

#endif /* SPINLOCK_H_ */
