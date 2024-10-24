/*! \file ringbuffer-varied.c
 *
 * SPDX-License-Identifier: GPL-2.0
 * Copyright (C) 2005-2022 Dahetral Systems
 * Author: David Turvene (dturvene@gmail.com)
 *
 * Use a simple static queue to implement a ringbuffer.
 */
/** \brief This is a modified version fo David T's rinbuffer which used  static uint32_t as the data store
 * per ring entry only this new version is expanded for a varied size data payload.
 * The idea is that ringbuffers for various queues will need varied size for various data packets as
 * part of a bottom up  structured-knowledge manifold system.
 *
 * The ringbuffer is contained in other C code with custom sized data struct for the custom usage.
 * This is the test file that will embody the local test version.
 *
 * This version uses a local static store for testing but the final version will be agnostic.
 *
 * */

#include <stdlib.h>     /* atoi, malloc, strtol, strtoll, strtoul, exit, EXIT_FAILURE */
#include <stdio.h>      /* char I/O, perror */
#include <unistd.h>     /* getpid, usleep,  common typdefs, e.g. ssize_t, includes getopt.h */
#include <string.h>     /* strlen, strsignal,, memset, bzero_explicit */
#include <stdint.h>     /* uint32_t, etc. */
#include <stdlib.h>     /* exit */
#include <pthread.h>    /* pthread_mutex, pthread_barrier */
#include <stdatomic.h>  /* atomic_ operations */
#include <stdbool.h>    /* boolean declaration and types: true, false */
#include "config.h"     /* meson generated configuration file */
#include "logevt.h"     /* event logging */

/* commandline args */
char *cmd_arguments = "\n"					\
                      " -t id: test id to run\n"				\
                      " -m: use mutex (default spinlock)\n"			\
                      " -c cnt: cnt events to enq (default 10000)\n"		\
                      " -l: event logging (default disabled)\n"		\
                      " -h: this help\n"					\
                      ;

static uint32_t debug_flag = 0;
static uint32_t testid = 0;
static uint32_t mutex_flag = 0;
static uint32_t cnt_events = 10000;
static bool log_flag = false;
//! \var g_max_entries is the global max number of entries - it can be checked on during operations or at the end
static size_t g_max_entries = 0;
#define ARRAY_SIZE(arr)  (sizeof(arr)/sizeof(arr[0]))
#define INVALID_EL (0xffffffff)

///
#define VERSION_STR 0.1


/*
 * Producer sends this element immediately before exitting
 * Consumer deq this element and exits
 * This must be large!
 */
#define END_EL (0xdeadbeef)

/// OLD CODE
///typedef uint32_t buf_t;
/** \struct buf_t \brief the void pointer placeholder
 *  The new varied size ringbuffer entry - which makes it more
 * relevant and useful for wider use - requires to exploit the
 * magical void * pointer as a universal placeholder for the memory that is
 * aimed at for replacement or recovery. David T.'s code enqueues an entry
 * and that means the data replaces where the enqueue pointer is aiming at.
 *
 * His code, and if you think about it, was for one integer. And he cheated by
 * calling everything a buf_t in the struct below and that made his logical
 * comparisons and pointer math was limited to one kind of struct type. His confused
 * allocating and logical comparisons made my life harder to expand his work.
 *
 * In the new version the "buffer" buf_t is a void pointer that acts like a placeholder
 * to aim at independently allocated buffers of BUFFER_SIZE size_t. You can now send a data
 * element of up to BUFFER_SIZE into a queue position and out of a queue position because
 * it accomodates any up to that max. Of course we are counting in size_t from 0.
 *
 * When this code is separated into a working #include that encapsulates all needed functionality
 * then all you need to do is redefine or define unique payload_t as the real contents
 * for each buffer. The payload_t can be up to BUFFER_SIZE wide.
 *
 * The beauty is that it will transfer by memcpy the entire buffer but when you type cast it to your
 * own format it will still have those data at proper offsets. As long as you keep the
 * typecast correct and use the same pointer referencing data will appear in the same format.
 *
 * */
/// \struct void * poiunter allows variable struct type casting with a varied data struct
typedef void * buf_t; // every buffer slot is by default an address.
/**
 * die - helper function to stop immediately
 * @msg - an informational string to identify where program failed
 *
 * See man:perror
 */
inline static void die(const char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

/**
 * nap - small sleep
 * @ms: number of millisecs to sleep
 *
 * useful for very small delays, causes thread switch.
 * See man:nanosleep for signal problems, maybe convert to
 */
inline static void nap(uint32_t ms)
{
    struct timespec t;
    t.tv_sec = (ms/1000);
    t.tv_nsec = (ms%1000)*1e6;
    nanosleep(&t, NULL);
}

/** \deprecated buf_debug is David T.'s code for int only buffers
 * buf_debug - helper function to display a queue bufs array element
 * @buf - pointer to bufs element
 */
inline static void buf_debug(const buf_t *buf)
{
    printf("%d ", *buf);
}

/* Fixed size of the array used for queuing */
// the original for single value buffer slots
///#define QDEPTH 4096
//! \def QDEPTH is the original array size; here it's  smaller queue number but larger struct sizes
#define QDEPTH 256
/// \note future expansion to variable variable buffer depth and buffer slot size is a few changes away...
///
/// \struct payload_t is the generic max size into every buffer store entry that must be defined with every ringbuffer implementation
///
/**
 \typedef payload_t can be made anything up to BUFFER_SIZE and the ringbuffer will work
 passing into and out of the queues.

*/
typedef struct payload
{
    float a;
    float b;
    float c;
    float d;
    float e;
    float f;   /// easy to understand and look if data was overwritten
    float g;
    float h;
    float i;
    float j;
} payload_t;

/// END OF FILE DEFINES


/// \def BUFFER_SIZE is the variable sized data payload buffer slot
#define BUFFER_SIZE	sizeof(payload_t)
/// BUFFER_SIZE becomes the maximum store per ringbuffer entry
// you can send and use smaller but it's restricted and will crash
// if you exceed that level
void payload_init ( payload_t * out )
{
    //a = b = c = d = e = f = g = h = i = j = 0;
    bzero(out, sizeof(payload_t));
}
void payload_set1 ( payload_t * out, float a )
{
    //a = b = c = d = e = f = g = h = i = j = 0;
    out->a = a;
}
void payload_set2 ( payload_t * out, float a, float b  )
{
    //a = b = c = d = e = f = g = h = i = j = 0;
    out->a = a;
    out->b = b;
}
void payload_set3 ( payload_t * out, float a, float b, float c  )
{
    //a = b = c = d = e = f = g = h = i = j = 0;
    out->a = a;
    out->b = b;
    out->c = c;
}
void  payload_set4 ( payload_t * out, float a, float b, float c, float d  )
{
    //a = b = c = d = e = f = g = h = i = j = 0;
    out->a = a;
    out->b = b;
    out->c = c;
    out->d = d;
}
void  payload_set5( payload_t * out, float a, float b, float c, float d, float e  )
{
    //a = b = c = d = e = f = g = h = i = j = 0;
    out->a = a;
    out->b = b;
    out->c = c;
    out->d = d;
    out->e = e;
}
void  payload_set6( payload_t * out, float a, float b, float c, float d, float e, float f  )
{
    //a = b = c = d = e = f = g = h = i = j = 0;
    out->a = a;
    out->b = b;
    out->c = c;
    out->d = d;
    out->e = e;
    out->f = f;
}
void  payload_set7( payload_t * out, float a, float b, float c, float d, float e, float f, float g  )
{
    //a = b = c = d = e = f = g = h = i = j = 0;
    out->a = a;
    out->b = b;
    out->c = c;
    out->d = d;
    out->e = e;
    out->f = f;
    out->g = g;
}
void  payload_set8( payload_t * out, float a, float b, float c, float d, float e, float f, float g, float h  )
{
    //a = b = c = d = e = f = g = h = i = j = 0;
    out->a = a;
    out->b = b;
    out->c = c;
    out->d = d;
    out->e = e;
    out->f = f;
    out->g = g;
    out->h = h;
}
void  payload_printf( payload_t * out )
{
    fprintf(stdout, "[%2f][%2f][%2f][%2f][%2f][%2f][%2f][%2f][%2f][%2f]\n", out->a, out->b, out->c, out->d, out->e, out->f, out->g, out->h, out->i, out->j  );
    //a = b = c = d = e = f = g = h = i = j = 0;
}
// heap store data observer
char * entry = NULL;
#define CHAR_STORE 256 // a float is 4 but expanded form expect 10 bytes each so overbudget most times
char * payload_sprintf( payload_t * out )
{
    // first time allocator
    if (entry = NULL)
    {
        entry = malloc(CHAR_STORE);
        // zero buffer first time
        bzero(entry, CHAR_STORE);
    }
    //int snprintf(char str[restrict .size], size_t size, const char *restrict format, ...);
    snprintf(entry, CHAR_STORE, "[%f][%f][%f][%f][%f][%f][%f][%f][%f][%f]", 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9, 100.0  );
    //a = b = c = d = e = f = g = h = i = j = 0;
    return entry;
}
/// \def PAYLOADCOPY for a simple one to one copy
#define PAYLOADCOPY(dst,src) memcpy(dst,src, sizeof(payload_t))
/**
 * struct sq - simple queue
 * @bufs: fix array of buffers
 * @enq: pointer to the bufs element to fill for the newest value,
 *       the element will either be invalid or, if valid, the
 *       oldest filled.
 * @deq: pointer to the bufs element to drain next,
 *       always the oldest element.
 * @count: the number of bufs elements containing data ///!!! WRONG ITS A POINTER ALSO but in address form!!!
 * @first: pointer to the first bufs element
 * @last: pointer to the last bufs element
 * @max: the number of total bufs in the array.
 * @cb: debug callback
 *
 * This is the main simple queue structure.
 * DRE 2024
 *  It's now more than an array of ints and fancy names.
 * simple buffer is an array of buffers with identical pointers and a real counter.
 *
 * The easy way to get at a location, a single pointer in an array of pointers is
 *
 * sq->first + sq->count which then moves to sq->bufs[count]
 * sq->first and sq->last are maintained aiming at the buffer first and last locations for comparison.
 *
 * The current to encode sq->enq can be refound by
 * sq->first + sq->count
 * BUT
 * sq->deq is relative generally as sq->first + ?
 * It now allocates for separate data for each point
 *
 * It is instantiated once for each queue.  The bufs array fills and empties going higher. If the
 * bufs array fills to the last element then it loops back to the first element
 * and starts to overwrite the oldest elements.
 *
 * This structure does not include concurrency mechanisms.  See mutexes and spinlocks
 */
typedef struct sq
{
    //! \var bufs is the modified longer than one element memallocated stores as array destinations
    buf_t bufs[QDEPTH];
    //!
    buf_t * enq; // relative ring buffer write into placeholder pointer
    //!
    buf_t * deq; // relative ring buffer read from placeholder pointer
    //! \var first is supposed to be the enqueuing
    // absolute pointer to first target
    buf_t * first;
    // absolute pointer to last target - he got lazy because types were the same and he mixes counts and pointers
    buf_t * last;
    //! \var max is an absolute for the total possible
    buf_t * max;
    //
    /// \note pointers are volatie and he accesses them unwittingly in his code so I put vars below to avoid overwrite
    /// \note knowing that
    //
    //! \var placeholder is a protection against accidental overwrites above,,,
    buf_t * placeholder;  // gives an overwrite buffer
    //! \var count modified to actual count - he got lazy and that's why portability wasn't there...
    size_t count;	// an absolute not a reference - but what he didn't use is it can be added as a simple offset if needed
    //! \var buffer_width is an unsure variable  added while trying to get the others to work
    size_t buffer_width; // in sizeof value
    //! \note I have left callback and it can be replaced
    //! \var cb is the local callback that David T. used for debugging it must have a function that printfs out the actual contents of the buffer.
    void (*cb)(const buf_t *);
} sq_t;
/**
 * rb_test - test ring buffer using sq_t
 *
 * Initial values can be defined at compile time but can also be dynamically set
 * before the first enqueue operation.
 *
 * NOTE: better to create on local stack if ringbuffer is only accessible to a
 * function/subfunction.
 *
 * \note basic values initialized here get overwritten... DRE 2024
 */
static sq_t rb_test =
{
    .bufs[0]=INVALID_EL, .bufs[1]=INVALID_EL, .bufs[3]=INVALID_EL,
    .enq = rb_test.bufs,
    .deq = rb_test.bufs,
    .count = 0,

    .first = rb_test.bufs,
    .last = &rb_test.bufs[ARRAY_SIZE(rb_test.bufs)-1],// declared as last entry
    .max = ARRAY_SIZE(rb_test.bufs),
    .cb = buf_debug, // never used
};
/// \fn Init_sq initializes every bufs buffer slot to the maximum size BUFFER_SIZE
void Init_sq ( sq_t * rb )
{
    int i = 0;// raw single row
    // resize for variable width
    rb->buffer_width = BUFFER_SIZE;

    // iterate and allocate
    for (i = 0; i < QDEPTH; i++)
    {
        // clear and allocate
        rb->bufs[i] = calloc( BUFFER_SIZE,0 ); /// just allocate
    }
    fprintf(stdout, "allocated %d buffer slots...\n", i);
    // reset actual count to zero
    rb->count = 0;

    // set the new callback
    rb->cb = payload_printf;
    // nothing else changes
}
/// \fn Destroy_sq initializes every bufs buffer slot the the maximum size
void Destroy_sq ( sq_t * rb )
{
    int i = 0;// raw single row
    rb->buffer_width = 0;
    for (i = 0; i < QDEPTH; i++)
    {
        free(&rb->bufs[i]); /// de allocate
    }
    // there is nothing left to find
    rb->max = 0; // one less than actual number as array starts from 0
    rb->first = 0; // one less than actual number as array starts from 0
    rb->last = 0; // one less than actual number as array starts from 0
    rb->count = 0; // one less than actual number as array starts from 0
}
/**
 * q_print: display the all bufs in the queue
 * @label: an informational string used to identify the queue state
 * @sqp: the simple queue context structure
 *
 * Start with first bufs element assigned to a tmp variable
 * print the element value
 * increment tmp until last bufs element
 */
void q_print(const char* label, const sq_t* sqp)
{
    const buf_t *tmp = sqp->first;
    printf("%s count=%d bufs=%p enq=%p deq=%p\n", label, sqp->count, sqp->bufs, sqp->enq, sqp->deq);
    do
    {
        sqp->cb(tmp);// now works with latest modifications...
    }
    while (tmp++ != sqp->last);
    printf("\n");
}

/**
 * experiment with pthread barrier construct. It does not seem to be needed
 */
//#define BARRIER
#ifdef BARRIER
/**
 * barrier - pthread barrier to start pthreads at roughly the same time
 *
 * This is created and used when the BARRIER define is set, otherwise all
 * uses are removed from code.
 */
pthread_barrier_t barrier;
#endif
static pthread_mutex_t sq_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * lock_t - type for the spinlock bit array
 * C11 spec says to use an atomic for atomic lock value
 */
typedef atomic_uint lock_t;

/**
 * lockholder - bit array marking the thread holding the spinlock
 *
 * This will be 0 if no thread holds lock, otherwise ONE of the defined lock
 * bits: LOCK_C for consumer and LOCK_P for producer.  Using a bitarray allows
 * for better metrics in the lock function
 */
lock_t lockholder = 0;
#define LOCK_C 0x01
#define LOCK_P 0x02

/*
 * lock_held_c, lock_held_p: metrics when trying to lock, indicating thread
 * currently holding the lock. Each is a counter incremented in the lock
 * function when lock acquire fails.
 */
static atomic_int lock_held_c, lock_held_p;

/**
 * lock - try to acquire lock atomically, spin until achieved
 * @bitarrayp: pointer to lock bitarray
 * @desired: bit value used to update lock
 *
 * Loop, testing for all lock bits cleared.  The current value is returned in
 * expected, which can then be used to updates metrics about which thread
 * currently holds the lock.
 * When the lock is cleared, atomically update it to the desired holder.
 *
 * Uses the gcc 7.5+ implementation of atomic_compare_exchange_weak
 * defined in
 * https://en.cppreference.com/w/c/atomic/atomic_compare_exchange
 *
 */
void lock(lock_t *bitarrayp, uint32_t desired)
{
    uint32_t expected = 0; /* lock is not held */
    uint32_t hung_lock = 0;
    /* When the lock is released (see release below) then
     * *bitarrayp is expected to be 0. If it is then *bitarrayp
     * is updated with the desired value - which will be either LOCK_P
     * or LOCK_C.
     * If the comparison fails (meaning the lock is still held), then
     * the current value of *bitarrayp is copied to expected.
     * The expected variable is compared with the two lock flags and an
     * the consumer or producer lock counter is incremented to record that
     * the lock is held.
     */
    do
    {
        if (expected & LOCK_P) lock_held_p++;
        if (expected & LOCK_C) lock_held_c++;
        expected = 0;
        /* occasionally see test timeouts, could be a deadlock
         * so put some code in that kills the task if lock is held too long
         */
        if (++hung_lock > 2000)
        {
            fprintf(stderr, "%s: lock may be hung at %u\n", __FUNCTION__, hung_lock);
            if (hung_lock > 4000)
                die("probably deadlock");
        }
#if 1
    }
    while (!atomic_compare_exchange_weak(bitarrayp, &expected, desired));
#else
        /* Try different memory models from
           /usr/lib/gcc/x86_64-linux-gnu/7/include/stdatomic.h
           memory model: SUC, FAIL
           __ATOMIC_SEQ_CST
           __ATOMIC_RELEASE
           __ATOMIC_ACQUIRE
           __ATOMIC_ACQ_REL: invalid for call
         */
    }
    while (!atomic_compare_exchange_weak_explicit(bitarrayp,
            &expected,
            desired,
            __ATOMIC_ACQUIRE,
            __ATOMIC_ACQUIRE
                                                 ));
#endif
}

/**
 * release - clear the bitarray, making the lock available to be acquired.
 * @bitarrayp: pointer to lock bitarray
 *
 * The current thread will have its bit set in the lock variable and be
 * the holder of the lock.  This call effectively releases the lock.
 * Note that *bitarrayp is of type lock_t, which is a C11 atomic.
 */
void release(lock_t *bitarrayp)
{
    *bitarrayp = 0;
}

/**
 * q_enq: enqueue a new value into the oldest ringbuffer element
 * @sqp: the simple queue context structure
 * @val: value to enter into current bufs element
 *
 * Logic:
 * - update element value and wrap or increment enq pointer
 * - if all bufs are being used then move the deq pointer to the
 *   current oldest (one more than the newest!),
 *   if bufs still available then increment buf count
 */
void q_enq(sq_t* sqp, buf_t val)
{
    /* command line argument to determine if mutex lock or spinlock */
    if (mutex_flag)
    {
        pthread_mutex_lock(&sq_mutex);
    }
    else
    {
        lock(&lockholder, LOCK_P);
    }
    /// if (debug_flag)
    //  printf("q_enq enter count=%d val=%s enq=%s deq=%s sqp->last=%p sqp->first=%p \n", sqp->count, payload_sprintf((payload_t *)&val), payload_sprintf((payload_t *)sqp->enq), payload_sprintf((payload_t *)sqp->deq), sqp->last, sqp->first);
    /// OLD CODE
    /// enqueue value into buf
    ///*(sqp->enq) = val;
    //
    /// NEW CODE - copy IN FROM external variable
    //
    memcpy( &sqp->enq, val, BUFFER_SIZE );/// NEW COPY INTO START OF STRUCT NO MATTER SIZE
    // for now a constant buffer size which would be the maximal
    /// ITERATOR FOR ENQUEUE OPERATION
    /** compare to last buf first and then increment to next buf
     * if match last then set to first
     * the enq pointer after last is never used
     */
    if (sqp->enq++ == sqp->last)
        sqp->enq = sqp->first; // roll around buffer array to lowest

    // increment regardless
    sqp->count++;

    /// GLOBAL Update for maximum entries
    if ( g_max_entries < sqp->count )
    {
        g_max_entries = sqp->count;
        if (log_flag)
        {
            /// log event before releasing lock = note largest number of queue entries at once
            evt_enq(EVT_MAX_QUEUE, g_max_entries);
        }

    }

    /* When the the array is full, q_enq will be overwriting the oldest buffer
     * so after enq updates the oldest buffer with the newest datum,
     * move the deq pointer to the NEXT oldest.
     * If the array is not full, then deq will still point to the oldest so just
     * increment the buffer count.
     */
    /// again because he's lazy ("efficient" meaning limited usefulness) he mixes counts and pointers
    if (sqp->first + sqp->count == sqp->max) // OLD CODE  if (sqp->count == sqp->max)
    {
        if (sqp->deq == sqp->last)
            sqp->deq = sqp->first + 1; // aim dequeue at the lowest absolute pointer plus one
        else
            sqp->deq = sqp->enq + 1; // move to just after enqueue
    }
    else // majority case - just iterate the count on the ring buffer for most
    {
        printf("q_enq exit count=%u \n", sqp->count );

    }
    //if (debug_flag)
    //  printf("q_enq exit count=%d enq=%s deq=%s sqp->last=%p sqp->first=%p \n", sqp->count, payload_sprintf((payload_t *)sqp->enq), payload_sprintf((payload_t *)sqp->deq), sqp->last, sqp->first);
    if (log_flag)
    {
        /* log event before releasing lock.  This makes the critical section
         * longer but logs the event accurately; outside of the critical
         * section will result in out-of-sequence events being logged.
         */
        evt_enq(EVT_ENQ, val);
    }
    /* command line argument to determine if mutex lock or spinlock */
    if (mutex_flag)
    {
        pthread_mutex_unlock(&sq_mutex);
    }
    else
    {
        release(&lockholder);
    }
}
/**
 * q_deq: dequeue the oldest ringbuffer element
 * @sqp: the simple queue context structure
 * @valp: return the value in the current deq element
 *
 * Logic:
 * - If no valid elements, return -1
 * - get value from bufs element
 * - mark queue element as invalid (for debugging) and decrement counter
 * - if last element then wrap to first, otherwise move to next element
 *
 * Return:
 *   0 for success, negative otherwise
 */
int q_deq(sq_t* sqp, buf_t* valp)
{
    /* if no valid entries, return error */
    if (sqp->count == 0)
    {
        fprintf(stdout, " WARNING no count in sq on dequeue!!!\n");
        return (-1);
    }
    /* command line argument to determine if mutex lock or spinlock */
    if (mutex_flag)
    {
        pthread_mutex_lock(&sq_mutex);
    }
    else
    {
        lock(&lockholder, LOCK_C);
    }
    //if (debug_flag) // raw output for now...
    //  printf("q_deq enter count=%d val=%s enq=%s deq=%s sqp->last=%p sqp->first=%p \n", sqp->count, payload_sprintf((payload_t *)valp), payload_sprintf((payload_t *)sqp->enq), payload_sprintf((payload_t *)sqp->deq), sqp->last, sqp->first);
    //
    ///
    // CRITICAL CODE - THE ACTUAL PURPOSE OF DEQUEUE FCN
    ///
    /// ORIGINAL CODE
    /* fill value with bufs entry data */
    ///*valp = *(sqp->deq);  // COPY OF AN NUMBER IS LIMIT FOR ORIGINAL THIS EFFORT
    //
    /// NEW CODE - copy out to external variable
    //
/// void *memcpy(void dest[restrict .n], const void src[restrict .n], size_t n);
    memcpy( *valp, &sqp->deq, BUFFER_SIZE );// for now a constant buffer size which would be the maximal
    /* set bufs element to invalid for debugging */
    //if (debug_flag)
    ///  *(sqp->deq) = INVALID_EL; /// OLD CODE - just a simple one address
    //  printf("q_deq exit count=%d val=%s enq=%s deq=%s sqp->last=%p sqp->first=%p \n", sqp->count, payload_sprintf((payload_t *)valp), payload_sprintf((payload_t *)sqp->enq), payload_sprintf((payload_t *)sqp->deq), sqp->last, sqp->first);

    ///
    // END CRITICAL CODE  - REST IS HOUSKEEPING
    //
    /* dec count because bufs element can be reused now */
    sqp->count--; // decrease valid available elements - because the total amount "available" never changes
    // This is David T's buffer decrement / dequeue interpretation
    //
    /* compare to last buf first and then increment to next buf
     * if match last then set to first
     * the enq pointer after last is never used
     */
    // increments queue buffer up towards sqp->deq
    if (sqp->deq++ == sqp->last)/// wraparound to the lowest buffer
        sqp->deq = sqp->first;
    if (log_flag)
    {
        /* log event before releasing lock.  This makes the critical section
         * longer but logs the event accurately; outside of the critical
         * section will result in out-of-sequence events being logged.
         */
        evt_enq(EVT_DEQ, *valp);
    }
    //if (debug_flag)
    //  printf("q_deq exit count=%d ep=%p val=%s dp=%p val=%s\n", sqp->count, sqp->enq, *(sqp->enq), sqp->deq, *(sqp->deq));
    fprintf(stdout, "q_deq exit sqp->count=%u\n", sqp->count);

    /* command line argument to determine if mutex lock or spinlock */
    if (mutex_flag)
    {
        pthread_mutex_unlock(&sq_mutex);
    }
    else
    {
        release(&lockholder);
    }
    return (0);
}

/**
 * q_producer: pthread to call q_enq using a monotonically increasing value
 * @arg: pthread arguments passed from pthread_create (not used)
 *
 * This pthread only enqueues values to the ringbuffer.  The values increase to
 * represent a chronologically order.  When the thread exits, it enqueus an
 * END_EL value for the consumer to recognize there are no more enqueued values.
 *
 * NOTES:
 * I experimented with allowing the thread to relax (short sleep) after
 * enq but that just seemed to unnecessarily slow down operations.
 * I experimented with a pthread barrier wait so producer/consumer start at roughly
 * the same time but this appears to be unnecesasry.
 *
 * Return: NULL
 */
void*
q_producer_ut(void *arg)
{
    int base_idx = 0;  /* a unique number to differentiate q_enq entries */
    void (*fnenq)(sq_t*, buf_t) = q_enq;
#ifdef BARRIER
    pthread_barrier_wait(&barrier);
#endif
    fprintf(stderr, "%s: several small enq tests\n", __FUNCTION__);
    /* test enq works before wrapping */
    for (int i=1; i<3; i++)
        fnenq(&rb_test, base_idx+i);
    /* test one loop around the ringbuffer works */
    base_idx += 100;
    for (int i=1; i<QDEPTH; i++)
        fnenq(&rb_test, base_idx+i);
    fnenq(&rb_test, END_EL);
    return (NULL);
}

/*
 * q_producer_empty - a minimal test of the producer/consumer
 *
 * See q_producer for doc.
 */
void *q_producer_empty(void *arg)
{
/// NEW CODE
    buf_t val = malloc( BUFFER_SIZE ); /// just allocate
    memcpy(&val, END_EL, sizeof(END_EL));
#ifdef BARRIER
    pthread_barrier_wait(&barrier);
#endif
    fprintf(stderr, "%s: a single q_enq\n", __FUNCTION__);
    /* single enq to start consumer */
    q_enq(&rb_test, &val);
    /// NEW CODE
    memcpy(&val, END_EL, sizeof(END_EL));

    /* end of enqueue */
    q_enq(&rb_test, END_EL);
    return (NULL);
}


/**
 * q_producer_stress2 - a relatively short stress test of the ringbuffer
 *
 * See q_producer for doc.
 */
void
*q_producer_stress2(void *arg)
{
    int base_idx = 0;  /* a unique number to differentiate q_enq entries */
#ifdef BARRIER
    pthread_barrier_wait(&barrier);
#endif
    fprintf(stderr, "%s: a statically sized stress test\n", __FUNCTION__);
    /* stress enq loop */
    for (int j=0; j<20; j++)
    {
        for (int i=1; i<QDEPTH; i++)
            q_enq(&rb_test, base_idx+i);
        base_idx += 100;
    }
    /* make inner loop bigger */
    for (int j=0; j<20; j++)
    {
        for (int i=1; i<128; i++)
            q_enq(&rb_test, base_idx+i);
        base_idx += 100;
    }
    /* end of enqueue */
    q_enq(&rb_test, END_EL);
    return (NULL);
}

/*
 * q_producer_stress3 - a long stress test of the ringbuffer
 *
 * See q_producer for doc.
 */
void
*q_producer_stress3(void *arg)
{
    int base_idx = 0;  /* a unique number to differentiate q_enq entries */
    fprintf(stderr, "%s: a dynamically sized stress test sending %u events\n",
            __FUNCTION__, cnt_events);
    /* stress enq loop */
    for (int i=0; i<cnt_events; i++)
    {
        q_enq(&rb_test, base_idx+i);
    }
    /* end of enqueue */
    q_enq(&rb_test, END_EL);
    return (NULL);
}

/**
 * q_consumer: pthread to call q_deq
 * @arg: pthread arguments passed from pthread_create (not used)
 *
 * This pthread loops until the END_EL value is received. It trys to dequeue a
 * value. If one is available the function logs it, otherwise it increases and
 * idle counter. After a value is dequeued it will also log the idle counter.
 *
 * When the consumer thread starts before the producer it busy-waits
 * until the first value is written by the producer.
 *
 * NOTE: I experimented with allowing the thread to relax (short sleep) after
 * deq but that just seemed to unnecessarily slow down operations.
 * NOTES:
 * I experimented with allowing the thread to relax (short sleep) after
 * enq but that just seemed to unnecessarily slow down operations.
 * I experimented with a pthread barrier wait so producer/consumer start at roughly
 * the same time but this appears to be unnecesasry.
 *
 */
void* q_consumer(void *arg)
{
    int done = 0;
    buf_t val = malloc( BUFFER_SIZE ); /// just allocate
    int idlecnt = 0;
    int (*fndeq)(sq_t*, buf_t*) = q_deq; /* use a fn pointer for easy management */
#ifdef BARRIER
    pthread_barrier_wait(&barrier);
#endif
    if (debug_flag)
        printf("%s: starting\n", __FUNCTION__);
    /* race condition busy-wait until producer enqueues something
     */
    while (fndeq(&rb_test, &val))
    {
        idlecnt++;
    }
    if (log_flag)
    {
        /* log how many idle loops before producer starts writing to queue */
        evt_enq(EVT_DEQ_IDLE, idlecnt);
    }
    idlecnt = 0;
    /* loop until the producer sends the END element */
    while (!done)
    {
        if (0 == fndeq(&rb_test, &val))
        {
            if (val == END_EL)
                done = 1;
            else
            {
                if (log_flag)
                {
                    /* log how many idle loops before a new element
                     * is written by producer
                     */
                    if (idlecnt > 0)
                        evt_enq(EVT_DEQ_IDLE, idlecnt);
                }
                idlecnt = 0;
            }
        }
        else
        {
            idlecnt++;
        }
    }
    if (debug_flag)
        fprintf(stderr, "%s: exiting\n", __FUNCTION__);
    return (NULL);
}

int main(int argc, char *argv[])
{
    int opt;
    pthread_t producer, consumer;
    void* (*fn_producer)(void *arg);
    void* (*fn_consumer)(void *arg);

    buf_t tbuffer;
    tbuffer = (buf_t)malloc(sizeof(payload_t)); // a generic payload type is the same as the specific buffer
    payload_t data1;
    payload_t data2;

    //! initialize the payload 1 & 2
    // 1 is the enqueue entry
    payload_init( &data1 );
    // 2 is the dequeue entry - as per David T's terminology
    payload_init( &data2 );
    // set 3 in data1
    payload_set3 ( &data1, 1.4, 10.5, -12.6  );
    // set 3 in data1
    payload_set5 ( &data2, -10.1, -88.4, 100, 200, 600  );
    payload_set5 ( &data2, -10.1, -88.4, 100, 200, 600  );
    payload_set5 ( tbuffer, -99.0, -99.4, -100, -200, -600  );
    //
    payload_printf(  &data1 );
    payload_printf(  &data2 );

    /* handle commandline options (see usage) */

    /// NEW INITIALIZATION - LOCAL SCOPE INIT FOR NOW
    Init_sq( &rb_test );

    while ((opt = getopt(argc, argv, "t:c:mlh")) != -1)
    {
        switch (opt)
        {
        case 't':
            testid = strtol(optarg, NULL, 0);
            break;
        case 'm':
            mutex_flag = 1;
            break;
        case 'c':
            cnt_events = strtol(optarg, NULL, 0);
            break;
        case 'l':
            log_flag = true;
            break;
        case 'h':
        default:
            fprintf(stderr, "Usage: %s %s\n", argv[0], cmd_arguments);
            exit(0);
        }
    }
    fprintf(stderr, "%s: ver=%s running testid=%d\n", argv[0], VERSION_STR, testid);
    if (log_flag)
        fprintf(stderr, "%s: event logger enabled with -l option\n"    "this signficantly increases the execution time\n", argv[0] );
    else
        fprintf(stderr, "%s: event logger not enabled\n", argv[0]);
    switch (testid)
    {
    case 1:
        fn_producer = q_producer_empty;
        break;
    case 2:
        fn_producer = q_producer_stress2;
        break;
    case 3:
        fn_producer = q_producer_stress3;
        break;
    default:
        fn_producer = q_producer_ut;
        break;
    }
    /* queue consumer is generic for all tests */
    fn_consumer = q_consumer;
//  #ifdef BARRIER
//  /* multithread, separate producer and consumer threads
//     use a barrier to start them at the same time
//   */
//  if (0 != pthread_barrier_init(&barrier, NULL, 2))
//    die("pthread_barrier_init");
//  #endif // BARRIER
//
///  TEST CODE
//
    ts_start();
//  if (0 != pthread_create(&producer, NULL, fn_producer, NULL))
//    die("pthread_create");
//  if (0 != pthread_create(&consumer, NULL, fn_consumer, NULL))
//    die("pthread_create");

//  /// CRITICAL SECTION - WHERE ACTION IN THREADS HAPPENS

//  /* wait for threads to exit */
//  pthread_join(producer, NULL);
//  pthread_join(consumer, NULL);

//  /// TEST SECTION - WHERE ACTION IN THREADS HAPPENS

//
    buf_t d = (void *) &data2;
    q_enq(&rb_test, d);
    q_enq(&rb_test, &data1);
    q_enq(&rb_test, &data1);
    q_enq(&rb_test, &data2);
    q_enq(&rb_test, &data1);
    fprintf(stdout,"testing callback...\n ");
    // you can see a data buffer with one call to callback like this...
    /// \note this is the proper refencing method for callback use
    // call by address to a pointer that aims at memory buffer
    rb_test.cb(&rb_test.enq);

    fprintf(stdout,"end testing callback...\n ");

    q_enq(&rb_test, &data2);
    q_enq(&rb_test, &data1);
    fprintf(stdout, " sq->count:%d  \n", rb_test.count );

    // test deq - SUCCESS
    /// \note this is the propery way to reference a buffer of a void * so that the data is updated
    /// by David T's code convention
    int a = q_deq( &rb_test, &tbuffer);
    if ( a == -1 )// don't write if garbage
        ;
    else
        payload_printf( tbuffer );



    ts_end();
    fprintf(stderr, "elapsed time from before first pthread_create to after last pthread_join: %s\n", ts_delta());
    if (log_flag)
    {
        /* dump all event log records to stdout AFTER the execution timer
         * has stopped. */
        print_evts();
    }
    if (!mutex_flag)
    {
        fprintf(stderr, "consumer contention lock_held_c=%d "
                "producer contention lock_held_p=%d\n",
                lock_held_c,
                lock_held_p);
    }

    /// NEW DECONSTRUCTION
    //Destroy_sq ( &rb_test );
    free(tbuffer);

}
