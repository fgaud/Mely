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


#ifndef _SWS_INCLUDES_H
#define	_SWS_INCLUDES_H

#include "mely.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>

/** File prefetching **/
//#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <map>
#include <string>
using namespace std;

/*
 * Cannot increase FD_SETSIZE on Linux, but we can increase __FD_SETSIZE
 * with glibc 2.2 (or later? remains to be seen). We do this by including
 * bits/types.h which defines __FD_SETSIZE first, then we redefine
 * __FD_SETSIZE. Ofcourse a user program may NEVER include bits/whatever.h
 * directly, so this is a dirty hack!
 */
#include <features.h>
#if (__GLIBC__ > 2) || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 2)
# include <bits/types.h>
# undef __FD_SETSIZE
# define __FD_SETSIZE 65535
#endif




#endif	/* _SWS_INCLUDES_H */

