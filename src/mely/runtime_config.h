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

#ifndef RUNTIME_CONFIG_H
#define RUNTIME_CONFIG_H

/** How many tasks from the same color do we have to execute ? **/
#define NB_TASKS_FROM_SAME_COLOR_THRES                  1

#define REMOVE_EPOLL_TIMEOUT                            0

#define STEALING                                        0
#define RESET_COLOR_ON_EMPTY_QUEUE                      0

#define SET_MAXIMUM_PRIORITY                            1
#define PADDING_SIZE                                    CACHE_LINE_SIZE
#define MAX_FREETASKS                                   300

//#define HARDWARE_COUNTERS                               1

#endif //RUNTIME_CONFIG_H
