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
/** Taken from Corey OS **/

#ifndef __PAD_H__
#define __PAD_H__

#define CACHE_LINE_SIZE 64
#define PAD_TYPE(t, ln)                                         \
	union __attribute__((__packed__,  __aligned__(ln))) {   \
	       t val;                                           \
	       char __pad[ln + (sizeof(t) / ln) * ln];          \
	} 

#define PAD(t)								                    \
	union __attribute__((__packed__,  __aligned__(CACHE_LINE_SIZE))) {	            \
	       t val;							                    \
	       char __p[CACHE_LINE_SIZE + (sizeof(t) / CACHE_LINE_SIZE) * CACHE_LINE_SIZE]; \
	} 


#endif /** __PAD_H__ **/
