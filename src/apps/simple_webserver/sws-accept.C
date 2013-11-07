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
#include "sws-accept.h"
#include "sws.h"
#include "sws-misc.h"


static int accept_socket;
static struct sockaddr_in server_addr;

#if ACCEPT_PER_CORE || ACCEPT_PER_INTERFACE
static int *accept_color;
#else
static int accept_color;
#endif //ACCEPT_PER_CORE

void init_accept(int port) {
   int nthreads = async_get_nthreads();
   unsigned int nb_interfaces = (sizeof(interfaces) / sizeof(char*));
   assert(NB_MAX_ACCEPTS >= nb_interfaces);
#if ACCEPT_PER_CORE || ACCEPT_PER_INTERFACE
   accept_color = (int*) calloc(NB_MAX_ACCEPTS, sizeof(int));
#endif //multiple accepts
   
   // This is not really usefull but it's better to have a symetric pattern to understand things...
   assert( nthreads % nb_interfaces == 0  || nb_interfaces % nthreads == 0);

#if ACCEPT_PER_INTERFACE
   int color_accept = 0;
   int nb_proc_per_interface = nthreads / nb_interfaces; // Note : We need that to be == 0 when nthreads<nb_interfaces.
   if(nthreads > (int)nb_interfaces) {
      for (unsigned int j = 0; j< nb_interfaces; j++) {
         //one color for one itf
         accept_color[j] = color_accept + j*nb_proc_per_interface;
      }
   } else {
      for (unsigned int j = 0; j< nb_interfaces; j++) {
         accept_color[j] = j;
      }
   }
#elif ACCEPT_PER_CORE
   for (int j = 0; j< nthreads; j++) {
      accept_color[j] = j;
   }
#else
   /** Hack to disable pinning if legacy mechanism is involved **/
   if(PIN_ACCEPT){
      accept_color = ACCEPT_CLOSE_COLOR_PINNED;
   } else {
      accept_color = ACCEPT_CLOSE_COLOR_NOT_PINNED;
   }
#endif //ACCEPT_PER_CORE

   for (unsigned int i = 0; i < nb_interfaces; i++) {

      accept_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
      DEBUG("new accept_socket : %d\n", accept_socket);
      int val = 1;

      if (setsockopt(accept_socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val))
         < 0) {
         perror("setsockopt: ");
         _exit(EXIT_FAILURE);
      }

      server_addr.sin_family = AF_INET;
      server_addr.sin_port = htons(port);
      assert(inet_addr(interfaces[i])!=(unsigned int)-1);
      server_addr.sin_addr.s_addr = inet_addr(interfaces[i]);//htonl(INADDR_ANY);

      if ((bind(accept_socket, (struct sockaddr*) &server_addr, sizeof(struct sockaddr)))
         < 0) {
         perror("Bind: ");
         exit(-1);
      }
      if (listen(accept_socket, LISTENQ_SIZE) < 0) {
         perror("Listen : ");
         exit(-1);
      }

      printf( "listening on interface %s:%d\n", interfaces[i], port);

#ifdef DEBUG_TASK
      print_socket_option(accept_socket);
#endif


#if ACCEPT_PER_INTERFACE
      //use the newly created socket
      register_accept( accept_color[i] , accept_socket);

#if ACCEPT_PER_CORE
      //use duplicates for other accepts on the same interface
      for (int j=1;j<nb_proc_per_interface;j++) {
         int dup_accept_socket;
         if ((dup_accept_socket = dup(accept_socket)) < 0) {
            perror("Dup : ");
	    exit(-1);
         }
         DEBUG("new dupped(%d) accept_socket : %d\n", accept_socket, dup_accept_socket);
         register_accept( accept_color[i] + j , dup_accept_socket);
      }
#endif //ACCEPT_PER_CORE

#elif ACCEPT_PER_CORE

      for (int j=0;j<nthreads;j++) {
         register_accept(accept_color[j], accept_socket);
      }

#else
      register_accept( accept_color , accept_socket);

#endif //ACCEPT_PER_INTERFACE
   }
}

