/* zutil.c -- target dependent utility functions for the compression library
 * Copyright (C) 1995-2005 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* @(#) $Id$ */

#include "zutil.h"

#ifndef NO_DUMMY_DECL
struct internal_state      {int dummy;}; /* for buggy compilers */
#endif

const char * const z_errmsg[10] = {
"need dictionary",     /* Z_NEED_DICT       2  */
"stream end",          /* Z_STREAM_END      1  */
"",                    /* Z_OK              0  */
"file error",          /* Z_ERRNO         (-1) */
"stream error",        /* Z_STREAM_ERROR  (-2) */
"data error",          /* Z_DATA_ERROR    (-3) */
"insufficient memory", /* Z_MEM_ERROR     (-4) */
"buffer error",        /* Z_BUF_ERROR     (-5) */
"incompatible version",/* Z_VERSION_ERROR (-6) */
""};


const char * ZEXPORT zlibVersion()
{
    return ZLIB_VERSION;
}

uLong ZEXPORT zlibCompileFlags()
{
    uLong flags;

    flags = 0;
    switch (sizeof(uInt)) {
    case 2:     break;
    case 4:     flags += 1;     break;
    case 8:     flags += 2;     break;
    default:    flags += 3;
    }
    switch (sizeof(uLong)) {
    case 2:     break;
    case 4:     flags += 1 << 2;        break;
    case 8:     flags += 2 << 2;        break;
    default:    flags += 3 << 2;
    }
    switch (sizeof(voidpf)) {
    case 2:     break;
    case 4:     flags += 1 << 4;        break;
    case 8:     flags += 2 << 4;        break;
    default:    flags += 3 << 4;
    }
    switch (sizeof(z_off_t)) {
    case 2:     break;
    case 4:     flags += 1 << 6;        break;
    case 8:     flags += 2 << 6;        break;
    default:    flags += 3 << 6;
    }
#ifdef DEBUG
    flags += 1 << 8;
#endif
#if defined(ASMV) || defined(ASMINF)
    flags += 1 << 9;
#endif
#ifdef ZLIB_WINAPI
    flags += 1 << 10;
#endif
#ifdef BUILDFIXED
    flags += 1 << 12;
#endif
#ifdef DYNAMIC_CRC_TABLE
    flags += 1 << 13;
#endif
#ifdef NO_GZCOMPRESS
    flags += 1L << 16;
#endif
#ifdef NO_GZIP
    flags += 1L << 17;
#endif
#ifdef PKZIP_BUG_WORKAROUND
    flags += 1L << 20;
#endif
#ifdef FASTEST
    flags += 1L << 21;
#endif
#ifdef STDC
#  ifdef NO_vsnprintf
        flags += 1L << 25;
#    ifdef HAS_vsprintf_void
        flags += 1L << 26;
#    endif
#  else
#    ifdef HAS_vsnprintf_void
        flags += 1L << 26;
#    endif
#  endif
#else
        flags += 1L << 24;
#  ifdef NO_snprintf
        flags += 1L << 25;
#    ifdef HAS_sprintf_void
        flags += 1L << 26;
#    endif
#  else
#    ifdef HAS_snprintf_void
        flags += 1L << 26;
#    endif
#  endif
#endif
    return flags;
}

#ifdef DEBUG

#  ifndef verbose
#    define verbose 0
#  endif
int z_verbose = verbose;

void z_error (m)
    char *m;
{
    fprintf(stderr, "%s\n", m);
    exit(1);
}
#endif

/* exported to allow conversion of error code to string for compress() and
 * uncompress()
 */
const char * ZEXPORT zError(int err)
{
    return ERR_MSG(err);
}


#ifndef MY_ZCALLOC /* Any system without a special alloc function */
#define MAX_PRE_ALLOC_ZONES  	((ZLIB_MAX_SIMULTANEOUS_COMPRESSIONS+1)*6)
#define PRE_ALLOC_SIZE 		65536
__thread char *pre_alloced_memory = 0;
static char* init_alloced_list(int nb_items, int size) {
     char *ret;
     ret = (char*)malloc(nb_items*size);
     if(!ret)
        return NULL;
	
     for(int i = 0; i<nb_items; i++) {
	void **addr = (void **)(ret+i*size);
	*addr = (ret + (i+1)*size);
     }
     void **end_addr = (void**) (ret + (nb_items-1)*size);
     *end_addr = NULL;

     return ret;
}

voidpf zcalloc (voidpf opaque, unsigned items, unsigned size)
{
   assert(size <= PRE_ALLOC_SIZE);
   if(!pre_alloced_memory) {
	pre_alloced_memory = init_alloced_list(MAX_PRE_ALLOC_ZONES, PRE_ALLOC_SIZE);
	if(!pre_alloced_memory)
	      assert(0 && "No more memory !");
   } 
   void **new_addr = (void **)pre_alloced_memory;
   if(*new_addr == NULL) {
	fprintf(stderr, "Warning : no more memory in zcalloc !\n");
	return NULL; 
   }
   
   void *ret = pre_alloced_memory;
   pre_alloced_memory = (char*)*(void**)new_addr;
   return ret;
}

void  zcfree (voidpf opaque, voidpf ptr)
{ 
    void **addr = (void**) ptr;
    *addr= pre_alloced_memory;
    pre_alloced_memory = (char*)ptr;
}

voidpf zcalloc_stream (voidpf opaque, unsigned items, unsigned size)
{
   return zcalloc(opaque, items, size);
}

void  zcfree_stream (voidpf opaque, voidpf ptr)
{
   zcfree(opaque, ptr);
}
#endif /* MY_ZCALLOC */
