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
#ifndef WS_CONFIG_H
#define WS_CONFIG_H

#include "runtime_config.h"

#if STEALING

#define WS_FUNCTION                             steal_work

/**
 * Optim 1: time left workstealing: steal long tasks first.
 * Also used for priority workstealing: tasks we don't want to steal have a time==0.
 */
#define TIME_LEFT_WORKSTEALING                  0
#define MUST_STEAL_THRESHOLD                    1000

#define SORT_STEAL_LIST                         0     /* TIME_LEFT_WORKSTEALING MUST BE 1 */
#define MEDIUM_THRES                            15000 /* A colors which represents more than 15000 is appealing...*/
#define HIGH_THRES                              30000 /* and more than 30000 is super appealing ! */

/**
 * Optim 2: cache efficient workstealing: steal neighbors first.
 */
#define CE_WS                                   0

/**
 * Optim 3: batch steal (~50% of available tasks)
 */
#define USE_BATCH_WS                            1
#define BATCH_TASK_WS                           0.5

#endif

#if SORT_STEAL_LIST && !TIME_LEFT_WORKSTEALING
#error "You must use TIME_LEFT_WORKSTEALING to activate SORT_STEAL_LIST"
#endif
#endif //WS_CONFIG_H
