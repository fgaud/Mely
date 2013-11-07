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

#ifndef _SWS_MISC_H
#define	_SWS_MISC_H

#include "sws.h"
void misc_init();
void print_footer();

int _parse_http_request(message_t* msg, char* req_end);
void write_headers(int socket_in, bool close, int length, char *content);
int suffixTest(const char *val, char *suffix);
void parse_wanted_mapping(char* mapping);

void print_socket_option(int fd);
int setNonblocking(int fd);
unsigned long get_time();


char *itoa(int value, char *string, int radix);

#if REUSE_MESSAGES
#define REUSE_MESSAGES_FREELIST_SIZE            100

struct _msg_freelist{
   message_t* message_freelist;
   uint32_t message_freelist_count;
};

typedef PAD_TYPE(struct _msg_freelist, PADDING_SIZE) msg_freelist_t;

void insert_on_msg_freelist(message_t* msg, int core_no);
void init_msg_freelist(int nthreads);
#endif

message_t* get_new_msg(int core_no, int sock);

/** In order to support HTTP pipelining, we must have a pending message list. */
message_t* insert_in_pending_list(message_t *msg);
message_t* first_pending(int socket);
message_t* pop_first_pending(int socket);
void free_pending_message_list(message_t *msg);

#endif	/* _SWS_MISC_H */

