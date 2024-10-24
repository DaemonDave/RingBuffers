
include_HEADERS = ringbuffer.h 

lib_LTLIBRARIES = libringbuffers.la

libringbuffers_la_SOURCES =  ringbuffer.c
libringbuffers_la_LIBADD = 


check_PROGRAMS = test_ringbuffer
test_ringbuffer_SOURCES = test_ringbuffer.c
test_ringbuffer_LDADD = libringbuffers.la

# ADDED DRE 2024 - for new variable ringbuffers
noinst_PROGRAMS = test-rb

#DRE 2024
# test-rb - tests new ringbuffer modified version with variable slots
test_rb_SOURCES = ringbuffer-varied.c logevt.c

test_rb_LDADD = 
#DRE 2024
