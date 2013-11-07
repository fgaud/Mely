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

#ifndef __BENCH_PAPI__
#define __BENCH_PAPI__

#ifdef HARDWARE_COUNTERS 

#include <papi.h>

void init_hardware_counters();
void start_hardware_counter(int num_thread, int * ES, int * events, int nb_events);
int stop_hardware_counters(int num_thread, int * ES, int nb_events, long long * results);
int read_hardware_counters(int num_thread, int * ES, int nb_events, long long * results);

#endif //HARDWARE_COUNTERS 

#endif // __BENCH_PAPI__

