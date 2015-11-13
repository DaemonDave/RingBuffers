/*  
 * Double ring buffer
 * 
 * */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// dynamic buffer of size * length * number of elements

typedef struct
{
		// the data area
    double ** buffer;
    // the number of rows in the entire entry
    unsigned int length;
    // the current starting point 
    unsigned int start;
    // the current ending point
    unsigned int end;
} DRingBuffer;



/*! \fn  DRingBuffer_create
 * 
 * new create makes a variable row size malloc of doubles once
 * 
 * Constructs a dynamic array / matrix of:
 * 
 * row *  length  = array of array elements
 * 
 * nested double double buffer[1][2]
 * 
 * reads in and out row by row
 *  
 * 
*/ 
DRingBuffer *DRingBuffer_create(unsigned int length,  unsigned int row  );

void DRingBuffer_destroy(DRingBuffer *buffer);

// returns current length
int DRingBuffer_getlength (DRingBuffer *buffer );

int DRingBuffer_read(DRingBuffer *buffer, double *target, int amount);

int DRingBuffer_multiread(DRingBuffer *buffer, double *target, int amount);

int DRingBuffer_write(DRingBuffer *buffer, double *data, int length);

int DRingBuffer_multiwrite(DRingBuffer *buffer, double *data, int length);

int DRingBuffer_empty(DRingBuffer *buffer);

int DRingBuffer_full(DRingBuffer *buffer);

int DRingBuffer_available_data(DRingBuffer *buffer);

int DRingBuffer_available_space(DRingBuffer *buffer);

char * DRingBuffer_gets(DRingBuffer *buffer, int amount);
// return a number based on current length
// minus the start point as the space left
#define DRingBuffer_available_data(B) (((B)->end + 1) % (B)->length - (B)->start - 1)

#define DRingBuffer_available_space(B) ((B)->length - (B)->end - 1)

#define DRingBuffer_full(B) (DRingBuffer_available_data((B)) - (B)->length == 0)

#define DRingBuffer_empty(B) (DRingBuffer_available_data((B)) == 0)

#define DRingBuffer_puts(B, D) DRingBuffer_write((B), (D), sizeof((D)))

#define DRingBuffer_get_all(B) DRingBuffer_gets((B), DRingBuffer_available_data((B)))

#define DRingBuffer_starts_at(B) ((B)->buffer + (B)->start)

#define DRingBuffer_ends_at(B) ((B)->buffer + (B)->end)

#define DRingBuffer_commit_read(B, A) ((B)->start = ((B)->start + (A)) % (B)->length)

#define DRingBuffer_commit_write(B, A) ((B)->end = ((B)->end + (A)) % (B)->length)
