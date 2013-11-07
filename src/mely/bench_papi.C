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

#include "runtime_config.h"
#ifdef HARDWARE_COUNTERS
#include "async.h"
#include "bench_papi.h"

void init_hardware_counters() {

#ifdef DEBUG_HWC
	printf("init_hardware_counters begin\n");
#endif

	int status;
	if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
		printf("can't PAPI_init\n");
		exit(1);
	}

	/* Enable and initialize multiplex support */
	if (PAPI_multiplex_init() != PAPI_OK){
		printf("(%s,%d) Error, PAPI_multiplex_init\n",__FUNCTION__,__LINE__);
		exit(1);
	}


	if ( (status=PAPI_thread_init(pthread_self)) != PAPI_OK) {
		printf("(%s,%d) Error, PAPI_thread_init : %d\n",__FUNCTION__,__LINE__,status);
		exit(status);
	}

#ifdef DEBUG_HWC
	printf("init_hardware_counters end\n");
#endif

}

void start_hardware_counter(int num_thread, int * ES, int * events, int nb_events) {
	int status;

#ifdef DEBUG_HWC
	printf("start_hardware_counters t-%d begin\n", num_thread);
#endif

	/* Create an EventSet */
	if ((status = PAPI_create_eventset(ES)) != PAPI_OK) {
		printf("can't create eventset errcode(1)=%d\n", status);
		exit(status);
	}

#ifdef DEBUG_HWC
	printf("ES is %d (thread %d)\n",*ES,num_thread);
#endif

	if (PAPI_set_multiplex(*ES) != PAPI_OK){
		printf("Error\n");
	}

	// add events to the event set
	if ( (status = PAPI_add_events(*ES, events, nb_events)) != PAPI_OK) {
		printf("Thread %d : can't add events errcode=%d\n",num_thread, status);
		exit(status);
	}


	if ( (status=PAPI_register_thread()) != PAPI_OK) {
		printf("Thread %d :can't register thread ret=%d\n",num_thread, status);
		exit(status);
	}


	if ((status = PAPI_start(*ES)) != PAPI_OK) {
		perror("-->error : ");
		printf("Thread %d :can't start counters errcode=%d\n",num_thread, status);
		exit(status);
	}

#ifdef DEBUG_HWC
	printf("start_hardware_counters t-%d end\n", num_thread);
#endif
}


int stop_hardware_counters(int num_thread, int * ES, int nb_events, long long * results) {
	int status;

#ifdef DEBUG_HWC
	printf("STOP. ES is %d (thread %d)\n",*ES,num_thread);
#endif

	bzero(results,(sizeof(long long)*nb_events));

	if ((status=PAPI_stop(*ES, results)) != PAPI_OK) {
		printf("can't stop counters errcode=%d (thread %d)\n", status,num_thread);
		exit(status);
	}

	if ((status=PAPI_cleanup_eventset(*ES)) != PAPI_OK) {
		printf("PAPI_cleanup_eventset errcode=%d\n", status);
		exit(status);
	}

	if ((status=PAPI_destroy_eventset(ES)) != PAPI_OK) {
		printf("PAPI_destroy_eventset errcode=%d\n", status);
		exit(status);
	}

	return status;
}

int read_hardware_counters(int num_thread, int * ES, int nb_events, long long * results) {
	int status;
	bzero(results,(sizeof(long long)*nb_events));

#ifdef DEBUG_HWC
	printf("STOP. ES is %d (thread %d)\n",*ES,num_thread);
#endif

	if ((status=PAPI_accum(*ES, results)) != PAPI_OK) {
		printf("can't stop counters errcode=%d (thread %d)\n", status,num_thread);
		exit(status);
	}

	return status;
}

#endif //HARDWARE_COUNTERS

