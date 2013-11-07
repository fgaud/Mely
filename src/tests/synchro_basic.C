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

#define SYNCHRO_COLOR 10

void event_end(int param) {
   DEBUG_TMP("WE ARE SURE event_1 and event_2 are done. param %d\n", param);
   exit(0);
}

//This callback must be called with the same color
int nb_events_coming = 0;
void event_synchro(int event_num) {
   nb_events_coming++;
   DEBUG_TMP("has event_num %d\n", event_num);
   if(nb_events_coming==2){
      //We can do whatever we want here, previous events are synchronized.
      cpucb_tail(cwrap(event_end, 789, 0));
   }
}

void event_1(int param) {
   DEBUG_TMP("param %d\n", param);
   cpucb_tail(cwrap(event_synchro, 1, SYNCHRO_COLOR));
}

void event_2(int param) {
   DEBUG_TMP("param %d\n", param);
   cpucb_tail(cwrap(event_synchro, 2, SYNCHRO_COLOR));
}


/*
 * Posts 2 events.
 * The 2 events are synchronized with event_synchro.
 * When the sync is done, event_synchro registers the event_end
 *
 *
 *         /--> event_1 --\
 *   main--                --> event_synchro x2 -->event_end
 *         \--> event_2 --/
 *
 */
int main(int argc, char *argv[]) {

   //posts event_1 with color 0
   cpucb_tail(cwrap(event_1, 123, 0));

   //posts event_2 with color 1
   cpucb_tail(cwrap(event_2, 456, 1));

   amain();
}
