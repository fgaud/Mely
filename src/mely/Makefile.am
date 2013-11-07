#include $(top_srcdir)/Rules.mk
#USE_REFCOUNT=no
lib_LTLIBRARIES = libmely.la

libmely_la_SOURCES = bench_papi.C core.C core_epoll.C itree.C task.common.C task.lbc.C

INCLUDES=-I$(top_srcdir)/src/mely/includes -I$(top_srcdir)/src/mely/.
include_HEADERS = $(top_srcdir)/src/mely/includes/mely.h \
	$(top_srcdir)/src/mely/includes/callback_norefcount.h

# INCLUDES= $(top_srcdir)/src/mely/include/mely.h\
#  $(top_srcdir)/src/mely/include/lock.h\
#  $(top_srcdir)/src/mely/include/runtime_config.h\
#  $(top_srcdir)/src/libdebug_tools/atomic.h\
#  $(top_srcdir)/src/mely/include/refcnt.h\
#  $(top_srcdir)/src/mely/include/runtime_stats.h\
#  $(top_srcdir)/src/mely/include/callback_norefcount.h\
#  $(top_srcdir)/src/mely/include/callback_refcounted.h\
#  $(top_srcdir)/src/mely/include/callback_norefcount_with_allocator.h\
#  $(top_srcdir)/src/mely/include/ws_config.legacy.h\
#  $(top_srcdir)/src/mely/include/ws_config.common.h

