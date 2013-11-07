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
#include "sws-cache.h"
#include "sws-misc.h"
#include "sws-profiling.h"

map<string,char*> cache;
uint64_t total_file_size = 0;

void cache_init(char *dir) {
/** Prefetching files **/
   fprintf(stderr, "Start prefetching files ...\n");
   int __attribute__((unused)) r = chdir(dir);
   int flags = 0;

   flags |= FTW_DEPTH;
   flags |= FTW_PHYS;

   unsigned long pst = get_time();
   if (nftw(".", prefetch_file, 20, flags) == -1) {
      perror("nftw");
      exit(EXIT_FAILURE);
   }

   printf( "Prefetched %.2Lf kB (%.2Lf MB, %.2Lf GB) in %.2Lf s\n",
            (long double)total_file_size/1024.,
            (long double)total_file_size/(1024.*1024.),
            (long double)total_file_size/(1024.*1024.*1024.),
            (long double) (get_time() - pst) / 1000000.);

   fprintf(stderr, "Prefetching done in in %.2Lf s ...\n",
            (long double) (get_time() - pst) / 1000000.);

   printf("\n********************************\n");
}

void print_cache() {
   std::map<string,char*>::iterator iter = cache.begin();
   while (iter != cache.end()) {
      PRINT_ALERT("Key : - %s Content : xxx\n",iter->first.c_str())
      ;
      iter++;
   }
}


/** Browse the specified directory and map file contents **/
int prefetch_file(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
   if(strstr(fpath, ".svn") || tflag != FTW_F){
      /* To tell nftw() to continue */
      return 0;
   }

   int rd;
   int file_size = sb->st_size;


#if FILE_HANDLER_BUILD_HEADER
   char* content;
   if (suffixTest(fpath, ".html")) {
      content = "text/html";
   }
   else if (suffixTest(fpath, ".png")) {
      content = "image/png";
   }
   else if (suffixTest(fpath, ".jpg") || suffixTest(fpath, ".jpeg")) {
      content = "image/jpeg";
   }
   else if (suffixTest(fpath, ".gif")) {
      content = "image/gif";
   }
   else {
      content = "text/plain";
   }

   char* file_content = (char*) calloc(1, sizeof(char)*(file_size
            +MAX_HEADER_SIZE));


   if(file_content == NULL){
      PRINT_ALERT("Cannot allocate memory for file %s. File size (%d b) is probably too big (or too many files have been prefetched)\n",
               fpath, file_size);
      _exit(EXIT_FAILURE);
   }

   sprintf(file_content, "HTTP/1.1 200 OK\r\nServer: Markov 0.1\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n", content, file_size);

   int hdr_length = strlen(file_content);
   DEBUG("Headers (size is %d) are:\n%s",hdr_length,file_content);

#else
   char* file_content = (char*) calloc(1, sizeof(char)*(file_size));
   int hdr_length = 0;
#endif

   // If the file contains nothing
   // We dont have to read
   if (file_size != 0) {
      DEBUG("Opening file %s (file size is %d)\n",fpath,file_size);
      int fd = open(fpath, O_RDONLY);

      if (fd < 0) {
         perror("Reading");
         _exit(EXIT_FAILURE);
      }

      int file_size_read = 0;

      do {
         DEBUG("Reading on file %s (%d)\n",fpath,fd);

         if ((rd = read(fd, file_content+hdr_length +file_size_read, file_size
                           - file_size_read)) < 0) {
            perror("read");
            _exit(EXIT_FAILURE);
         }

         file_size_read += rd;

      } while ( (file_size - file_size_read) != 0);

      DEBUG("Closing fd %d\n",fd);
      close(fd);
   }

   DEBUG("Read on file %s complete\n",fpath);

#if !USE_SENDFILE
   if(fpath[0] == '.' && fpath[1] == '/'){
      cache[&fpath[2]] = file_content;
      DEBUG("Prefetching file %s (file size is %d)\n",&fpath[2],file_size);
   }
   else{
      cache[fpath] = file_content;
      DEBUG("Prefetching file %s (file size is %d)\n",fpath,file_size);
   }
#else
   free(file_content);
#endif

   total_file_size += file_size;

   /* To tell nftw() to continue */
   return 0;
}

