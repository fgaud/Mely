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

#ifndef __M_IMPL__
#define __M_IMPL__

#include "mely.h"

// Browse directory
#include <errno.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

#include <sys/stat.h>

#include <map>
#include <string>

#define USE_OVER_ALLOCATOR                      0

#define WITH_PARSE_REQUEST_HANDLER              1

#define CLOSE_AFTER_REQUEST                     0

#define SYNCHRONOUS_CALL_BETWEEN_RCW            0

#define DONT_USE_EPOLL                          0

#define MAX_HEADER_SIZE                         1024
#define MAX_FILENAME_LENGTH                     100
#define MAX_REQUEST_SIZE                        1024

/** ListenQ size **/
#define LISTENQ_SIZE                            50000

/** Need to be less than 1024 because sockets are also internally used **/
#define MAX_SIMULTANEOUS_CLIENTS                64000

/** Batch Size **/
#define ACCEPT_BATCH_SIZE                       200

/** Non blocking sockets **/
#define NON_BLOCKING_SOCKET                     1

/** Send/Receive buffer size **/
#define OPT_USE_DEFAULT_SOCK_BUF_SIZE           -1

/** Use send file ? **/
#define USE_SENDFILE                            0

/** Use GZip compression **/
#if USE_GZIP && USE_SENDFILE
#error "GZIP not configured with sendfile !"
#endif


/** Simulate a CPU bottleneck ? **/
#define WITH_FAKE_CPU_STAGE                     0
#define CPU_STAGE_DURATION                      100000 // Processing duration in cycles
#define CPU_STAGE_FREQUENCY                     1000+get_current_proc()*10 // Each XX requests

/** File summer stage ? **/
#define WITH_FILESUMMER_STAGE                   0
#if WITH_FILESUMMER_STAGE
#define FILESUMMER_RESPONSE_SIZE                1
#endif

/** Fake small stages **/
#define WITH_FAKE_SMALL_STAGES                  0
#if WITH_FAKE_SMALL_STAGES
#define HOW_MANY_FAKE_STAGES                    10
#define FAKE_STAGE_DURATION                     1000 //cycles
#endif


#define WITH_FAKE_CACHE_STAGES                  0
#if WITH_FAKE_CACHE_STAGES
#define HOW_MANY_FAKE_CACHE_STAGES              8
#define FAKE_CACHE_PIPELINE_SIZE                3
#define FAKE_CACHE_STAGE_DURATION               1000 //cycles
#endif

#define UNFREQUENT_FILE_USE_FAKE_CPU_STAGE      0

#define UNFREQUENT_FILE_USE_FILESUMMER          0
#if UNFREQUENT_FILE_USE_FILESUMMER
#define FILESUMMER_RESPONSE_SIZE                1
#endif //UNFREQUENT_FILE_USE_FILESUMMER

#define ONLY_UNFREQUENT_FILE_USE_GZIP           0
#if ONLY_UNFREQUENT_FILE_USE_GZIP && !USE_GZIP
#error 'GZIP not enabled'
#endif

#define USE_EVENT_DRIVEN_GZIP                   0
#if USE_EVENT_DRIVEN_GZIP && !USE_GZIP
#error  "USE_EVENT_DRIVEN_GZIP without GZip ? (You fouged yourself...)"
#endif

#if USE_SENDFILE
#include <sys/sendfile.h>
#endif

//Switch between one global Accept or one per interface.
#define ACCEPT_PER_INTERFACE                    0

#define ACCEPT_PER_CORE                         0

#define REUSE_MESSAGES                          0

/****************** Colorig options ************************/

/** Set Accept/Close processor affinity **/
#define PIN_ACCEPT                              0
#define ACCEPT_CLOSE_COLOR_PINNED               -2
#define ACCEPT_CLOSE_COLOR_NOT_PINNED           1


/** Different coloring method **/
#define PER_FLOW_COLORS                         1
#define PER_HANDLER_TYPE_COLOR                  0
#define PER_CACHE_PART_COLOR                    0
#define PER_FREQUENT_FILE_DISTRIB_COLOR         0

#if PER_FLOW_COLORS
#define PER_FLOW_BUT_STEAL_ONLY_BIG_HANDLERS    0
#endif

#if PER_HANDLER_TYPE_COLOR
#define HOW_MANY_NETWORK_CORES                  nthreads/2
//#define HOW_MANY_NETWORK_CORES                  2
#endif //PER_HANDLER_TYPE_COLOR

#if PER_CACHE_PART_COLOR
#define CORE_FOLDER_STR_LEN 7
#endif //PER_CACHE_PART_COLOR

#if PER_FREQUENT_FILE_DISTRIB_COLOR
#error "PER_FREQUENT_FILE_DISTRIB_COLOR NOT SUPPORTED"
#define FREQ_FILE_BASE_CORE                     2
#define HOW_MANY_CORE_TO_DEDICATE_TO_FREQ_FILES 2

#if HOW_MANY_CORE_TO_DEDICATE_TO_FREQ_FILES > 2
#error 'TODO. Does not work currently'
#endif //HOW_MANY_CORE_TO_DEDICATE
#endif //PER_FREQUENT_FILE_DISTRIB_COLOR


#define READ_REQ_DURATION        43000 // 21000
#define PARSE_REQUEST_DURATION   22000 // 2000
#define CIC_DURATION             20000 // 2500
#define WRITE_DURATION           17500 // 17500
#define FREE_REQ_DURATION        400
#define CLOSE_DURATION           14000
#define ACCEPT_DURATION          165000


/*******************************************************/
enum parse_request_steps{
   PARSE_METHOD,
   PARSE_FILE,
   PARSE_HTTP_VERSION,
   PARSE_END
};

enum handlers {
   h_Accept,
   h_ReadRequest,

#if WITH_PARSE_REQUEST_HANDLER
   h_ParseRequest,
#endif

#if !USE_SENDFILE
   h_CheckInCache,
#else
   h_SendFile,
   h_WriteHeaders,
#endif

#if USE_GZIP
   h_CompressResponse,
#elif WITH_FAKE_CPU_STAGE
   h_FakeCpuStage,
#elif WITH_FILESUMMER_STAGE || UNFREQUENT_FILE_USE_FILESUMMER
   h_FileSummerStage,
#elif WITH_FAKE_SMALL_STAGES
   h_FakeSmallStage,
#elif WITH_FAKE_CACHE_STAGES
   h_FakeCacheStage,
#endif
   h_Write,
   h_FreeRequest,
   h_Close,
   h_BadRequest,
   h_FourOhFor,
   FIN_ENUM,
};

typedef struct _message_t{
   int socket;
   int length;

   char* response;
   int response_size;

   char request[MAX_REQUEST_SIZE];
   char *file_requested;

   bool in_cache;

   bool close_after_parsing;
   char* req_end;

   int accept_color;
   int read_color;

   struct _message_t *next_message;  // For HTTP pipelining
#if USE_SENDFILE
   int file_size;
   int read_fd;
   off_t offset;
#endif

#if REUSE_MESSAGES
   struct _message_t* next_free_msg;
#endif

#if FAKE_STAGE_DURATION
   int nb_fs_calls;
#endif

#if FAKE_CACHE_STAGE_DURATION
   int nb_fcs_calls;
   int previous_color;
#endif

#if PROFILE_REQUEST_PROCESSING_DURATION
   uint64_t request_processing_start_time;
#endif

#if DEBUG_RUID
   int slg_client_num;
   int slg_client_req_id;
   int slg_client_hostname;
#endif

}message_t;

typedef void(*handler_t)(message_t*);

extern unsigned long start_time;

#if DONT_USE_EPOLL
void Accept (int fd, int c);
#else
void Accept (int fd);
#endif

void ReadRequest (message_t *msg);
void ParseRequest(message_t *msg);

#if USE_SENDFILE
void SendFile(message_t *msg);
void WriteHeaders(message_t *msg);
#else
void ReadFile (message_t *msg);
void CheckInCache(message_t *msg);
void Write (message_t *msg);
#endif

#if USE_GZIP && WITH_FAKE_SMALL_STAGES
#error "GZIP & Fake Small Stage ar incompatible"
#endif

#if USE_GZIP
void CompressResponse(message_t* msg);
#elif WITH_FAKE_CPU_STAGE
void FakeCpuStage(message_t *msg);
#elif WITH_FILESUMMER_STAGE || UNFREQUENT_FILE_USE_FILESUMMER
void FileSummerStage(message_t* msg);
#elif WITH_FAKE_SMALL_STAGES
void FakeSmallStage(message_t* msg);
#elif WITH_FAKE_CACHE_STAGES
void FakeCacheStage(message_t* msg);
#endif

void FreeRequest(message_t *msg);
void Close (message_t *msg);
void WakeUpAccept (int increments);
void BadRequest(message_t *msg, int err);
void FourOhFor(message_t *msg, int err);
void Dec_Accepted_Clients();
void register_accept(int color, int fd);

extern uint64_t total_time_cycle;

/** Check and internal define **/

/** Wether or not file handler build header **/
#if (USE_GZIP && !ONLY_UNFREQUENT_FILE_USE_GZIP) || WITH_FILESUMMER_STAGE
#define FILE_HANDLER_BUILD_HEADER               0
#elif DEBUG_RUID
#define FILE_HANDLER_BUILD_HEADER               0
#else
#define FILE_HANDLER_BUILD_HEADER               1
#endif

#if PER_FLOW_COLORS + PER_HANDLER_TYPE_COLOR + PER_CACHE_PART_COLOR + PER_FREQUENT_FILE_DISTRIB_COLOR != 1
#error 'Must only activate one option'
#endif

#if USE_SENDFILE && WITH_FILESUMMER_STAGE
#error 'Sendfile is not currently activable with FILESUMMER'
#endif

#if WITH_FAKE_CPU_STAGE + WITH_FAKE_SMALL_STAGES + WITH_FILESUMMER_STAGE + \
   WITH_FAKE_CACHE_STAGES + UNFREQUENT_FILE_USE_FILESUMMER + ONLY_UNFREQUENT_FILE_USE_GZIP  > 1
#error 'Incompatible options'
#endif

#if UNFREQUENT_FILE_USE_FAKE_CPU_STAGE && !WITH_FAKE_CPU_STAGE
#error "Incompatible opts"
#endif
#endif //__M_IMPL__
