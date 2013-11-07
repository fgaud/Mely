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
#ifndef RTM_TASK_H
#define RTM_TASK_H

#include "mely.h"
#include "core.h"
#include "ws_config.lbc.h"
#include "pad.h"
#include "runtime_stats.h"
#include <map>
#include <vector>

/**
 * Tasks are represented by a linked lists
 * Each task is associated with a callback, a color and a priority
 * A special task could be dummy. Probably because we don't want to have empty lists
 */
struct Task {
   CBV_PTR_TYPE cb;
   Task *next;
   Task *prev;
   bool dummy;
   int color;
   int prio;

   Task (CBV_PTR_TYPE c) : cb (c), next (0), prev (0), dummy (false),
   color (c->getcolor ()), prio (c->getprio ()) {
   }

   Task (int task_color) : cb (NULL), next (NULL), prev (NULL), dummy (true),
            color (task_color), prio (0) {
   }

   void clear () {
      cb = NULL;
   }
   void setcb (CBV_PTR_TYPE c)
   { cb = c; color = c->getcolor (); prio = c->getprio (); }
};

struct Task_List {
   Task *first_task;
   Task *last_head_task;
   Task *last_task;

   // Used when chaining task_list to reduce allocation costs
   Task_List * next;
   Task_List * prev;

   int is_stealable;
   Task_List * steal_next;
   Task_List * steal_prev;

   int nb_callbacks;

   int color;
   int current_prio;
   int default_prio;

   int dummy;
#if TIME_LEFT_WORKSTEALING
   uint64_t total_processing_duration;
#endif
};

/** This structure represents a thread **/
struct _thread_private {
   volatile int task_count;

   struct Task *free_task;
   int freetask_count;

   /** List of color in the thread's queue **/
   Task_List * color_list;
   Task_List * last_of_color_list;

   int steal_color_list_count;
   Task_List * steal_color_list;
   #if SORT_STEAL_LIST
   Task_List * steal_last_of_color_list_low;
   Task_List * steal_last_of_color_list_medium;
   Task_List * steal_last_of_color_list_high;
   #else
   Task_List * steal_last_of_color_list;
   #endif

   /** How many tasks from the same color have we already executed ? **/
   int nb_tasks_from_color_executed;

   /** Have we been stolen ? **/
   int has_been_stolen;
   int was_stealable;

   /** Number of colors in the thread's task lists **/
   int num_unique_colors;

   char sleeping;
};

typedef PAD_TYPE(struct _thread_private, PADDING_SIZE) thread_private_t;

/** Init and check functions **/
void init_task_state();
void task_set_nthreads (int nthreads);
int task_get_nthreads ();
void task_go ();

/** Runtime function **/
int register_task (CBV_PTR_TYPE cb);
int register_task_head (CBV_PTR_TYPE cb);

/** Manipulating colors **/
void inc_color_count(int which_thread, int color);
void dec_color_count(int which_thread, int color);
int count_unique_colors(int which_thread);
int count_color(int thread, int color);

/** Debugging/Profiling purposes **/
void task_print_queue ();
void dump_runtime_parameters(int maxfd);
void print_queue_lengths();
void task_set_cpu_map (int nthreads);

static inline void free_callback(CBV_PTR_TYPE cb) {
   if(cb->getFlagAutoFree())
   {
      delete cb;
   }
}

#endif /* RTM_TASK_H */
