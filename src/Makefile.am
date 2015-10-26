
include_HEADERS = ringbuffer.h 

lib_LTLIBRARIES = libringbuffers.la

libringbuffers_la_SOURCES =  ringbuffer.c
libringbuffers_la_LIBADD = 


check_PROGRAMS = test_ringbuffer
test_ringbuffer_SOURCES = test_ringbuffer.c
test_ringbuffer_LDADD = libringbuffers.la

