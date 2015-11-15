/*  
 * float ring buffer - custom specific ring buffers for arrays of floats
 * 
 * What does this change to the existing ring buffer paradigm?
 * 
 * If the row is the basic unit of reading and writing
 * 
 * then are we inserting or pushing or just overwriting?
 *
 * We can also have sub row inserts 
 * 
 * Push/ Pop - writes to and increases / reads from and decreases the start/end point within
 * the ring.
 * Write/ Read - overwrites n instances of r rows starting at the current location without changing start/end
 * 
 * 
 * 
 *  
 * */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


// within a millionth of same value
#define TOLERANCE 0.000001

// dynamic buffer of rowsize * length * number of elements
typedef struct  __attribute__ ((__packed__)) 
{
		// the data area
    float * array;
    // the number of rows in the entire entry
    unsigned long int length;
    // the size of the float entity of array size
    unsigned long int rowsize;
    // the computed size offset per row
    unsigned int rowoffset;
    // transitory pointers for address offsets
    float * start_ptr;
    float * end_ptr;
    // the current starting point 
    unsigned long int start;
    // the current ending point
    unsigned long int end;
    // the number of writes
    unsigned int write_count;
} FRingBuffer;

/*! \fn  FRingBuffer_create
 * 
 * new create makes a variable row size malloc of floats once
 * 
 * Constructs a dynamic array / matrix of:
 * 
 * row *  length  = array of array elements
 * 
 * nested float float buffer[1][2]
 *  
 * 
*/ 
FRingBuffer *FRingBuffer_create( unsigned int length,  unsigned int row  );

/*! \fn FRingBuffer_destroy
 * frees up the allocated memory
 * 
 * 
 * */
void FRingBuffer_destroy( FRingBuffer *buffer);

/*! \fn FRingBuffer_getlength
 *
 *  returns current length of the ring buffer rows
 * 
 * */
int FRingBuffer_getlength ( FRingBuffer *buffer );

/*! \fn FRingBuffer_getrowsize
 *
 *  returns current  ring buffer row size of floats
 * 
 * */
int FRingBuffer_getrowsize ( FRingBuffer *buffer );

/*! \fn FRingBuffer_read
 *  
 *   Read at the current position but don't increase the start and end
 *   postitions.
 * */
int FRingBuffer_read( FRingBuffer *buffer, float *target, int amount);

/*! \fn FRingBuffer_push
 *	Pushes onto the stack length amount of rows
 *  on top of old data.
 * 
 * */
int FRingBuffer_push( FRingBuffer *buffer, float *data, int length);

/*! \fn FRingBuffer_pop
 *	Pushes onto the stack length amount of rows
 *  on top of old data.
 * 
 * */
int FRingBuffer_pop( FRingBuffer *buffer, float *data, int length);

/*! \fn FRingBuffer_write
 *  Writes to the stack at the current position without updating the start and end position
 * 
 * */
int FRingBuffer_write( FRingBuffer *buffer, float *data, int length);


/*! \fn FRingBuffer_empty
 *
 * 
 * */
int FRingBuffer_empty( FRingBuffer *buffer);

/*! \fn FRingBuffer_full
 *
 * 
 * */
int FRingBuffer_full( FRingBuffer *buffer);

/*! \fn FRingBuffer_available_data
 *
 * 
 * */
int FRingBuffer_available_data(FRingBuffer *buffer);

/*! \fn FRingBuffer_available_space
 *
 * 
 * */
int FRingBuffer_available_space(FRingBuffer *buffer);

/*! \fn FRingBuffer_getrow
 *
 * return a row pointer for computation
 * get a row of items as a pointer to data
 * this should be an external function for computation
 * */
float *  FRingBuffer_getrow(  FRingBuffer *buffer, int amount );
/*! \fn FRingBuffer_getrowsize
 *
 * return a row pointer for computation
 * get a row of items as a pointer to data
 * this should be an external function for computation
 * */
int FRingBuffer_getrowsize (FRingBuffer *buffer );

/*! \fn  FRingBuffer_printrow
 * 
 * prints out the values from a single row of entries
 * size based on current rowsize
 * 
*/ 
int FRingBuffer_printrow(FRingBuffer *buffer,  unsigned int row  );

/* An opaque, incomplete type for the FIRST-CLASS ADT. */
// pointer to function 
typedef int  (FRingBuffer_FuseFcn)( float * a, float * b, unsigned int start, unsigned int end , unsigned int rowsize  );

/*!  \fn  FRingBuffer_fuserows
 * 
 * 	Fuse the contents of ring buffer  
 *  by conflation function / data fusion function 
 *  that seeks through rows and looks at certain values specified 
 *  by \var range [ from 0 to range values in comparison  ] for comparison.
 *  
 * 	
 * */
int FRingBuffer_fuserows(FRingBuffer *buffer,  unsigned int range,  FRingBuffer_FuseFcn fcn );


//! \todo need to change start / end calculations - DONE
//  for rows of arrays
//  for floats in a row
//  returns length in the ring buffer of rows of data 
#define FRingBuffer_available_data(B) (((B)->end + 1) % (B)->length - (B)->start - 1)
// returns length of empty rows remaining
#define FRingBuffer_available_space(B) ((B)->length - (B)->end - 1)
// return if ring buffer is full of data
#define FRingBuffer_full(B) (FRingBuffer_available_data((B)) - (B)->length == 0)
// returns if ring buffer is empty - completely available
#define FRingBuffer_empty(B) (FRingBuffer_available_data((B)) == 0)
// computes the current start of empty data
#define FRingBuffer_starts_at(B) ((B)->array +  (B)->start  )
// computes the current end of written data
#define FRingBuffer_ends_at(B) ((B)->array + (B)->end   )
// moves down the start to before the latest data
#define FRingBuffer_commit_read(B, A) ((B)->start = ((B)->start + (A)) % (B)->length)
// moves up the end to at new data
#define FRingBuffer_commit_write(B, A) ((B)->end = ((B)->end + (A)) % (B)->length)
