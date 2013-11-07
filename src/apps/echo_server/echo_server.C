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
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Browse directory
#include <errno.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

#include <sys/stat.h>

#include "mely.h"

struct sockaddr_in server_addr;

int create_accept_socket(int port) {
   int accept_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
   int val = 1;

   if (setsockopt(accept_socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
      PANIC("Cannot use SO_REUSEADDR\n");
   }
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(port);
   server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

   if ((bind(accept_socket, (struct sockaddr*) &server_addr, sizeof(struct sockaddr))) < 0) {
      PANIC("Bind failed\n");
   }
   if (listen(accept_socket, 1000) < 0) {
      PANIC("Listen failed\n");
   }
   return accept_socket;
}

// Handlers
void RegisterAccept();
void Accept(int fd);
void ReadRequest(int fd);
void WriteResponse(int fd, char * buffer);

int main(int argc, char **argv) {
   cpucb(cwrap(RegisterAccept, 0));
   amain();
}

void RegisterAccept() {
   // fdcb CANNOT be called before amain(), so we have to call it in a callback.
   int accept_socket = create_accept_socket(8080);
   printf("Register accept callback for fd %d\n", accept_socket);
   fdcb(accept_socket, selread, cwrap(Accept, accept_socket, 0));
}

void Accept(int fd) {
   printf("Doing Accept\n");
   socklen_t length = sizeof(struct sockaddr);
   int sock = accept4(fd, (struct sockaddr *)&server_addr, &length, SOCK_NONBLOCK);

   if (sock == -1 && errno!= EAGAIN) {
      PRINT_ALERT("Error on accept, errno is : %d (%s)\n",errno,strerror(errno));
      exit(-1);
   }
   else if (sock > -1) {
      fdcb(sock, selread, cwrap(ReadRequest, sock, sock));
   }
}


void ReadRequest(int fd) {
   char* buffer = (char * ) malloc (sizeof(char));
   int rd = read(fd, buffer, sizeof(char));

   if (rd == -1 && errno != ECONNRESET) {
      printf("Error while reading on socket %d\n", fd);
      perror("Reading request");
      exit(-1);
   }
   // Connection was closed or reseted
   else if (rd == -1 || rd == 0) {
      fdcb(fd, selread, NULL);
      fdcb(fd, selwrite, NULL);
      close(fd);
      return;
   }

   fdcb(fd, selwrite, cwrap(WriteResponse,fd, buffer, fd));
}



void WriteResponse(int fd, char * buffer) {
   int wr = write(fd, buffer, sizeof(char));

   if (wr == -1 && (errno != EAGAIN && errno != EBADF)) {
      perror("Error");
      exit(EXIT_FAILURE);
   } else if (wr == -1 && errno == EAGAIN) {
      wr = 0;
   } else {
      free(buffer);
      fdcb_finished(true);
   }

}
