/* zutil.h -- internal interface and configuration of the compression library
 * Copyright (C) 1995-2005 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

/* @(#) $Id$ */

#ifndef ZUTIL_H
#define ZUTIL_H

#define ZLIB_INTERNAL
#include "zlib.h"
#include "async.h"
#undef DEBUG

#ifdef __x86_64__
   typedef unsigned long int	uint64_t;
#  define rdtscll(val) { \
       unsigned int __a,__d;                                        \
       asm volatile("rdtsc" : "=a" (__a), "=d" (__d));              \
       (val) = ((unsigned long)__a) | (((unsigned long)__d)<<32);   \
   }
#else
   typedef unsigned long long int	uint64_t;
#  define rdtscll(val) __asm__ __volatile__("rdtsc" : "=A" (val))
#endif


#  include <string.h>
#  include <stdlib.h>
#  include <stdio.h>
#  include <errno.h>
#ifndef local
#  define local static
#endif
/* compile with -Dlocal if your debugger can't find static symbols */

typedef unsigned char  uch;
typedef uch FAR uchf;
typedef unsigned short ush;
typedef ush FAR ushf;
typedef unsigned long  ulg;

extern const char * const z_errmsg[10]; /* indexed by 2-zlib_error */
/* (size given to avoid silly warnings with Visual C++) */

#define ERR_MSG(err) z_errmsg[Z_NEED_DICT-(err)]

#define ERR_RETURN(strm,err) \
  return (strm->msg = (char*)ERR_MSG(err), (err))
/* To be used only when the state is known to be valid */

        /* common constants */

#ifndef DEF_WBITS
#  define DEF_WBITS MAX_WBITS
#endif
/* default windowBits for decompression. MAX_WBITS is for compression only */

#if MAX_MEM_LEVEL >= 8
#  define DEF_MEM_LEVEL 8
#else
#  define DEF_MEM_LEVEL  MAX_MEM_LEVEL
#endif
/* default memLevel */

#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES    2
/* The three kinds of block type */

#define MIN_MATCH  3
#define MAX_MATCH  258
/* The minimum and maximum match lengths */

#define PRESET_DICT 0x20 /* preset dictionary flag in zlib header */

        /* target dependencies */
#ifndef OS_CODE
#  define OS_CODE  0x03  /* assume Unix */
#endif

#ifndef F_OPEN
#  define F_OPEN(name, mode) fopen((name), (mode))
#endif

         /* functions */

#    define HAVE_VSNPRINTF
#  define HAVE_MEMCPY
#    define zmemcpy memcpy
#    define zmemcmp memcmp
#    define zmemzero(dest, len) memset(dest, 0, len)

#  define Assert(cond,msg)
#  define Trace(x)
#  define Tracev(x)
#  define Tracevv(x)
#  define Tracec(c,x)
#  define Tracecv(c,x)

voidpf zcalloc OF((voidpf opaque, unsigned items, unsigned size));
void   zcfree  OF((voidpf opaque, voidpf ptr));

voidpf zcalloc_stream OF((voidpf opaque, unsigned items, unsigned size));
void   zcfree_stream  OF((voidpf opaque, voidpf ptr));

#if 1
#define ZALLOC(strm, items, size) \
           (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr)  (*((strm)->zfree))((strm)->opaque, (voidpf)(addr))
#define TRY_FREE(s, p) {if (p) ZFREE(s, p);}


#define ZALLOC_STREAM(strm, items, size) \
           zcalloc_stream((strm)->opaque, (items), (size))
#define ZFREE_STREAM(strm, addr)  zcfree_stream((strm)->opaque, (voidpf)(addr))

#else
#define ZALLOC(strm, items, size) \
           calloc(items, size)
#define ZFREE(strm, addr) free(addr)
#define TRY_FREE(s, p) {if (p) ZFREE(s, p);}
#endif


#endif /* ZUTIL_H */
