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

#ifndef _SWS_ACCEPT_H
#define	_SWS_ACCEPT_H

void init_accept(int port);

#define NB_MAX_ACCEPTS          8

#if ACCEPT_PER_INTERFACE
 static char * interfaces[] = { "192.168.20.51", "192.168.21.51", "192.168.22.51", "192.168.23.51", "192.168.24.51", "192.168.25.51", "192.168.26.51", "192.168.27.51"};
//static char * interfaces[] = { "192.168.20.50", "192.168.21.50", "192.168.22.50", "192.168.23.50"};
#else
__attribute__((unused)) static char * interfaces[] = {"0.0.0.0"};
#endif
#endif	/* _SWS_ACCEPT_H */


