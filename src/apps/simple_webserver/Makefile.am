if WANT_GZIP
SUBDIRS = zlib
endif

bin_PROGRAMS=sws
sws_SOURCES = sws.C sws-accept.C sws-misc.C sws-profiling.C sws-cache.C
sws_LDADD = $(top_srcdir)/src/mely/libmely.la

if WANT_GZIP
sws_LDADD += zlib/libz.la
endif

sws_CPPFLAGS = @SWS_CPPFLAGS@
INCLUDES= -I$(top_srcdir)/src/mely/includes

     # lib_LTLIBRARIES =
     # if WANT_LIBFOO
     # lib_LTLIBRARIES += libfoo.la
     # endif
     # if WANT_LIBBAR
     # lib_LTLIBRARIES += libbar.la
     # endif
