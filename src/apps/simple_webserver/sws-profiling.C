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

#include "sws.h"
#include "sws-includes.h"
#include "sws-profiling.h"

#if PROFILE_TIME_EVOLUTIONS
#define NB_ELTS_STATS_ACCEPT 50000
uint64_t nb_done_accept[MAX_THREADS];
uint64_t accept_time[MAX_THREADS][NB_ELTS_STATS_ACCEPT];
uint16_t accept_nb[MAX_THREADS][NB_ELTS_STATS_ACCEPT];

#define NB_ELTS_STATS_REGISTER_TASK 500000
uint64_t nb_done_register[MAX_THREADS];
uint64_t register_time[MAX_THREADS][NB_ELTS_STATS_REGISTER_TASK];
uint8_t register_to_core[MAX_THREADS][NB_ELTS_STATS_REGISTER_TASK];

void insert_stat_accept(int value) {
   int current_core = get_current_proc();
   uint64_t nb_done = nb_done_accept[current_core];
   if(nb_done < NB_ELTS_STATS_ACCEPT) {
      rdtscll(accept_time[current_core][nb_done]);
      accept_nb[current_core][nb_done] = value;
      nb_done_accept[current_core]++;
   }
}
void insert_stat_register_task(int value) {
   int current_core = get_current_proc();
   uint64_t nb_done = nb_done_register[current_core];
   if(nb_done < NB_ELTS_STATS_REGISTER_TASK) {
      rdtscll(register_time[current_core][nb_done]);
      register_to_core[current_core][nb_done] = value;
      nb_done_register[current_core]++;
   }
}

void print_time_stats() {
  for(int i = 0; i<task_get_nthreads(); i++){
      printf("***! Accept Time\n");
      for(uint j=0;j<nb_done_accept[i]; j++) {
         printf("%d\t%llu\t%d\n", i, accept_time[i][j], (int)accept_nb[i][j]);
      }
      printf("***! Register Task on other core Time\n");
      for(uint j=0;j<nb_done_register[i]; j++) {
         printf("%d\t%llu\t%d\n", i, register_time[i][j], (int)register_to_core[i][j]);
      }
  }
}
#endif

#if PROFILE_APP_HANDLERS
static PAD(handler_stats_t) h_stats[FIN_ENUM+1][MAX_THREADS];
static PAD(req_stats_t) r_stats[MAX_THREADS];
void print_requests_stats();

handler_stats_t* get_hstat(size_t which_handler, size_t core_no) {
   return &(h_stats[which_handler][core_no].val);
}

req_stats_t* get_rstat(size_t core_no) {
   return &(r_stats[core_no].val);
}

void print_handler_stats(){
   unsigned int nthreads = task_get_nthreads();
   const unsigned int str_size = 20;
   char handler_name[str_size];

   printf( "**** Webserver stats ****\n");
   for(unsigned int i = 0; i != FIN_ENUM; i++){
      switch(i){
         case h_Accept:
            strncpy(handler_name, "Accept", str_size);
            break;
         case h_ReadRequest:
            strncpy(handler_name, "ReadRequest", str_size);
            break;

#if WITH_PARSE_REQUEST_HANDLER
         case h_ParseRequest:
            strncpy(handler_name, "ParseRequest", str_size);
            break;
#endif

#if !USE_SENDFILE
         case h_CheckInCache:
            strncpy(handler_name, "CheckInCache", str_size);
            break;
#else
         case h_SendFile:
            strncpy(handler_name, "SendFile", str_size);
            break;
         case h_WriteHeaders:
            strncpy(handler_name, "WriteHeaders", str_size);
            break;
#endif

#if USE_GZIP
         case h_CompressResponse:
            strncpy(handler_name, "CompressResponse", str_size);
            break;
#elif WITH_FAKE_CPU_STAGE
         case h_FakeCpuStage:
            strncpy(handler_name, "FakeCpuStage", str_size);
            break;
#elif WITH_FAKE_SMALL_STAGES
         case h_FakeSmallStage:
            strncpy(handler_name, "FakeSmallStage", str_size);
            break;
#elif WITH_FILESUMMER_STAGE
         case h_FileSummerStage:
            strncpy(handler_name, "FileSummerStage", str_size);
            break;
#elif WITH_FAKE_CACHE_STAGES
         case h_FakeCacheStage:
            strncpy(handler_name, "FakeCacheStage", str_size);
            break;
#endif
         case h_Write:
            strncpy(handler_name, "Write", str_size);
            break;
         case h_FreeRequest:
            strncpy(handler_name, "FreeRequest", str_size);
            break;
         case h_Close:
            strncpy(handler_name, "Close", str_size);
            break;
         case h_BadRequest:
            strncpy(handler_name, "BadRequest", str_size);
            break;
         case h_FourOhFor:
            strncpy(handler_name, "FourOhFor", str_size);
            break;
         default:
            PRINT_ALERT("Unknown handler, Big error ...\n");
            exit(EXIT_FAILURE);
      }
      printf( "%s\n", handler_name);

      uint64_t global_nb_calls = 0;
      uint64_t global_total_length = 0;
      uint64_t global_tbtc = 0;

      uint64_t global_accept_nb_successes = 0;
      long double global_handler_rate = 0;

      uint64_t global_write_length = 0;
      uint64_t global_write_real_duration = 0;

      for(unsigned int j = 0; j <= nthreads; j++){
         uint64_t nb_calls = j == nthreads ? global_nb_calls : h_stats[i][j].val.nb_calls;
         uint64_t total_duration = 0;
         long double avg_length = 0;
         long double tbtc = 0;
         long double handler_rate = 0;

         long double accept_avg_success = 0;

         long double write_length = 0;
         long double write_real_duration = 0;

         if(nb_calls){
            if(j == nthreads){
               total_duration = global_total_length;
               avg_length = (long double) global_total_length / (long double) global_nb_calls;
               tbtc = (long double) global_tbtc / (long double) (global_nb_calls - nthreads);

               handler_rate = global_handler_rate;

               if(i == h_Accept){
                  accept_avg_success = (long double) global_accept_nb_successes/ (long double) global_nb_calls;
               }

               else if(i == h_Write){
                  write_length = (long double) global_write_length/ (long double) global_nb_calls;
                  write_real_duration = (long double) global_write_real_duration/ (long double) global_nb_calls;
               }
               printf("* Global:\n");

            }
            else{
               total_duration = h_stats[i][j].val.total_duration;

               avg_length =  (long double) h_stats[i][j].val.total_duration / (long double) nb_calls;
               tbtc = (long double) h_stats[i][j].val.time_between_two_calls / (long double) (nb_calls-1);
               handler_rate = (long double) HANDLER_RATE_BASE / tbtc;

               global_nb_calls += nb_calls;
               global_total_length += h_stats[i][j].val.total_duration;
               global_tbtc += h_stats[i][j].val.time_between_two_calls;

               global_handler_rate += handler_rate;

               if(i == h_Accept){
                  accept_avg_success = (long double) h_stats[i][j].val.accept_nb_successes/ (long double) nb_calls;
                  global_accept_nb_successes += h_stats[i][j].val.accept_nb_successes;
               }
               else if(i == h_Write){
                  write_length = (long double) h_stats[i][j].val.write_length/ (long double) nb_calls;
                  global_write_length += h_stats[i][j].val.write_length;

                  write_real_duration = (long double) h_stats[i][j].val.write_real_duration/ (long double) nb_calls;
                  global_write_real_duration += h_stats[i][j].val.write_real_duration;
               }

               printf("* Thread %u:\n", j);
            }

            printf("\t* Nb calls: %llu\n", (long long unsigned) nb_calls);
            printf("\t* Total handler duration: %llu (%.2Lf%% of total time)\n",
                     (long long unsigned) total_duration,
                     (long double) total_duration * 100. / (long double) total_time_cycle);

            printf("\t* Avg handler duration: %.2Lf\n", avg_length);
            printf("\t* Time between two call: %.2Lf\n", tbtc);
            printf("\t* Handler call rate: %.2Lf (nb calls in %llu cycles)\n", handler_rate, (long long unsigned) HANDLER_RATE_BASE);

            if(i == h_Accept){
               printf("\t* Nb success per accept: %.2Lf (%.2Lf %%)\n",
                        accept_avg_success,
                        (accept_avg_success * 100.) / ACCEPT_BATCH_SIZE);
            }

            if(i == h_Write){
               printf("\t* Real write duration: %.2Lf\n",
                        write_real_duration);

               printf("\t* Avg write return size: %.2Lf\n",
                        write_length);
            }
         }
      }
   }

   print_requests_stats();

}

void print_requests_stats(){
   printf("\n --- Requests stats ---\n");
   for(int i = 0; i<task_get_nthreads(); i++){
      printf("* Core %d\n", i);
      printf("\tNb requests processed : %llu\n", (long long unsigned) r_stats[i].val.nb_request_processed);
      if (r_stats[i].val.nb_request_processed) {
         printf("\tRequest avg processing real time: %llu cycles\n",
                  (long long unsigned) r_stats[i].val.total_request_real_time / r_stats[i].val.nb_request_processed);
         printf("\tRequest avg processing useful time: %llu cycles\n",
                  (long long unsigned) r_stats[i].val.total_request_useful_time / r_stats[i].val.nb_request_processed);
      }
   }
}
#endif


void profile_init() {
#if PROFILE_APP_HANDLERS
   memset(h_stats, 0, MAX_THREADS * (FIN_ENUM+1) * sizeof(handler_stats_t));
   size_t tp_size = CACHE_LINE_SIZE + ((sizeof(handler_stats_t) / CACHE_LINE_SIZE) * CACHE_LINE_SIZE);
   memset(h_stats, 0, tp_size * (FIN_ENUM+1) * MAX_THREADS);
   register_atexit_handler(print_handler_stats);
#endif
#if PROFILE_TIME_EVOLUTIONS
   register_atexit_handler(print_time_stats);
#endif

   register_EH_name((void*)Accept,              "[FDCB] Accept");
   register_EH_name((void*)ReadRequest,         "[FDCB] ReadRequest");

#if WITH_PARSE_REQUEST_HANDLER
   register_EH_name((void*)ParseRequest,         "ParseRequest");
#endif

#if USE_GZIP
   register_EH_name((void*)CompressResponse,    "CompressResponse");
#elif WITH_FAKE_CPU_STAGE
   register_EH_name((void*)FakeCpuStage,        "FakeCpuStage");
#elif WITH_FILESUMMER_STAGE
   register_EH_name((void*)FileSummerStage,     "FileSummerStage");
#elif WITH_FAKE_SMALL_STAGES
   register_EH_name((void*)FakeSmallStage,      "FakeSmallStage");
#elif WITH_FAKE_CACHE_STAGES
   register_EH_name((void*)FakeCacheStage,      "FakeCacheStage");
#endif

#if USE_SENDFILE
   register_EH_name((void*)WriteHeaders, "WriteHeaders");
   register_EH_name((void*)SendFile, "SendFile");
#else
   register_EH_name((void*)CheckInCache,        "CheckInCache");
   register_EH_name((void*)Write,               "Write");
#endif
   register_EH_name((void*)Dec_Accepted_Clients,"Dec_Accepted_Clients");
   register_EH_name((void*)FreeRequest,         "FreeRequest");
   register_EH_name((void*)Close,               "Close");
   register_EH_name((void*)FourOhFor,           "FourOhFor");
   register_EH_name((void*)BadRequest,          "BadRequest");
}
