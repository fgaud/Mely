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

#include "sws-includes.h"
#include "sws.h"
#include "sws-misc.h"
#include "sws-profiling.h"
#include "sws-cache.h"
#include "sws-accept.h"

#if USE_GZIP
#include "zlib/zlib.h"
#endif

#define _exit(n) fflush(NULL); exit(n);

/** Extern var **/
unsigned long start_time;
uint64_t total_time_cycle;
uint64_t start_time_cycle = 0;

/** Can we close the fd ? Yes if no pending treatment is scheduled ! */
static int* nb_pending_treatments_fd;
#if ACCEPT_PER_CORE || ACCEPT_PER_INTERFACE
   static int *nb_client_accepted;
#else
   static int nb_client_accepted = 0;
#endif //ACCEPT_PER_CORE

extern int nthreads;



#if WITH_FAKE_CACHE_STAGES
static __thread int next_fake_color = 0;
#endif

#if DONT_USE_EPOLL
#define fdcb(a,b,c) PANIC("use of redifned fdcb!\n");
#endif


void _register_accept(int color, int fd) {
   DEBUG("FDCB(Accept) registered on fd %d, color %d\n",fd, color);
   fdcb(fd, selread, cwrap_timeleft(Accept, fd, color, ACCEPT_DURATION));
}

void register_accept(int color, int fd) {
   printf("accepting connections on fd %d, color %d\n",fd, color);
   #if DONT_USE_EPOLL
   cpucb_tail(cwrap_timeleft(Accept, fd, color, color, ACCEPT_DURATION));
   #else //DONT_USE_EPOLL
   cpucb_tail(cwrap_timeleft(_register_accept,color,fd,color, ACCEPT_DURATION));
   #endif
}


static void _register_read(int color, int fd, message_t *msg) {
   DEBUG("FDCB(Read) registered on fd %d, color %d\n",fd, color);
   CBV_PTR_TYPE cb = cwrap_timeleft(ReadRequest, msg, color, READ_REQ_DURATION);
   fdcb(fd, selread, cb);
}

static void register_read(int color, int fd, message_t *msg) {
   cpucb_tail(cwrap_timeleft(_register_read,color,fd,msg,color, READ_REQ_DURATION));
}

int main(int argc, char **argv) {
   if (argc != 3) {
      fprintf(stderr, "Usage: %s <port-number> <root-dir>\n", argv[0]);
      _exit(EXIT_FAILURE);
   }

   printf("******** Webserver info ********\n");
   printf("Separate ParseRequest handler: %s\n", WITH_PARSE_REQUEST_HANDLER ? "yes" : "no");
   printf("Reuse continuations: %s\n", REUSE_MESSAGES ? "yes" : "no");
   printf("Accept batch : %d\n", ACCEPT_BATCH_SIZE);
   printf("Use sendfile: %d\n", USE_SENDFILE);
#if !USE_SENDFILE
   printf("FILE_HANDLER_BUILD_HEADER = %d\n", FILE_HANDLER_BUILD_HEADER);
#endif
#if ACCEPT_PER_INTERFACE
   printf("ACCEPT_PER_INTERFACE\n");
#endif
#if ACCEPT_PER_CORE
   printf("ACCEPT_PER_CORE\n");
#endif

   printf("Coloring method = ");

#if PER_FLOW_COLORS
   printf("PER_FLOW_COLORS, PER_FLOW_BUT_STEAL_ONLY_BIG_HANDLERS = %d\n", PER_FLOW_BUT_STEAL_ONLY_BIG_HANDLERS);
#endif
#if PER_HANDLER_TYPE_COLOR
   printf("PER_HANDLER_TYPE_COLOR\n");
#endif
#if PER_CACHE_PART_COLOR
   printf("PER_CACHE_PART_COLOR\n");
#elif PER_FREQUENT_FILE_DISTRIB_COLOR
   printf("PER_FREQUENT_FILE_DISTRIB_COLOR (FREQ_FILE_BASE_CORE = %d)\n", FREQ_FILE_BASE_CORE);
#endif

   printf("With filesummer: %s\n", WITH_FILESUMMER_STAGE ? "yes" : "no");
   printf("With cpu stage: %s\n", WITH_FAKE_CPU_STAGE ? "yes" : "no");
#if WITH_FAKE_CPU_STAGE
   printf("\tCpu stage duration: %d\n", CPU_STAGE_DURATION);
#if UNFREQUENT_FILE_USE_FAKE_CPU_STAGE
   printf("\tUNFREQUENT_FILE_USE_FAKE_CPU_STAGE\n");
#else
   printf("\tCpu stage frequency: %d\n", CPU_STAGE_FREQUENCY);
#endif
#endif
   printf("With fake small stage: %s\n", WITH_FAKE_SMALL_STAGES ? "yes" : "no");
#if WITH_FAKE_SMALL_STAGES
   printf("\tNb fake stage per request: %d\n", HOW_MANY_FAKE_STAGES);
   printf("\tFake stage duration: %d\n", FAKE_STAGE_DURATION);
#endif
   printf("With fake cache stage: %s\n", WITH_FAKE_CACHE_STAGES ? "yes" : "no");
#if WITH_FAKE_CACHE_STAGES
   printf("\tNb fake cache stages: %d\n", HOW_MANY_FAKE_CACHE_STAGES);
   printf("\tFake cache stage pipeline size: %d\n", FAKE_CACHE_PIPELINE_SIZE);
   printf("\tFake cache stage duration: %d\n", FAKE_CACHE_STAGE_DURATION);
#endif
   printf("Filesum unfrequent response : %s\n", UNFREQUENT_FILE_USE_FILESUMMER ? "true" : "false");
   printf("Gzip unfrequent response : %s\n", ONLY_UNFREQUENT_FILE_USE_GZIP && USE_GZIP ? "true" : "false");
   printf("Max simultaneous clients: %d\n", MAX_SIMULTANEOUS_CLIENTS);
   printf("Non blocking sockets: %d\n", NON_BLOCKING_SOCKET);
   printf("Read/Write buffer size: %d\n", OPT_USE_DEFAULT_SOCK_BUF_SIZE);
   printf("Listen Queue size: %d\n", LISTENQ_SIZE);
   printf("Accept pinned : %s\n", PIN_ACCEPT ? "true" : "false");
   printf("DocumentRoot : %s\n", argv[2]);
   printf("Build zipped responses : %s\n", USE_GZIP ? "true" : "false");
   printf("Use Over Allocator for messages : %s\n", USE_OVER_ALLOCATOR ? "true" : "false");
   printf("********************************\n\n");

   printf("Size of a continuation: %lu bytes\n", (long unsigned) sizeof(message_t));

#if PER_FREQUENT_FILE_DISTRIB_COLOR
   if(FREQ_FILE_BASE_CORE > nthreads){
      PANIC("FREQ_FILE_BASE_CORE (currently set to %d) must be < nb threads (= %d)\n",
               FREQ_FILE_BASE_CORE, nthreads);
   }
   if(HOW_MANY_CORE_TO_DEDICATE_TO_FREQ_FILES > (nthreads - 1)){
      PANIC("You must leave at least one core for unfrequent files\n");
   }
   if(HOW_MANY_CORE_TO_DEDICATE_TO_FREQ_FILES + FREQ_FILE_BASE_CORE > nthreads){
      PANIC("FREQ_FILE_BASE_CORE=% doesn't allow %d cores dedicated to frequent files\n", FREQ_FILE_BASE_CORE,HOW_MANY_CORE_TO_DEDICATE_TO_FREQ_FILES);
   }
#endif

#if PER_HANDLER_TYPE_COLOR
   if(HOW_MANY_NETWORK_CORES >= nthreads){
      PRINT_ALERT("You must specified a number or network cores (currently set to %d) < total cores (= %d)\n",
               HOW_MANY_NETWORK_CORES, nthreads);
      exit(EXIT_FAILURE);
   }
#endif //PER_HANDLER_TYPE_COLOR
   
   /* Initializations */
   misc_init();
   profile_init();

   cache_init(argv[2]);
   nb_pending_treatments_fd = (int*) calloc(100000, sizeof(*nb_pending_treatments_fd));

#if ACCEPT_PER_CORE || ACCEPT_PER_INTERFACE
   nb_client_accepted = (int*) calloc(NB_MAX_ACCEPTS, sizeof(int));
#endif //multiple accepts

#if REUSE_MESSAGES
   init_msg_freelist(nthreads);
#endif

   init_accept(atoi(argv[1]));

   print_footer();

   printf("Registering %d average duration for handler ReadRequest\n",READ_REQ_DURATION);
   printf("Registering %d average duration for handler CheckInCache\n",CIC_DURATION);
   printf("Registering %d average duration for handler ParseRequest\n",PARSE_REQUEST_DURATION);
   printf("Registering %d average duration for handler Write\n", WRITE_DURATION);
   printf("Registering %d average duration for handler FreeRequest\n", FREE_REQ_DURATION);
   printf("Registering %d average duration for handler Close\n", CLOSE_DURATION);
   printf("Registering %d average duration for handler Accept\n", ACCEPT_DURATION);
   printf("Remember that other handlers have 0 as default duration\n\n");

   start_time = get_time();
   rdtscll(start_time_cycle);

   fprintf(stderr,"Server started !\n");
   amain();
}

#if PER_FLOW_COLORS && ACCEPT_PER_INTERFACE && !ACCEPT_PER_CORE
static __thread int balance = 0;
#endif

static inline int _choose_new_flow_color(handler_t handler, message_t* msg) {
   int color;

#if PER_FLOW_COLORS
   #if ACCEPT_PER_CORE
   // Warning : with workstealing this color might get mapped on another proc
   color = get_current_proc() + (async_get_nthreads())*msg->socket;
   #elif ACCEPT_PER_INTERFACE
   int nb_proc_per_interface = async_get_nthreads()/(sizeof(interfaces)/sizeof(*interfaces));
   balance = (balance+1)%nb_proc_per_interface;
   color = get_current_proc() + (async_get_nthreads())*msg->socket + balance;
   #else
   color = msg->socket;
   #endif
#elif PER_HANDLER_TYPE_COLOR
   #error "Not done"
#elif PER_CACHE_PART_COLOR
   #error "Not done"
#elif PER_FREQUENT_FILE_DISTRIB_COLOR
   #if ACCEPT_PER_CORE
   // Warning : with workstealing this color might get mapped on another proc
   color = get_current_proc() + (async_get_nthreads())*msg->socket;
   #elif ACCEPT_PER_INTERFACE
   int nb_proc_per_interface = async_get_nthreads()/(sizeof(interfaces)/sizeof(*interfaces));
   balance = (balance+1)%nb_proc_per_interface;
   color = get_current_proc() + (async_get_nthreads())*msg->socket + balance;
   #else
   color = msg->socket;
   #endif
#else
   #error "No coloring option chosen !"
#endif
   return color;
}

/** Colorization functions for the accepted sockets **/
static inline int _choose_next_color_in_flow(handler_t handler, message_t* msg){
   int color;

   if(handler == ReadRequest) {
      PANIC("Why do you use _choose_next_color_in_flow for ReadRequest ?!");
   }

#if !PER_FLOW_COLORS
   if(handler == FreeRequest || handler == Close) {
      return msg->read_color;
   }
#endif

   /**********************************************************************
    *
    *                            /\
    *                           /  \
    *                          /    \
    *                         /  !!  \
    *                         --------
    *                      Do not fouge !
    *
    ***********************************************************************/

#if PER_FLOW_COLORS
   color = get_current_color();

#if PER_FLOW_BUT_STEAL_ONLY_BIG_HANDLERS
   if(handler == ParseRequest || handler == CheckInCache || handler == FreeRequest){
      color = -get_current_proc() -1;
   }
#endif

#elif PER_HANDLER_TYPE_COLOR
   #error "Not done"
   // We must put the write/close on the the correct cores ?
#elif PER_CACHE_PART_COLOR
   #error "Not done"
   // We must put the write/close on the the correct cores ?
#elif PER_FREQUENT_FILE_DISTRIB_COLOR
   if(
#if !USE_SENDFILE
     handler == CheckInCache
#else
     handler == WriteHeaders
#endif // USE_SENDFILE
   ){

      DEBUG("File requested: %s\n", msg->file_requested);
      //WARNING: parsing is basic and conforms only to path like /frequent_files/filename
      if(strstr(msg->file_requested,"frequent_files") != NULL){
         color = (nthreads+ FREQ_FILE_BASE_CORE) + (msg->socket % HOW_MANY_CORE_TO_DEDICATE_TO_FREQ_FILES);
         DEBUG("Frequent file. Choosing color %d\n", color);
      }
      else{
         color = msg->socket % (nthreads - HOW_MANY_CORE_TO_DEDICATE_TO_FREQ_FILES);
         if(color >= FREQ_FILE_BASE_CORE){
            color += nthreads + HOW_MANY_CORE_TO_DEDICATE_TO_FREQ_FILES;
         }
         else{
            color += nthreads;
         }

         DEBUG("Unfrequent file. Choosing color %d\n", color);
      }
   }
   #if ACCEPT_PER_INTERFACE && !ACCEPT_PER_CORE
   else if(handler == ReadRequest)
   {
      //now assumes one ITF per CORE
      color = get_current_proc();
   }
   #endif
   else
   {
      color = get_current_color();
   }
#endif //CASES

   return color;
}

#if WITH_FAKE_CPU_STAGE
static inline void cpu_burn();
#endif //WITH_FAKE_CPU_STAGE

static inline void _register_next(handler_t handler, message_t* msg, int color){
#if SYNCHRONOUS_CALL_BETWEEN_RCW
   handler(msg);
#else
   int tl = 0;
   if(handler == ParseRequest){
      tl = PARSE_REQUEST_DURATION;
   }
   else if(handler == CheckInCache){
      tl = CIC_DURATION;
   }
   else if(handler == Write){
      tl = WRITE_DURATION;
   }
   else if(handler == FreeRequest){
      tl = FREE_REQ_DURATION;
   }
   else if(handler == Close){
      tl = CLOSE_DURATION;
   }
#if WITH_FAKE_CPU_STAGE
   else if((void*)handler == (void*)cpu_burn){
      tl = CPU_STAGE_DURATION;
   }
#endif //WITH_FAKE_CPU_STAGE

   cpucb_tail(cwrap_timeleft(handler, msg, color, tl));
#endif // SYNCHRONOUS_CALL_BETWEEN_RCW
}

static inline void _register_next(handler_t handler, message_t* msg){
   int color = _choose_next_color_in_flow(handler, msg);
   _register_next(handler, msg, color);
}

/********************/


#if DONT_USE_EPOLL
void Accept(int fd, int c)
#else
void Accept(int fd)
#endif
{
   static char first_request = 1;
   if(first_request == 1){
      DEBUG("Received first_request, reseting stats\n");
      start_time = get_time();
      rdtscll(start_time_cycle);
      reset_runtime_stats();

      first_request = 0;
   }

#ifdef PROFILE_APP_HANDLERS
   START_HANDLER_PROFILE(Accept);
#endif

   socklen_t length = sizeof(struct sockaddr);
   DEBUG("Calling accept for file descriptor %d on proc %d\n",fd, get_current_proc());

   int nb_batched = 0;
   int sock;
   struct sockaddr_in server_addr;

#if PROFILE_TIME_EVOLUTIONS
   int nb_client_accepted_before = nb_client_accepted;
#endif

#if ACCEPT_PER_CORE || ACCEPT_PER_INTERFACE
   int nthreads = async_get_nthreads();
   int color = get_current_color();
   while (nb_batched < ACCEPT_BATCH_SIZE && nb_client_accepted[color]
            < MAX_SIMULTANEOUS_CLIENTS/nthreads)
#else //ACCEPT_PER_CORE
   while (nb_batched < ACCEPT_BATCH_SIZE && nb_client_accepted
            < MAX_SIMULTANEOUS_CLIENTS)
#endif //ACCEPT_PER_CORE
   {

      sock = accept4(fd, (struct sockaddr *)&server_addr, &length, SOCK_NONBLOCK);
      nb_batched++;

      DEBUG("Accept called for file descriptor %d - Resulting fd is %d\n",fd,sock);

      if (sock == -1 && errno!= EAGAIN) {
         PRINT_ALERT("Error on accept, errno is : %d (%s)\n",errno,strerror(errno))
         ;
         _exit(EXIT_FAILURE);
      }
      else if (sock > -1) {
         DEBUG("Accept a new connection, resulting fd is %d\n",sock);
#if ACCEPT_PER_CORE || ACCEPT_PER_INTERFACE
         nb_client_accepted[color] ++;
#else
         nb_client_accepted ++;
#endif //ACCEPT_PER_CORE

         int optval = 1;
         socklen_t optlen = sizeof (optval);
         if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &optval, optlen) < 0) {
            PRINT_ALERT("Error on accept when seting TCP_NODELAY, errno is : %d (%s)\n",
                     errno,strerror(errno))
                     ;
            _exit(EXIT_FAILURE);
         }

         int size= OPT_USE_DEFAULT_SOCK_BUF_SIZE;
         if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void *) &size, sizeof(size))
                  < 0) {
            PRINT_ALERT("Error on accept when seting SO_RCVBUF, errno is : %d (%s)\n",
                     errno,strerror(errno))
                     ;
            _exit(EXIT_FAILURE);
         }
         if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void *) &size, sizeof(size))
                  < 0) {
            PRINT_ALERT("Error on accept when seting SO_SNDBUF, errno is : %d (%s)\n",
                     errno,strerror(errno))
                     ;
            _exit(EXIT_FAILURE);
         }

#ifdef DEBUG_TASK
         print_socket_option(sock);
#endif

#if CLOSE_AFTER_REQUEST
         /**
          * Linger on close if data present; socked will be closed immediatly
          */

         struct linger linger;
         // Initalizing linger
         linger.l_onoff = 1;
         /*0 = off (l_linger ignored), nonzero = on */
         linger.l_linger =0;
         /*0 = discard data, nonzero = wait for data sent */

         if (setsockopt(sock, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger))
                  < 0) {
            PRINT_ALERT("Error on accept when seting SO_SNDBUF, errno is : %d (%s)\n",
                     errno,strerror(errno))
                     ;
            _exit(EXIT_FAILURE);
         }
#endif //CLOSE_AFTER_REQUEST

         int core_no = get_current_proc();
         message_t* out = get_new_msg(core_no, sock);

         out->accept_color = get_current_color(); // To fix for workstealing

#if DONT_USE_EPOLL
         int color = _choose_new_flow_color(ReadRequest, out);
         cpucb_tail(cwrap_timeleft(ReadRequest, out, color, READ_REQ_DURATION));
#else   //USE EPOLL
         int color = _choose_new_flow_color(ReadRequest, out);
         //fdcb(sock, selread, cwrap(ReadRequest, out, color));
         register_read(color, sock, out);
#endif //DONT_USE_EPOLL

#if TRACE_ACCEPT
         get_hstat(h_Accept,get_current_proc())->accept_nb_successes++;
#endif

      }
      else{
         break;
      }
   }

#if PROFILE_TIME_EVOLUTIONS
   insert_stat_accept(nb_client_accepted_before - nb_client_accepted);
#endif


#if DONT_USE_EPOLL
   cpucb_tail(cwrap_timeleft(Accept, fd, c, c, ACCEPT_DURATION));
#endif

#ifdef PROFILE_APP_HANDLERS
   STOP_HANDLER_PROFILE(Accept);
#endif
}

void ReadRequest(message_t* msg) {
   DEBUG("ReadRequest called on fd %d\n",msg->socket);

#ifdef PROFILE_APP_HANDLERS
   START_HANDLER_PROFILE(ReadRequest);
#endif

#if PROFILE_REQUEST_PROCESSING_DURATION
   if(msg->request_processing_start_time == (uint64_t)-1){
      rdtscll(msg->request_processing_start_time);
   }
#endif

   int rd;
   int total_read = msg->length;
   message_t *out_list = NULL;

   msg->read_color = get_current_color();


   do{
      rd = read(msg->socket, msg->request+msg->length, MAX_REQUEST_SIZE-msg->length-1);

      if ((rd == -1 && errno == ECONNRESET) || rd == 0)  // Connection was close or reseted
      {
         DEBUG("Connection %d was close or reseted\n",msg->socket);
         fdcb(msg->socket, selread, NULL);
         if(total_read > 0){
            msg->close_after_parsing = true;
         }
         else
         {
            goto error;
         }
      }
      else if (rd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
      {
         break;
      }
      else if (rd == -1)
      {
         PRINT_ALERT("Read parameters:\n\t- Fd: %d\n\t- Buffer: %p\n\t- Offset: %d\n\t- BUFFER_SIZE = %d\n",
                  msg->socket,msg->request,msg->length,MAX_REQUEST_SIZE)
                  ;
         PRINT_ALERT("Error while reading on socket %d (errno=%d)\n",msg->socket,errno);
         perror("Reading request");
         _exit(EXIT_FAILURE);
      }
      else
      {
         total_read += rd;
         msg->length += rd;
         assert (msg->length < (MAX_REQUEST_SIZE -1));
      }
   } while(rd > 0 && msg->length < (MAX_REQUEST_SIZE - 1));

   if(total_read > 0)
   {
      msg->request[msg->length] = '\0';

      DEBUG("Request : %s (length %d)\n", msg->request, msg->length);

      // Check \r\n\r\n
      msg->req_end = strstr(msg->request, "\r\n\r\n");
      if(msg->req_end != NULL) {

#if DEBUG_RUID
         char* unique_id = strstr(msg->request, "Request unique id");
         if(unique_id){
            msg->slg_client_num = atoi(unique_id + 19*sizeof(char));
            char* sep = strstr(unique_id + 19*sizeof(char), "-");
            msg->slg_client_req_id = atoi(sep+1);
            char* sep2 = strstr(sep+1, "-");
            msg->slg_client_hostname = atoi(sep2+1);
         }
#endif

         /** it works because multiple messages per connexion are not allowed with
          *  CLOSE_AFTER_REQUEST
          */
#if !CLOSE_AFTER_REQUEST
         if(!msg->close_after_parsing){
            /** Create a new message **/
            message_t *out, *last_list_index = NULL;
            char *last_index = msg->req_end;
            do {
               out = get_new_msg(get_current_proc(), msg->socket);
               out->accept_color = msg->accept_color;
               int req_size = (last_index + (4*sizeof(char))) - msg->request;
               if(req_size != msg->length) {
                  char *end_out = strstr(last_index + 4*sizeof(char), "\r\n\r\n");
                  if(end_out == NULL) { // END
                     memcpy(out->request, last_index + (4*sizeof(char)), msg->length - req_size);
                     out->length = msg->length - req_size;
                     out->request[out->length] = 0;
                     break;
                  } else {
                     memcpy(out->request, last_index + (4*sizeof(char)), end_out - last_index);
                     out->length = msg->length - req_size;
                     out->request[out->length] = 0;
                     out->read_color = get_current_color();
                     out->req_end = out->request + (out->length - 4*sizeof(char));
                     last_index = end_out;

                     if(out_list == NULL) {
                        last_list_index = out_list = out;
                     } else {
                        last_list_index->next_message = out;
                        last_list_index = out;
                     }
                  }
               } else
                  break;
            } while(1);

            int current_color = get_current_color();
#if DONT_USE_EPOLL
            cpucb_tail(cwrap_timeleft(ReadRequest, out, current_color, READ_REQ_DURATION));
#else
            fdcb_finished(true); /* Let the next callback handle the read */
            fdcb(out->socket, selread, cwrap_timeleft(ReadRequest, out, current_color, READ_REQ_DURATION));
#endif // DONT_USE_EPOLL
         }

#else
         //assume we finished reading all the request
         fdcb(msg->socket, selread, NULL);
         msg->close_after_parsing = 1;
#endif //CLOSE_AFTER_REQUEST

         goto success;
      } else if(msg->length == MAX_REQUEST_SIZE - 1) { // Check \r\n\r\n
         /** We have read MAX_REQUEST_SIZE and still have no valid request ? => Kick the client. */
         goto error;
      }
   }

#if DONT_USE_EPOLL
   cpucb_tail(cwrap_timeleft(ReadRequest, msg, msg->socket, READ_REQ_DURATION));
#endif

#ifdef PROFILE_APP_HANDLERS
   STOP_PROCESSING_HANDLER_PROFILE(ReadRequest);
#endif
   return;



success:
      if(nb_pending_treatments_fd[msg->socket]) {
         insert_in_pending_list(msg);
      } else {
         assert(first_pending(msg->socket) == NULL);
         nb_pending_treatments_fd[msg->socket]++;
#if WITH_PARSE_REQUEST_HANDLER
         _register_next(ParseRequest, msg);
#else
         ParseRequest(msg);
#endif
      }
      if(out_list)
         insert_in_pending_list(out_list);


#ifdef PROFILE_APP_HANDLERS
STOP_PROCESSING_HANDLER_PROFILE(ReadRequest);
#endif
      return;




error:
      /** Free the pending message. */
      free_pending_message_list(msg);
      int color = _choose_next_color_in_flow(Close, msg);
      cpucb_tail(cwrap_timeleft(Close, msg, color, CLOSE_DURATION));
      STOP_HANDLER_PROFILE(ReadRequest);
      return;
}


void ParseRequest(message_t* msg){
#if PROFILE_APP_HANDLERS && WITH_PARSE_REQUEST_HANDLER
   START_HANDLER_PROFILE(ParseRequest);
#endif
   /** We need to take care of partial messages here. */


   int parse_error = _parse_http_request(msg, msg->req_end);
   if(parse_error){
      fdcb(msg->socket, selread, NULL);
      cpucb_tail(cwrap(BadRequest,msg,400,msg->socket));
#if PROFILE_APP_HANDLERS && WITH_PARSE_REQUEST_HANDLER
      STOP_HANDLER_PROFILE(ParseRequest);
#endif
      return;
   }

   bool special_req = false;

   DEBUG("File requested: %s\n", msg->file_requested);
   if(strncmp(msg->file_requested, "/dump_hwc", 9) == 0)
   {
      DEBUG("Found dump hwc\n");
      save_bench_time(get_time() - start_time);
      dump_current_state(false);
      special_req = true;
   }
   else if(strncmp(msg->file_requested, "/end_iteration", 14) == 0)
   {
      DEBUG("Found end iteration\n");
      save_bench_time(get_time() - start_time);
      dump_current_state(true);
      special_req = true;
   }
   else if(strncmp(msg->file_requested, "/end_execution", 14) == 0)
   {
      DEBUG("Found end execution\n");
      unsigned long total_time = get_time() - start_time;

      uint64_t stop_time_cyles = 0;
      rdtscll(stop_time_cyles);

      total_time_cycle = stop_time_cyles - start_time_cycle;
      save_bench_time(total_time);

      for(int i = 0; i< task_get_nthreads(); i++)
      {
         DEBUG("Sending stop hw counter to thread %d\n", i);
         cpucb_tail( cpwrap(stop_hw_counters, 0-(i+1), 65535) );
      }
      special_req = true;
   }

   if(msg->close_after_parsing){
      DEBUG("Close after parsing set. Registering close for fd %d\n", msg->socket);
      free_pending_message_list(msg);
      nb_pending_treatments_fd[msg->socket] = 0;
      int color = _choose_next_color_in_flow(Close, msg);
      cpucb_tail(cwrap_timeleft(Close, msg, color, CLOSE_DURATION));
   }
   else if(!special_req)
   {
      DEBUG("Found a request for file %s\n", msg->file_requested);
      // We parsed the request, calling next stage
#if USE_SENDFILE
      _register_next(WriteHeaders, msg);
#else
      _register_next(CheckInCache, msg);
#endif // USE_SENDFILE
   }

#if PROFILE_APP_HANDLERS && WITH_PARSE_REQUEST_HANDLER
   STOP_PROCESSING_HANDLER_PROFILE(ParseRequest);
#endif
}

#if USE_SENDFILE
void WriteHeaders(message_t* msg)
{
#ifdef PROFILE_APP_HANDLERS
   START_HANDLER_PROFILE(WriteHeaders);
#endif

   if(msg->response == NULL)
   {
      // Thats the first time

      int res;
      struct stat sb;

      int fni = 0;
      if(msg->file_requested[0] == '/')
      {
         fni = 1;
      }

      DEBUG("File %s requested\n",&(msg->file_requested[fni]));
      res = stat(&(msg->file_requested[fni]), &sb);

      msg->file_size = sb.st_size;

      if (res < 0)
      {
         fdcb(msg->socket, selread, NULL);
         cpucb_tail(cwrap(FourOhFor,msg,404,msg->socket));
         return;
      }
      char* content;
      if (suffixTest(msg->file_requested, ".html"))
      {
         content = "text/html";
      }
      else if (suffixTest(msg->file_requested, ".png"))
      {
         content = "image/png";
      }
      else if (suffixTest(msg->file_requested, ".jpg") || suffixTest(msg->file_requested, ".jpeg"))
      {
         content = "image/jpeg";
      }
      else if (suffixTest(msg->file_requested, ".gif"))
      {
         content = "image/gif";
      }
      else
      {
         content = "text/plain";
      }

      msg->response = (char*) calloc(1,sizeof(char)*(MAX_HEADER_SIZE));
      assert(msg->response != NULL);

      sprintf(msg->response, "HTTP/1.1 200 OK\r\nServer: Markov 0.1\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n",
               content,msg->file_size);

      DEBUG("Header is %s",msg->response);

      msg->in_cache = false;
      msg->length = 0;
      msg->response_size = strlen(msg->response);

      // Cork -- disallow sending response
      int optval = 1;
      socklen_t optlen = sizeof (optval);
      if (setsockopt (msg->socket, IPPROTO_TCP, TCP_CORK, &optval, optlen) < 0)
      {
         PRINT_ALERT("Error on accept when seting TCP_CORK, errno is : %d (%s)\n",
                  errno,strerror(errno));
         _exit(EXIT_FAILURE);
      }
   }

   int wr = write(msg->socket,msg->response+msg->length, msg->response_size-msg->length);

   if(wr == -1 && errno != EAGAIN)
   {
      //perror("Write header error");
      //_exit(EXIT_FAILURE);
      _register_next(FreeRequest,msg);
      return;
   }
   else if(wr == -1 && errno == EAGAIN)
   {
      wr = 0;
   }

   msg->length += wr;

   if(msg->length == msg->response_size)
   {
      msg->length = 0;
      msg->response_size = 0;
      _register_next(SendFile,msg);
   }
   else
   {
      cpucb_tail(cwrap(WriteHeaders,msg,msg->socket));
   }

#ifdef PROFILE_APP_HANDLERS
   STOP_PROCESSING_HANDLER_PROFILE(WriteHeaders);
#endif
}

void SendFile(message_t* msg)
{
#ifdef PROFILE_APP_HANDLERS
   START_HANDLER_PROFILE(SendFile);
#endif
   int fni = 0;
   if(msg->file_requested[0] == '/')
   {
      fni = 1;
   }

   if(msg->read_fd == -1)
   {

      msg->read_fd = open(&(msg->file_requested[fni]), O_RDONLY);

      if (msg->read_fd < 0)
      {
         perror("Reading");
         _exit(EXIT_FAILURE);
      }

      msg->offset = 0;
   }

   int wr = sendfile(msg->socket,msg->read_fd,&msg->offset ,msg->file_size-msg->length);

   if(wr == -1 && errno != EAGAIN)
   {
      DEBUG("Write error : errno is %d\n",errno);
      //perror("Write error");
      //_exit(EXIT_FAILURE);

      _register_next(FreeRequest,msg);
      return;
   }
   else if(wr == -1 && errno == EAGAIN)
   {
      wr = 0;
   }

   msg->length += wr;

   if(msg->length == msg->file_size)
   {
      // Uncorking -- Authorize sending response
      int optval = 0;
      socklen_t optlen = sizeof (optval);
      if (setsockopt (msg->socket, IPPROTO_TCP, TCP_CORK, &optval, optlen) < 0)
      {
         PRINT_ALERT("Error on accept when seting TCP_CORK, errno is : %d (%s)\n",
                  errno,strerror(errno));
         _exit(EXIT_FAILURE);
      }

      close(msg->read_fd);
      _register_next(FreeRequest,msg);
   }
   else
   {
      cpucb_tail(cwrap(SendFile,msg,msg->socket));
   }

#ifdef PROFILE_APP_HANDLERS
   STOP_PROCESSING_HANDLER_PROFILE(SendFile);
#endif

}
#else

void CheckInCache(message_t *msg) {
#ifdef PROFILE_APP_HANDLERS
   START_HANDLER_PROFILE(CheckInCache);
#endif

   int fni = 0;
   if (msg->file_requested[0] == '/') {
      fni = 1;
   }

   //print_cache();
   msg->response = cache[&(msg->file_requested[fni])];
   msg->in_cache = true;
   msg->length = 0;

   if (msg->response == NULL) {
      PRINT_ALERT("File %s not found\n",&(msg->file_requested[fni]));
      fdcb(msg->socket, selread, NULL);
      cpucb_tail(cwrap(FourOhFor, msg, 404, msg->socket));
      return;
   }

   DEBUG("Response is %s",msg->response);
   msg->response_size = strlen(msg->response);

#if USE_GZIP && ONLY_UNFREQUENT_FILE_USE_GZIP
   if(strstr(msg->file_requested,"frequent_files") != NULL){
      _register_next(Write, msg);
   }
   else{
      _register_next(CompressResponse, msg);
   }

#elif UNFREQUENT_FILE_USE_FILESUMMER
   if(strstr(msg->file_requested,"frequent_files") != NULL){
      _register_next(Write, msg);
   }
   else{
      _register_next(FileSummerStage, msg);
   }
#elif USE_GZIP
   _register_next(CompressResponse, msg);
#elif WITH_FAKE_CPU_STAGE

#if UNFREQUENT_FILE_USE_FAKE_CPU_STAGE
   if(strstr(msg->file_requested,"frequent_files") != NULL){
      _register_next(Write, msg);
   }
   else{
      _register_next(FakeCpuStage, msg);
   }
#else
   static __thread unsigned int nb_requests = 0;
   nb_requests++;
   if(nb_requests >= CPU_STAGE_FREQUENCY){
      _register_next(FakeCpuStage, msg);
      nb_requests = 0;
   }
   else{
      _register_next(Write, msg);
   }
#endif

#elif WITH_FILESUMMER_STAGE
   _register_next(FileSummerStage, msg);
#elif WITH_FAKE_SMALL_STAGES
   _register_next(FakeSmallStage, msg);
#elif WITH_FAKE_CACHE_STAGES
   msg->nb_fcs_calls = 0;
   msg->previous_color = get_current_color();

   _register_next(FakeCacheStage, msg, next_fake_color++);
#else
   _register_next(Write, msg);
#endif //WITH_FAKE_??

#ifdef PROFILE_APP_HANDLERS
   STOP_PROCESSING_HANDLER_PROFILE(CheckInCache);
#endif
}

#endif // USE_SENDFILE

#if USE_GZIP
#if USE_EVENT_DRIVEN_GZIP
void end_compress(void *arg);
#endif

#if USE_EVENT_DRIVEN_GZIP
static __thread int nb_simultaneous_compress = 0;
#endif

void CompressResponse(message_t* msg){
   START_HANDLER_PROFILE(CompressResponse);
   assert(msg->response);

#if USE_EVENT_DRIVEN_GZIP
   if(nb_simultaneous_compress >= ZLIB_MAX_SIMULTANEOUS_COMPRESSIONS) {
      // We'd like a way to indicate that we want to put that on another core because we are too busy.
      // -> CompressResponse pipelining queue ? Workstealing priority ?
      cpucb_tail(cpwrap(CompressResponse, msg, get_current_color(), -400));
      return;
   }
   nb_simultaneous_compress++;
#endif

#  if ONLY_UNFREQUENT_FILE_USE_GZIP
   /** The header was put before so we search for the correct index of the base file. */
   DEBUG("Before: %s\n", msg->response);
   char *base_file = strstr(msg->response,"\r\n\r\n");
   assert(base_file);
   base_file += 4;
   DEBUG("After : %s\n", base_file);
#  else
   /** No header was ever added so it's OK. */
   char *base_file = msg->response;
#  endif

   uLongf file_length = strlen(base_file)+1;
   msg->response = (char*)malloc(file_length * 1.01 + 12 + MAX_HEADER_SIZE);
   if(!msg->response) {
      fprintf(stderr, "%s:%d No more memory !\n", __FILE__, __LINE__);
      exit(-42);
   }
   strcpy(msg->response, "HTTP/1.1 200 OK\r\nServer: Markov 0.1\r\nContent-Encoding: deflate\r\nContent-Type: text/html\r\nContent-Length:       \r\n\r\n");
   int actual_length = strlen(msg->response); // Should not take too much time.

#  if !USE_EVENT_DRIVEN_GZIP
   uLongf compressed_length = file_length*1.01-actual_length-1;
   int err = compress((Bytef*)(msg->response + actual_length), &compressed_length, (const Bytef*)base_file, file_length);
   if(err < 0) {
      printf("ERROR %d when calculating GZIP sum of: %s\n", err, base_file);
      printf("%d : %s\n", (int)file_length, base_file);
      _exit(EXIT_FAILURE);
   }
   itoa((int)compressed_length, (char*)(msg->response + actual_length - 10), 10);
   msg->response_size = actual_length+compressed_length;
   msg->in_cache = false;

   _register_next(Write, msg);
#  else
   msg->length = file_length*1.01-actual_length-1;
   msg->response_size = actual_length;
   async_compress((msg->response + actual_length), &(msg->length), base_file, file_length, end_compress, (void*)msg);
#  endif

   STOP_PROCESSING_HANDLER_PROFILE(CompressResponse);
}

#if USE_EVENT_DRIVEN_GZIP
void end_compress(void *arg) {
   nb_simultaneous_compress--;
   assert(nb_simultaneous_compress >= 0);

   message_t *msg = (message_t*)arg;
   itoa(msg->length, (char*)(msg->response + msg->response_size - 10), 10);
   msg->response_size += msg->length;
   msg->in_cache = false;
   msg->length = 0;
   _register_next(Write, msg);
}
#endif
#endif


#if WITH_FAKE_CPU_STAGE
static inline void cpu_burn() {
   uint64_t tts, tte;
   rdtscll(tts);
   tte = tts;
   while ((tte - tts) < CPU_STAGE_DURATION) {
      rdtscll(tte);
   }
}

void dummy_log_event(uint64_t len){
   uint64_t tts, tte;
     rdtscll(tts);
     tte = tts;
     while ((tte - tts) < len) {
        rdtscll(tte);
     }
}

/*void FakeCpuStage(message_t* msg)
{
#ifdef PROFILE_APP_HANDLERS
   START_HANDLER_PROFILE(FakeCpuStage);
#endif

#define DUMMMY_LOG_EVENT_TIME   10

   int color = 3;//get_current_color();
   int nb_to_prod = 10*get_current_proc();

   for(int i=0; i<nb_to_prod; i++){
         cpucb(cwrap_timeleft(dummy_log_event, DUMMMY_LOG_EVENT_TIME, color, DUMMMY_LOG_EVENT_TIME));
   }

   for(int i=0; i<nb_to_prod; i++){
      cpucb_tail(cwrap_timeleft(dummy_log_event, DUMMMY_LOG_EVENT_TIME, color, DUMMMY_LOG_EVENT_TIME));
   }

   //cpu_burn();
   _register_next(Write,msg);

#ifdef PROFILE_APP_HANDLERS
   STOP_PROCESSING_HANDLER_PROFILE(FakeCpuStage);
#endif
}*/

void FakeCpuStage(message_t* msg)
{
#ifdef PROFILE_APP_HANDLERS
   START_HANDLER_PROFILE(FakeCpuStage);
#endif

#define DUMMMY_LOG_EVENT_TIME   100

  // int proc = get_current_proc();
 //  int nb_to_prod = (1+proc)*1;

//   int color = msg->socket;//get_current_color();

   //Rules legWS to have cost on ws
   /*for(int i=0; i<nb_to_prod; i++){
         cpucb(cwrap_timeleft(dummy_log_event, DUMMMY_LOG_EVENT_TIME, color, DUMMMY_LOG_EVENT_TIME));
   }

   for(int i=0; i<nb_to_prod; i++){
      cpucb_tail(cwrap_timeleft(dummy_log_event, DUMMMY_LOG_EVENT_TIME, color, DUMMMY_LOG_EVENT_TIME));
   }*/

   //Try to kill core 0, or 7
   //5 GOOD and 8 color very good
   //3 Good
   for(int i=0; i<1; i++){
      cpucb_tail(cwrap_timeleft(dummy_log_event, DUMMMY_LOG_EVENT_TIME*50, 7, DUMMMY_LOG_EVENT_TIME*50));
   }

   //cpu_burn();
   _register_next(Write,msg);

#ifdef PROFILE_APP_HANDLERS
   STOP_PROCESSING_HANDLER_PROFILE(FakeCpuStage);
#endif
}


#endif

#if WITH_FILESUMMER_STAGE || UNFREQUENT_FILE_USE_FILESUMMER

//__thread unsigned char sum[FILESUMMER_RESPONSE_SIZE+1];
void FileSummerStage(message_t* msg){
#ifdef PROFILE_APP_HANDLERS
   START_HANDLER_PROFILE(FileSummerStage);
#endif
   unsigned char sum[FILESUMMER_RESPONSE_SIZE+1];
   memset(sum, 'a', sizeof(unsigned char) * (FILESUMMER_RESPONSE_SIZE));
   sum[FILESUMMER_RESPONSE_SIZE] = 0;

   unsigned int idx = 0;

   assert(FILESUMMER_RESPONSE_SIZE < msg->response_size);

   for(int i = 0; i < msg->response_size; i++){
      // We want an ascii char
      sum[idx] = (sum[idx] + msg->response[i]) % 62;
      if(sum[idx] < 10){
         sum[idx]+= 48;
      }
      else if(sum[idx] < 36){
         sum[idx] = (sum[idx] -10) + 65;
      }
      else{
         sum[idx] = (sum[idx] - 36) + 97;
      }
      idx = (idx + 1) % FILESUMMER_RESPONSE_SIZE;
   }

   //msg->response = (char*) calloc(1, sizeof(char)*(FILESUMMER_RESPONSE_SIZE+MAX_HEADER_SIZE));
   msg->response = (char*) malloc(sizeof(char)*(FILESUMMER_RESPONSE_SIZE+MAX_HEADER_SIZE));
   assert(msg->response);
   msg->in_cache = false;

   assert(msg->response != NULL);
   sprintf(msg->response, "HTTP/1.1 200 OK\r\nServer: Markov 0.1\r\nContent-Type: sws/filesum\r\nContent-Length: %d\r\n\r\n%s%c",
            FILESUMMER_RESPONSE_SIZE, sum, 0);

   DEBUG("Response is %s\n", msg->response);

   msg->response_size = strlen(msg->response);

   _register_next(Write,msg);

#ifdef PROFILE_APP_HANDLERS
   STOP_PROCESSING_HANDLER_PROFILE(FileSummerStage);
#endif
}
#endif

#if WITH_FAKE_SMALL_STAGES
void FakeSmallStage(message_t* msg){
#ifdef PROFILE_APP_HANDLERS
   START_HANDLER_PROFILE(FakeSmallStage);
#endif

   DEBUG("called %d time for socket %d\n", msg->nb_fs_calls, msg->socket);
   uint64_t fcs, fse;
   rdtscll(fcs);
   fse = fcs;
   while ((fse - fcs) < FAKE_STAGE_DURATION) {
      rdtscll(fse);
   }

   if(++msg->nb_fs_calls < HOW_MANY_FAKE_STAGES){
      _register_next(FakeSmallStage, msg);
   }
   else{
      _register_next(Write, msg);
   }

#ifdef PROFILE_APP_HANDLERS
   STOP_PROCESSING_HANDLER_PROFILE(FakeSmallStage);
#endif
}
#endif //WITH_FAKE_SMALL_STAGES

#if WITH_FAKE_CACHE_STAGES
void FakeCacheStage(message_t* msg){
#ifdef PROFILE_APP_HANDLERS
   START_HANDLER_PROFILE(FakeCacheStage);
#endif

   DEBUG("called %d time for socket %d with color %d\n", msg->nb_fcs_calls, msg->socket, get_current_color());
   uint64_t fcs, fse;
   rdtscll(fcs);
   fse = fcs;
   while ((fse - fcs) < FAKE_CACHE_STAGE_DURATION) {
      rdtscll(fse);
   }

   if(++msg->nb_fcs_calls < FAKE_CACHE_PIPELINE_SIZE){
      _register_next(FakeCacheStage, msg, get_current_color());
   }
   else{
      _register_next(Write, msg, msg->previous_color);
   }

#ifdef PROFILE_APP_HANDLERS
   STOP_PROCESSING_HANDLER_PROFILE(FakeCacheStage);
#endif
}
#endif //WITH_FAKE_CACHE_STAGES


void Write(message_t *msg) {
#ifdef PROFILE_APP_HANDLERS
   START_HANDLER_PROFILE(Write);
#endif

   DEBUG("Writing on the socket %d\n",msg->socket);

#if DEBUG_RUID
   struct sockaddr_in csin;
   unsigned int size = sizeof(csin);
   int r = getpeername(msg->socket, (struct sockaddr*)&csin, &size);

   char* ip = (char*) csin.sin_addr.s_addr;
   if(r == 0 && ip[3] != msg->slg_client_hostname){
      PANIC("dest ip (= %d) != msg->slg_client_num (= %d)\n", ip[3], msg->slg_client_hostname);
   }

   if(msg->length == 0){
      char* file_content = msg->response;
      msg->response = (char*) malloc(sizeof(char)*(msg->response_size+MAX_HEADER_SIZE));
      assert(msg->response);
      msg->in_cache = false;

      assert(msg->response != NULL);
      sprintf(msg->response, "HTTP/1.1 200 OK\r\nServer: Markov 0.1\r\nContent-Type: sws/filesum\r\nContent-Length: %d\r\nRequest unique id: %d-%d-%d\r\n\r\n%s%c",
               msg->response_size,
               msg->slg_client_num, msg->slg_client_req_id, msg->slg_client_hostname,
               file_content, 0);

      msg->response_size = strlen(msg->response);
   }
#endif

#if PROFILE_APP_HANDLERS
   uint64_t real_write_cost_start, real_write_cost_stop;
   rdtscll(real_write_cost_start);
#endif

   int wr = write(msg->socket, msg->response + msg->length, msg->response_size - msg->length);

#if PROFILE_APP_HANDLERS
   rdtscll(real_write_cost_stop);
   get_hstat(h_Write,get_current_proc())->write_length += wr;
   get_hstat(h_Write,get_current_proc())->write_real_duration += (real_write_cost_stop - real_write_cost_start);
#endif

   if (wr == -1 && (errno != EAGAIN && errno != EBADF && errno != EPIPE && errno != ECONNRESET)) {
      int err= errno;
      PRINT_ALERT("Write error on socket %d (return %d): errno is %d (%s)\n",msg->socket,wr,err,strerror(err));
      _exit(EXIT_FAILURE);
   }
   else if (wr == -1 && errno == EAGAIN) {
      wr = 0;
   }
   else if (wr == -1 && (errno == EBADF || errno == EPIPE || errno == ECONNRESET)) {
      // Socket has been probably closed before by ReadRequest
      _register_next(FreeRequest, msg);
      STOP_HANDLER_PROFILE(Write);
      return;
   }

   msg->length += wr;
   DEBUG("Written %d on %d bytes\n",msg->length,msg->response_size);

   if(msg->length == msg->response_size)
   {
      // All things have been written
      _register_next(FreeRequest, msg);
   }
   else {
      cpucb_tail(cwrap_timeleft(Write, msg, get_current_color(), WRITE_DURATION));
   }

#ifdef PROFILE_APP_HANDLERS
   STOP_PROCESSING_HANDLER_PROFILE(Write);
#endif
}


void take_care_of_HTTP_pipelining(int msg_socket) {
   message_t *msg = pop_first_pending(msg_socket); // Exists
   if(msg->req_end != NULL) {
      int req_size = (msg->req_end + (4*sizeof(char))) - msg->request;
      assert(req_size == msg->length);
      #if WITH_PARSE_REQUEST_HANDLER
         _register_next(ParseRequest, msg);
      #else
         ParseRequest(msg);
      #endif
   } else {
      PANIC("Partial message in the pending list ?");
   }
}

void FreeRequest(message_t *msg) {
   DEBUG("Finished treating --%s--\n", msg->request);
   DEBUG("Answer is --%s--\n", msg->response);

   int msg_socket = msg->socket;

#ifdef PROFILE_APP_HANDLERS
   START_HANDLER_PROFILE(FreeRequest);
#endif
#if PROFILE_REQUEST_PROCESSING_DURATION
   uint64_t r_start_time = msg->request_processing_start_time;
   uint64_t r_stop_time;
#endif

   if (!msg->in_cache) {
      free(msg->response);
   }

#if REUSE_MESSAGES
   insert_on_msg_freelist(msg, get_current_proc());
#else
#if USE_OVER_ALLOCATOR
   EAllocator<sizeof(message_t)>::free(msg);
#else
   delete msg;
#endif
#endif


   if(first_pending(msg_socket) != NULL) {
      take_care_of_HTTP_pipelining(msg_socket);
   } else {
      nb_pending_treatments_fd[msg_socket]--;
      assert(nb_pending_treatments_fd[msg_socket] == 0);
   }

#if PROFILE_REQUEST_PROCESSING_DURATION
   rdtscll(r_stop_time);
   get_rstat(get_current_proc())->nb_request_processed++;
   get_rstat(get_current_proc())->total_request_real_time += (r_stop_time - r_start_time);
#endif

#ifdef PROFILE_APP_HANDLERS
   STOP_PROCESSING_HANDLER_PROFILE(FreeRequest);
#endif
}

void Close(message_t *msg) {
#ifdef PROFILE_APP_HANDLERS
   START_HANDLER_PROFILE(Close);
#endif

   int s = msg->socket;
   if(nb_pending_treatments_fd[s]) {
      cpucb_tail(cwrap_timeleft(Close, msg, get_current_color(), CLOSE_DURATION));
      return;
   }

   if(!msg->in_cache && msg->response != NULL){
     free(msg->response);
   }
   
   DEBUG("Close called for socket %d\n",s);

   if (close(s) == -1) {
      PRINT_ALERT("Error when closing socket %d (errno %d)\n",s,errno)
      ;
      perror("Close: ");
      _exit(-1);
   }

   DEBUG("Close returned %s\n",strerror(errno));
   
#if BATCH_REGISTER_TASK && !BATCH_REGISTER_TASK_FOR_ALL
   cpucb_batch_register(cwrap(Dec_Accepted_Clients, msg->accept_color));
#else
   cpucb_tail(cwrap(Dec_Accepted_Clients, msg->accept_color));
#endif

#if REUSE_MESSAGES
   insert_on_msg_freelist(msg, get_current_proc());
#else
#if USE_OVER_ALLOCATOR
   EAllocator<sizeof(message_t)>::free(msg);
#else
   delete msg;
#endif
#endif

#ifdef PROFILE_APP_HANDLERS
   STOP_HANDLER_PROFILE(Close);
#endif
}

void Dec_Accepted_Clients() {
#if ACCEPT_PER_CORE || ACCEPT_PER_INTERFACE
   unsigned int color = get_current_color();
   nb_client_accepted[color]--;
   assert(nb_client_accepted[color] >= 0);
#else
   nb_client_accepted--;
   assert(nb_client_accepted >= 0);
#endif //ACCEPT_PER_CORE
}

void FourOhFor(message_t *in, int err) {
#ifdef PROFILE_APP_HANDLERS
   START_HANDLER_PROFILE(FourOhFor);
#endif

   char *msg = "HTTP/1.1 404 File not found\r\n";
   int r = write(in->socket, msg, strlen(msg));
   msg = "<html><body><h2>404 File Not Found!</h2></body></html>\n";
   write_headers(in->socket, true, strlen(msg), "text/html");
   r = write(in->socket, msg, strlen(msg));

   free_pending_message_list(in);
   nb_pending_treatments_fd[in->socket] = 0;

   int color = _choose_next_color_in_flow(Close, in);
   cpucb_tail(cwrap(Close, in, color));

#ifdef PROFILE_APP_HANDLERS
   STOP_HANDLER_PROFILE(FourOhFor);
#endif
}

void BadRequest(message_t *in, int err) {
#ifdef PROFILE_APP_HANDLERS
   START_HANDLER_PROFILE(BadRequest);
#endif

   char *msg;
   int r;
   switch (err) {

      case 400:
         msg = "HTTP/1.1 400 Bad Request\r\n";
         if (write(in->socket, msg, strlen(msg)) != -1) {
            msg = "<html><body><h2>400 Bad Request!</h2></body></html>\n";
            write_headers(in->socket, true, strlen(msg), "text/html");
            r = write(in->socket, msg, strlen(msg));
         }
         break;

      case 501:
         msg = "HTTP/1.1 501 Not Implemted\r\n";
         if (write(in->socket, msg, strlen(msg)) != -1) {
            msg = "<html><body><h2>501 Not Implemented!</h2></body></html>\n";
            write_headers(in->socket, true, strlen(msg), "text/html");
            r = write(in->socket, msg, strlen(msg));
         }
         break;

      case 408:
         msg = "HTTP/1.1 408 Request Timeout\r\n";
         if (write(in->socket, msg, strlen(msg)) != -1) {
            msg = "<html><body><h2>408 Request Timeout!</h2></body></html>\n";
            write_headers(in->socket, true, strlen(msg), "text/html");
            r = write(in->socket, msg, strlen(msg));
         }
         break;
   }

   free_pending_message_list(in);
   nb_pending_treatments_fd[in->socket] = 0;

   int color = _choose_next_color_in_flow(Close, in);
   cpucb_tail(cwrap(Close, in, color));

#ifdef PROFILE_APP_HANDLERS
   STOP_HANDLER_PROFILE(BadRequest);
#endif
}
