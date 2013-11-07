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

/** In order to support HTTP pipelining, we must have a pending message list. */
static message_t** pending_request;
message_t* insert_in_pending_list(message_t *msg) {
   if(pending_request[msg->socket] == NULL) {
      pending_request[msg->socket] = msg;
      return msg;
   }

   message_t *list = pending_request[msg->socket];
   while(list->next_message)
      list = list->next_message;
   if(list != msg)
      list->next_message = msg;
   return pending_request[msg->socket];
}

message_t* first_pending(int socket) {
   if(pending_request[socket]) {
      return pending_request[socket];
   }
   return NULL;
}

message_t* pop_first_pending(int socket) {
   if(pending_request[socket]) {
      message_t *ret = pending_request[socket];
      pending_request[socket] = ret->next_message;
      return ret;
   }
   return NULL;
}

void free_pending_message_list(message_t *msg) {
   message_t *list = pending_request[msg->socket];
   while(list) {
      message_t *temp = list->next_message;
      if(list!=msg)
         delete list;
      list = temp;
   }
}


char *itoa(int value, char *string, int radix) {
  char tmp[33];
  char *tp = tmp;
  int i;
  unsigned v;
  int sign;
  char *sp;

  if (radix > 36 || radix <= 1)
  {
    errno = EDOM;
    return 0;
  }

  sign = (radix == 10 && value < 0);
  if (sign)
    v = -value;
  else
    v = (unsigned)value;
  while (v || tp == tmp)
  {
    i = v % radix;
    v = v / radix;
    if (i < 10)
      *tp++ = i+'0';
    else
      *tp++ = i + 'a' - 10;
  }

  if (string == 0)
    string = (char *)malloc((tp-tmp)+sign+1);
  sp = string;

  if (sign)
    *sp++ = '-';
  while (tp > tmp)
    *sp++ = *--tp;
  return string;
}


int _parse_http_request(message_t* msg, char* req_end){
   // Simple parsing, I need to find 3 things GET file HTTP/1.1
   // Other things are ignored
   // Parsing similar to the one done in u-server
   char* cur = msg->request;
   char* cur_token_beg;
   int cur_word_length = 0;
   int parse_error = 0;

   for(int i = PARSE_METHOD; i < PARSE_END; i++){
      cur_word_length = 0;
      cur_token_beg = cur;
      while (cur < req_end
               &&  cur_word_length < msg->length
               && !isspace(*cur)
               && *cur != '\r'
               && *cur != '\n') {
         cur++;
         cur_word_length++;
      }
      *cur++ = 0;

      switch(i){
         case PARSE_METHOD:
            DEBUG("New request:\n");
            DEBUG("\tMethod = %s\n", cur_token_beg);
            if(!strcmp(cur_token_beg, "GET") == 0) {
               PRINT_ALERT("Only GET is supported (not %s)\n", cur_token_beg);
               parse_error = 1;
            }
            break;
         case PARSE_HTTP_VERSION:
            DEBUG("\tHTTP Version = %s\n", cur_token_beg);
            if(!strcmp(cur_token_beg, "HTTP/1.1") == 0) {
               PRINT_ALERT("Only HTTP/1.1 is supported (not %s)\n", cur_token_beg);
               parse_error = 1;
            }
            break;
         case PARSE_FILE:
            msg->file_requested = cur_token_beg;
            DEBUG("\tFile requested = %s\n", msg->file_requested);
            break;
         default:
            PRINT_ALERT("Unknown parse action. Bug...");
            _exit(EXIT_FAILURE);
      }
   }

   if(parse_error){
      return parse_error;
   }

   while(cur < req_end && (isspace(*cur) || *cur == '\r' || *cur == '\n')){
      cur++;
   }

   return parse_error;
}

/* The signal SIGPIPE handler function */
void sigpipe_handler(int signal) {
   //printf("ignoring SIGPIPE: %d\n", signal);
}


void misc_init() {
   signal( SIGPIPE, sigpipe_handler); /* Registering the handler, catching SIGPIPE signals */
   pending_request = (message_t**) calloc(100000, sizeof(*pending_request));
}

void print_footer() {
   printf("********************************\n\n");


   DEBUG("--------------------------------------------------------\n");
   DEBUG("Accept ptr: %p\n", (void *) Accept);
   DEBUG("ReadRequest ptr: %p\n", (void *) ReadRequest);
#if USE_SENDFILE
   DEBUG("WriteHeaders ptr: %p\n", (void *) WriteHeaders);
   DEBUG("SendFile ptr: %p\n", (void *) SendFile);
#else
   //DEBUG("FILE_HANDLER ptr: %p\n", (void *) FILE_HANDLER);
   DEBUG("Write ptr: %p\n", (void *) Write);
#endif

   DEBUG("FreeRequest ptr: %p\n", (void *) FreeRequest);
   DEBUG("Close ptr: %p\n", (void *) Close);
   DEBUG("--------------------------------------------------------\n\n");
}

#if REUSE_MESSAGES
static msg_freelist_t message_freelist[MAX_THREADS];

void insert_on_msg_freelist(message_t* msg, int core_no){
   if(message_freelist[core_no].val.message_freelist_count < REUSE_MESSAGES_FREELIST_SIZE){
      msg->next_free_msg = message_freelist[core_no].val.message_freelist;
      message_freelist[core_no].val.message_freelist = msg;
      message_freelist[core_no].val.message_freelist_count++;
   }
   else{
#if USE_OVER_ALLOCATOR
      EAllocator<sizeof(message_t)>::free(msg);
#else
      delete msg;
#endif
   }
}

void init_msg_freelist(int nthreads){
   for(int i = 0 ; i < nthreads; i++){
      for(int j = 0; j < REUSE_MESSAGES_FREELIST_SIZE; j++){
#if USE_OVER_ALLOCATOR
         message_t *msg = (message_t*) EAllocator<sizeof(message_t)>::alloc(core_no);
#else
         message_t* msg = new message_t;
#endif
         insert_on_msg_freelist(msg, i);
      }
   }
}
#endif

message_t* get_new_msg(int core_no, int sock){
#if PROFILE_REQUEST_PROCESSING_DURATION
   uint64_t gnm_start_time;
   rdtscll(gnm_start_time);
#endif

   message_t* out;
#if REUSE_MESSAGES
   if(message_freelist[core_no].val.message_freelist_count < REUSE_MESSAGES_FREELIST_SIZE){
      out = message_freelist[core_no].val.message_freelist;
      message_freelist[core_no].val.message_freelist = message_freelist[core_no].val.message_freelist->next_free_msg;
      out->next_free_msg = NULL;
      return out;
   }
   else{
#if USE_OVER_ALLOCATOR
      out = (message_t*) EAllocator<sizeof(message_t)>::alloc(core_no);
#else
      out = new message_t;
#endif
   }
#else
#if USE_OVER_ALLOCATOR
   out = (message_t*) EAllocator<sizeof(message_t)>::alloc(core_no);
#else
   out = new message_t;
#endif
#endif //REUSE_MESSAGES

   out->socket = sock;
   out->length = 0;
   out->file_requested = NULL;
   out->request[0] = '\0';
   out->in_cache = false;
   out->response = NULL;

   out->close_after_parsing = false;
   out->req_end = NULL;

#if USE_SENDFILE
   out->read_fd = -1;
   out->file_size = -1;
#endif //USE_SENDFILE

#if WITH_FAKE_SMALL_STAGES
   out->nb_fs_calls = 0;
#endif //WITH_FAKE_SMALL_STAGES

#if PROFILE_REQUEST_PROCESSING_DURATION
   out->request_processing_start_time = (uint64_t)-1;
#endif

   out->accept_color = -42;
   out->read_color = -42;     // NEEDS to be -42 !
   out->next_message = NULL;
   return out;
}

/***********************/
/** Utility functions **/
/***********************/
int suffixTest(const char *val, char *suffix) {
   int len = strlen(val);
   int s_len = strlen(suffix);
   int i;

   for (i=0; i<s_len; i++) {
      if (val[len-i]!=suffix[s_len-i])
         return 0;
   }
   return 1;
}

void write_headers(int socket_in, bool close, int length, char *content) {
   char msg[128];

   sprintf(msg, "Content-Length: %d\r\n", length);
   int r = write(socket_in, msg, strlen(msg));
   sprintf(msg, "Server: Markov 0.1\r\n");
   r = write(socket_in, msg, strlen(msg));
   sprintf(msg, "Content-Type: %s\r\n", content);
   r = write(socket_in, msg, strlen(msg));
   if (close) {
      r = write(socket_in, "Connection: close\r\n", 19);
   }
   r = write(socket_in, "\r\n", 2);
}

/*
int setNonblocking(int fd) {
   DEBUG("Setting fd %d in a non-blocking mode\n",fd);

   int x;
   x=fcntl(fd, F_GETFL, 0);
   return fcntl(fd, F_SETFL, x | O_NONBLOCK);
}
*/

unsigned long get_time() {
   struct timeval time;
   gettimeofday(&time, NULL);
   return time.tv_sec * 1000000 + time.tv_usec;
}

void print_socket_option(int fd) {
   socklen_t optlen;
   int sendwin;
   int rcvwin;
   int optval;

   PRINT_ALERT("#########  Socket options #########\n")
   ;

   /** Send buffer **/
   optlen = sizeof(int);
   if (getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sendwin, &optlen) < 0) {
      PRINT_ALERT("Get SO_SNDBUF didn't work\n")
      ;}
   else
   {
      PRINT_ALERT("\t* SO_SNDBUF is %d\n", sendwin);
   }

   /** Receive buffer **/
   optlen = sizeof(int);
   if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rcvwin, &optlen) < 0)
   {
      PRINT_ALERT("Get SO_RCVBUF didn't work\n");
   }
   else
   {
      PRINT_ALERT("\t* SO_RCVBUF is %d\n", rcvwin);
   }

   /* Check the keepalive */
   optlen = sizeof(int);
   if(getsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) < 0)
   {
      PRINT_ALERT("Get SO_KEEPALIVE didn't work\n");
   }
   else
   {
      PRINT_ALERT("\t* SO_KEEPALIVE is %s\n", (optval ? "ON" : "OFF"));
   }

   /* Check the REUSEADDR */
   optlen = sizeof(int);
   if(getsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, &optlen) < 0)
   {
      PRINT_ALERT("Get SO_REUSEADDR didn't work\n");
   }
   else
   {
      PRINT_ALERT("\t* SO_REUSEADDR is %s\n", (optval ? "ON" : "OFF"));
   }

   /* Check the NO_DELAY */
   optlen = sizeof(int);
   if(getsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &optval, &optlen) < 0)
   {
      PRINT_ALERT("Get TCP_NODELAY didn't work\n");
   }
   else
   {
      PRINT_ALERT("\t* TCP_NODELAY is %s\n", (optval ? "ON" : "OFF"));
   }

   /* Check the linger */
   struct linger ling;
   optlen = sizeof(struct linger);

   if (getsockopt(fd, SOL_SOCKET, SO_LINGER, &ling, &optlen) < 0)
   {
      PRINT_ALERT("Get SO_LINGER didn't work\n");
   }
   else
   {
      PRINT_ALERT("\t* SO_LINGER %s, time = %d\n",
               ling.l_onoff ? "ON" : "OFF", ling.l_linger);
   }

   /* Check the RCV_TMEOUT */
   struct timeval timer;
   optlen = sizeof(timer);

   if(getsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timer, &optlen) < 0)
   {
      PRINT_ALERT("Get SO_RCVTIMEO didn't work\n");
   }
   else
   {
      PRINT_ALERT("\t* SO_RCVTIMEO is %ld.%06ld\n",timer.tv_sec, timer.tv_usec);
   }

   /* Check the SND_TMEOUT */
   optlen = sizeof(timer);

   if(getsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timer, &optlen) < 0)
   {
      PRINT_ALERT("Get SO_SNDTIMEO didn't work\n");
   }
   else
   {
      PRINT_ALERT("\t* SO_SNDTIMEO is %ld.%06ld\n",timer.tv_sec, timer.tv_usec);
   }

   PRINT_ALERT("###################################\n");
}
