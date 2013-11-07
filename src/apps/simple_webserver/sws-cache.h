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

#ifndef _SWS_CACHE_H
#define	_SWS_CACHE_H

extern map<string,char*> cache;
extern uint64_t total_file_size;


void cache_init(char *dir);
void print_cache();

int prefetch_file(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf);


#endif	/* _SWS_CACHE_H */

