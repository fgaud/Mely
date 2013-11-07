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


#ifndef _SWS_PROFILING_H
#define	_SWS_PROFILING_H
#ifdef PROFILE_APP_HANDLERS

void print_handler_stats();

#define HANDLER_RATE_BASE                       2330000000UL

/** Profiling **/
typedef struct{
   uint64_t nb_calls;
   uint64_t total_duration;
   uint64_t time_between_two_calls;

   union{
      uint64_t accept_nb_successes;
      struct{
         uint64_t write_length;
         uint64_t write_real_duration;
      };
   };
}handler_stats_t;

typedef struct{
   uint64_t total_request_real_time;
   uint64_t total_request_useful_time;
   uint64_t nb_request_processed;
}req_stats_t;

handler_stats_t* get_hstat(size_t which_handler, size_t core_no);
req_stats_t* get_rstat(size_t core_no);


#define START_HANDLER_PROFILE(fun) \
   static uint64_t h_last_call_time = 0; \
   uint64_t h_start_time, h_stop_time; \
   DEBUG("Handler num %d start\n", h_ ##fun);\
   get_hstat(h_ ##fun,get_current_proc())->nb_calls++; \
   rdtscll(h_start_time); \
   if(h_last_call_time > 0){ \
      get_hstat(h_ ##fun,get_current_proc())->time_between_two_calls += h_start_time - h_last_call_time; \
   } \
   h_last_call_time = h_start_time


#define STOP_HANDLER_PROFILE(fun) \
   DEBUG("Handler num %d stop\n", h_ ##fun); \
   rdtscll(h_stop_time); \
   get_hstat(h_ ##fun,get_current_proc())->total_duration += (h_stop_time - h_start_time);

#define STOP_PROCESSING_HANDLER_PROFILE(fun) \
   DEBUG("Handler num %d stop\n", h_ ##fun); \
   rdtscll(h_stop_time); \
   get_hstat(h_ ##fun,get_current_proc())->total_duration += (h_stop_time - h_start_time); \
   get_rstat(get_current_proc())->total_request_useful_time += (h_stop_time - h_start_time)

#else
#define START_HANDLER_PROFILE(fun)
#define STOP_HANDLER_PROFILE(fun)
#define STOP_PROCESSING_HANDLER_PROFILE(fun)
#endif

#if PROFILE_TIME_EVOLUTIONS
void insert_stat_accept(int value);
void insert_stat_register_task(int value);
#endif


void profile_init();
#endif	/* _SWS_PROFILING_H */

