/*
 * SPDX-License-Identifier: GPL-2.0
 * Copyright (C) 2005-2022 Dahetral Systems
 * Author: David Turvene (dturvene@gmail.com)
 *
 * Copy the ringbuffer pattern in ringbuffer.c for an event logger in
 * the ringbuffer.c example.  evt_que calls are placed strategically in
 * the test code while running.  THen print_evts is called to display all
 * events.
 */

#ifndef _LOGEVT_H
#define _LOGEVT_H

#include <stdint.h>     /* uint32_t, etc. */
#include <time.h>       /* timespec */
#include <pthread.h>    /* pthread_mutex */
#include <errno.h>		// errno
// time stamp
static struct timespec ts, te, delta;

inline static void ts_start()
{
    clock_gettime(CLOCK_MONOTONIC, &ts);
}

inline static void ts_end()
{
    clock_gettime(CLOCK_MONOTONIC, &te);
}

inline static char *ts_delta()
{
    static char str[80];
    /* calculate timer offset */
    if ((te.tv_nsec - ts.tv_nsec) < 0)
    {
        delta.tv_sec = te.tv_sec - ts.tv_sec - 1;
        delta.tv_nsec = 1e9 + te.tv_nsec - ts.tv_nsec;
    }
    else
    {
        delta.tv_sec = te.tv_sec - ts.tv_sec;
        delta.tv_nsec = te.tv_nsec - ts.tv_nsec;
    }
    sprintf(str, "delta=%ld.%09ld", delta.tv_sec, delta.tv_nsec);
    return (str);
}


/** \enum evtid_t a simple event list
 * David T.'s event types to log (modified) - Explained by DRE 2024
 */
typedef enum evtid
{
    EVT_ENQ = 1,			// EVENT: Enqueue happened
    EVT_DEQ = 2,			// EVENT Dequeue happened
    EVT_DEQ_IDLE = 3,		// Q is idle - EVENT
    EVT_MAX_QUEUE = 4,  	// A maximum Q event 
    EVT_END = 5,			// EVENT - ENDING Q OPERATIONS
} evtid_t;

/* externally visible prototypes */
void evt_enq(evtid_t id, uint32_t val);
void print_evts(void);
void fprint_evts(void);

#endif /* _LOGEVT_H */


