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
#ifndef RUNTIME_STATS_H_
#define RUNTIME_STATS_H_

#include "mely.h"
#include "pad.h"
#include "runtime_config.h"

#include <map>

struct _eh_stats_t{
#if TRACE_MAPPING_WS
   uint64_t nb_steals;
#endif
   uint64_t nb_calls;
   uint64_t length;
};

struct _thread_stats{
   uint64_t tasks_done;
   uint64_t cb_exec_time;

#ifdef TRACE_CONTENTION
   uint64_t time_in_contention;
   uint64_t lock_nb_calls;
#endif

#ifdef TRACE_CHOOSE_TASK
   uint64_t choose_task_cost;
   uint64_t choose_task_nb_calls;

#define LOG_CHOOSE_TASK(...) \
   do { \
      unsigned long long global_start; \
	   unsigned long long global_stop;  \
	   rdtscll(global_start);  \
      __VA_ARGS__ \
      rdtscll(global_stop);   \
     THREAD_STATS.choose_task_cost += global_stop - global_start; \
     THREAD_STATS.choose_task_nb_calls++; \
   } while(0)
#else
#define LOG_CHOOSE_TASK(...)  __VA_ARGS__
#endif//TRACE_CHOOSE_TASK_COST

#ifdef TRACE_WORKSTEALING
   unsigned long long workstealing_count;
   unsigned long long workstealing_try_steal;

   uint64_t workstealing_per_thread_count[MAX_THREADS];

   unsigned long long nb_tasks_stolen_per_steal;
   unsigned long long nb_colors_stolen_per_steal;

   unsigned long long workstealing_choose_color_cost;
   unsigned long long workstealing_choose_victim_cost;

   unsigned long long workstealing_internal_cost;
   unsigned long long workstealing_global_cost;
   uint64_t workstealing_success_cost;
   unsigned long long workstealing_lock_cost;

   unsigned long long workstealing_lose_stealing_cost;
   unsigned long long workstealing_lose_stealing_count;

   //These stats are OK when coloring is well balanced. In the case of SFS there is to much zero color to have an avg with a small stddev
   unsigned long long nb_colors_seen_when_stealing;
   unsigned long long nb_tasks_seen_when_stealing;
#endif

#ifdef PROFILING_SUPPORT
   //used to take into account the register_task in runtime cost (and not in cb_exec_time)
   unsigned long long register_task_time;
#endif

#if TRACE_REGISTER_TASK
   uint64_t register_task_nb_calls;
   uint64_t register_task_costs;
   uint64_t register_task_lock_costs;
   uint64_t register_task_find_task_costs;
   uint64_t register_task_insert_task_costs;
   uint64_t register_task_wake_up_costs;
   uint64_t register_task_cb_first_access;
#define LOG_REGISTER_TASK_WAKEUP_COST(...) \
   do { \
      uint64_t t_start, t_stop; \
      rdtscll(t_start); \
      __VA_ARGS__ \
      rdtscll(t_stop); \
     THREAD_STATS.register_task_wake_up_costs += (t_stop - t_start); \
   } while(0);
#else
#define LOG_REGISTER_TASK_WAKEUP_COST(...) __VA_ARGS__
#endif

#ifdef TRACE_THREAD_WAKEUP
   uint64_t thread_wakeup_calls;
   uint64_t thread_wakeup_costs;

   uint64_t thread_wakeup_notify_calls;
   uint64_t thread_wakeup_notify_costs;
#define LOG_WAKEUP_NOTIFY(...) \
   do { \
      uint64_t wp_start, wp_stop; \
      rdtscll(wp_start); \
      __VA_ARGS__ \
      rdtscll(wp_stop); \
      THREAD_STATS.thread_wakeup_notify_calls ++;
      THREAD_STATS.thread_wakeup_notify_costs += wp_stop - wp_start; \
   } while(0);
#define LOG_WAKEUP(...) \
   do { \
      uint64_t w_start, w_stop; \
      rdtscll(w_start); \
      __VA_ARGS__ \
      rdtscll(w_stop); \
      THREAD_STATS.thread_wakeup_calls ++;
      THREAD_STATS.thread_wakeup_costs += w_stop - w_start; \
   } while(0);
#else
#define LOG_WAKEUP_NOTIFY(...) __VA_ARGS__
#define LOG_WAKEUP(...) __VA_ARGS__
#endif

#ifdef PROFILE_CORE_LOCK
   uint64_t core_epoll_lock_cost;
#endif

#ifdef PROFILE_CORE_EPOLL
   uint64_t core_epoll_calls;
   uint64_t core_epoll_callback_cost;
   uint64_t core_epoll_wait_cost;
   uint64_t core_epoll_avg_ret;

   uint64_t core_epoll_reg_cost;
   uint64_t core_epoll_already_reg_cost;
   uint64_t core_epoll_reg_epoll_remove_cost;
   uint64_t core_epoll_internal_add_cost;
   uint64_t core_epoll_internal_remove_cost;
   uint64_t core_epoll_cleanpipe_cost;
   uint64_t core_epoll_time_between_two_calls;
   uint64_t core_epoll_last_call_time;
#define LOG_EPOLL_REMOVE(...) \
   do { \
      uint64_t epoll_internal_remove_start, epoll_internal_remove_stop; \
      rdtscll(epoll_internal_remove_start); \
      __VA_ARGS__ \
      rdtscll(epoll_internal_remove_stop); \
      CORE_STATS.core_epoll_internal_remove_cost += (epoll_internal_remove_stop - epoll_internal_remove_start); \
   } while(0);
#define LOG_EPOLL_ADD(...) \
   do { \
      uint64_t epoll_internal_add_start, epoll_internal_add_stop; \
      rdtscll(epoll_internal_add_start); \
      __VA_ARGS__ \
      rdtscll(epoll_internal_add_stop); \
      CORE_STATS.core_epoll_internal_add_cost += (epoll_internal_add_stop - epoll_internal_add_start); \
   } while(0);
#define LOG_REMOVE_COST(...) \
   do { \
	   uint64_t epoll_reg_remove_start, epoll_reg_remove_stop; \
	   rdtscll(epoll_reg_remove_start); \
	   __VA_ARGS__ \
	   rdtscll(epoll_reg_remove_stop); \
	   CORE_STATS.core_epoll_reg_epoll_remove_cost += (epoll_reg_remove_stop - epoll_reg_remove_start); \
   } while(0);
#define LOG_START_FD_POLL_CHECK(...) \
   do { \
	   uint64_t epoll_reg_start, epoll_reg_stop; \
      rdtscll(epoll_reg_start);                 \
      __VA_ARGS__ \
      rdtscll(epoll_reg_stop);                    \
     CORE_STATS.core_epoll_reg_cost += (epoll_reg_stop - epoll_reg_start); \
   } while(0);

#define LOG_EPOLL_WAIT_TIME(...) \
   do { \
	   uint64_t epoll_wait_start, epoll_wait_stop;  \
      rdtscll(epoll_wait_start);                   \
      __VA_ARGS__ \
      rdtscll(epoll_wait_stop);                    \
      CORE_STATS.core_epoll_wait_cost += (epoll_wait_stop - epoll_wait_start); \
      CORE_STATS.core_epoll_avg_ret += n;          \
   } while(0);
#define LOG_PIPE_CLEANING(...) \
   do { \
      uint64_t epoll_cleanpipe_start, epoll_cleanpipe_stop; \
      rdtscll(epoll_cleanpipe_start); \
      __VA_ARGS__ \
      rdtscll(epoll_cleanpipe_stop); \
     CORE_STATS.core_epoll_cleanpipe_cost += (epoll_cleanpipe_stop - epoll_cleanpipe_start); \
   } while(0);
#define LOG_FDWATCHER_CHECK(...) \
   do { \
        uint64_t epoll_cb_start, epoll_cb_stop = 0;\
        rdtscll(epoll_cb_start);\
        if(CORE_STATS.core_epoll_last_call_time > 0){\
           CORE_STATS.core_epoll_time_between_two_calls += epoll_cb_start - CORE_STATS.core_epoll_last_call_time;\
        }\
        CORE_STATS.core_epoll_last_call_time = epoll_cb_start;\
      __VA_ARGS__ \
      rdtscll(epoll_cb_stop); \
     CORE_STATS.core_epoll_callback_cost += (epoll_cb_stop - epoll_cb_start); \
     CORE_STATS.core_epoll_calls++; \
   } while(0);
#else
#define LOG_EPOLL_REMOVE(...) __VA_ARGS__
#define LOG_EPOLL_ADD(...) __VA_ARGS__
#define LOG_REMOVE_COST(...) __VA_ARGS__
#define LOG_START_FD_POLL_CHECK(...) __VA_ARGS__
#define LOG_EPOLL_WAIT_TIME(...) __VA_ARGS__
#define LOG_PIPE_CLEANING(...) __VA_ARGS__
#define LOG_FDWATCHER_CHECK(...) __VA_ARGS__
#endif

   /** Warning: when adding new map, you must update _reset_stats **/
#ifdef TRACE_MAPPING
   /** Mapping EH <--> Nb called **/
   std::map<void* , _eh_stats_t*>* eh_stats;
#define LOG_MAPPING(...) \
   do {                    \
	   uint64_t tts, tte;   \
	   rdtscll(tts);        \
	   void * ___cb_faddr = cb->getfaddr();   \
	   __VA_ARGS__ \
	   rdtscll(tte);  \
	   if((*(THREAD_STATS.eh_stats))[___cb_faddr] == NULL){  \
         (*(THREAD_STATS.eh_stats))[___cb_faddr] = (_eh_stats_t*)calloc(sizeof(_eh_stats_t),1); \
         (*(THREAD_STATS.eh_stats))[___cb_faddr]->nb_calls = 0;   \
      }  \
      (*(THREAD_STATS.eh_stats))[___cb_faddr]->nb_calls++;  \
      (*(THREAD_STATS.eh_stats))[___cb_faddr]->length += tte - tts; \
   } while(0);
#else
#define LOG_MAPPING(...) __VA_ARGS__
#endif
};


typedef PAD_TYPE(struct _thread_stats, PADDING_SIZE) thread_stats_t;
extern thread_stats_t thread_stats[MAX_THREADS+1];
extern __thread unsigned int _thread_no;
#define THREAD_STATS                    (thread_stats[_thread_no].val)
#define STATS(i)                        (thread_stats[i].val)

#endif /* RUNTIME_STATS_H_ */
