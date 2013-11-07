// -*-c++-*-
/* $Id: amisc.h,v 1.26 2001/08/23 21:29:32 kaminsky Exp $ */

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
#ifndef _ASYNC_AMISC_H_
#define _ASYNC_AMISC_H_ 1

#include <stdio.h>
#include <unistd.h>
#include <typeinfo>
#include <errno.h>
#include <time.h>
#include <sys/file.h>
#include <sys/sysinfo.h>
#include <sys/wait.h>
#include <gnu/libc-version.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <pthread.h>
#include "mely.h"
#include "pad.h"
#include "atomic.h"
#include "lock.h"
#include "keyfunc.h"
#include "runtime_config.h"
#include "runtime_stats.h"
#include "callback_norefcount.h"
#ifdef HARDWARE_COUNTERS
#include <papi.h>
#include "bench_papi.h"
#endif

extern timespec tsnow;
extern const time_t &timenow;

void ainitialize();
void acheck_task();
void acheck_task_norepeat();

extern cbv cbv_null;
extern cbi cbi_null;

void close_on_exec (int s);
int _make_async (int);
int color_to_thread(int color);
void _epoll_steal(int color, int victim_number);

/*
 * Other random stuff
 */
#ifdef __cplusplus
/* Egcs g++ without '-ansi' has some strange ideas about what NULL should be. */
#undef NULL
#define NULL 0
#undef __null
#define __null 0
#endif /* __cplusplus */

#endif /* !_ASYNC_AMISC_H_ */
