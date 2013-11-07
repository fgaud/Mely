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
#include "mely.h"

#define HOW_MANY        5
#define DELAY           3

static cbv cb;
static int nb_iter = 0;

void docallback(int color) {
   DEBUG_TMP("Processed event with color %d\n",color);
   nb_iter++;

   if(nb_iter < HOW_MANY){
      DEBUG_TMP("Registering a new delay cb = %p\n",cb);
      delaycb(DELAY, 0, cb);
   } else {
      cb->setFlagAutoFree(true);
      for(int i = 0; i< task_get_nthreads(); i++){
         cpucb_tail( cpwrap(stop_hw_counters, 0-(i+1), 65535) );
      }
      save_bench_time(HOW_MANY * DELAY);
   }
}

int main(int argc, char *argv[]) {
   //async_init();

   cb = cwrap(docallback, 1, 1);
   cb->setFlagAutoFree(false);
   DEBUG_TMP("New callback auto free flag = %d (cb = %p)\n",cb->getFlagAutoFree(), cb);
   delaycb(DELAY, 0, cb);

   amain();
}
