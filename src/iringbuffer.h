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


// dynamic buffer of rowsize * length * number of elements
typedef struct  __attribute__ ((__packed__))
{
    // the data area
    int * array;
    // the number of rows in the entire entry
    unsigned long int length;
    // the size of the float entity of array size
    unsigned long int rowsize;
    // the computed size offset per row
    unsigned int rowoffset;
    // transitory pointers for address offsets
    int * start_ptr;
    int * end_ptr;
    // the current starting point
    unsigned long int start;
    // the current ending point
    unsigned long int end;
    // the number of writes
    unsigned int write_count;
}
IRingBuffer;

/*! \fn  IRingBuffer_create
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
IRingBuffer *IRingBuffer_create( unsigned int length,  unsigned int row  );

/*! \fn IRingBuffer_destroy
 * frees up the allocated memory
 *
 *
 * */
void IRingBuffer_destroy( IRingBuffer *buffer);

/*! \fn IRingBuffer_getlength
 *
 *  returns current length of the ring buffer rows
 *
 * */
int IRingBuffer_getlength ( IRingBuffer *buffer );

/*! \fn IRingBuffer_getrowsize
 *
 *  returns current  ring buffer row size of floats
 *
 * */
int IRingBuffer_getrowsize ( IRingBuffer *buffer );

/*! \fn IRingBuffer_read
 *
 *   Read at the current position but don't increase the start and end
 *   postitions.
 * */
int IRingBuffer_read( IRingBuffer *buffer, int *target, int amount);

/*! \fn IRingBuffer_push
 *	Pushes onto the stack length amount of rows
 *  on top of old data.
 *
 * */
int IRingBuffer_push( IRingBuffer *buffer, int *data, int length);

/*! \fn IRingBuffer_pop
 *	Pushes onto the stack length amount of rows
 *  on top of old data.
 *
 * */
int IRingBuffer_pop( IRingBuffer *buffer, int *data, int length);

/*! \fn IRingBuffer_write
 *  Writes to the stack at the current position without updating the start and end position
 *
 * */
int IRingBuffer_write( IRingBuffer *buffer, int *data, int length);


/*! \fn IRingBuffer_empty
 *
 *
 * */
int IRingBuffer_empty( IRingBuffer *buffer);

/*! \fn IRingBuffer_full
 *
 *
 * */
int IRingBuffer_full( IRingBuffer *buffer);

/*! \fn IRingBuffer_available_data
 *
 *
 * */
int IRingBuffer_available_data(IRingBuffer *buffer);

/*! \fn IRingBuffer_available_space
 *
 *
 * */
int IRingBuffer_available_space(IRingBuffer *buffer);

/*! \fn IRingBuffer_getrow
 *
 * return a row pointer for computation
 *
 * looks into buffer at row amount (0 is first row in ring buffer)
 * and assigns the pointer out to that value at the first [0] array value.
 *
 * Returns the address value or zero for failure.
 *
 * This used to have a return pointer but it will be a multiple-accessed function and
 * we don't want memory leaks for wasted non-freed allocations.
 *
 * */
int  IRingBuffer_getrow(int * out, IRingBuffer *buffer, int amount );

int IRingBuffer_getrowsize (IRingBuffer *buffer );

/*! \fn  IRingBuffer_printrow
 *
 * prints out the values from a single row of entries
 * size based on current rowsize
 *
 *
*/
int IRingBuffer_printrow(IRingBuffer *buffer,  unsigned int row  );

//! \todo need to change start / end calculations
//  for rows of arrays
//  for floats in a row
//  returns length in the ring buffer of rows of data
#define IRingBuffer_available_data(B) (((B)->end + 1) % (B)->length - (B)->start - 1)
// returns length of empty rows remaining
#define IRingBuffer_available_space(B) ((B)->length - (B)->end - 1)
// return if ring buffer is full of data
#define IRingBuffer_full(B) (IRingBuffer_available_data((B)) - (B)->length == 0)
// returns if ring buffer is empty - completely available
#define IRingBuffer_empty(B) (IRingBuffer_available_data((B)) == 0)
// computes the current start of empty data
#define IRingBuffer_starts_at(B) ((B)->array +  (B)->start  )
// computes the current end of written data
#define IRingBuffer_ends_at(B) ((B)->array + (B)->end   )
// moves down the start to before the latest data
#define IRingBuffer_commit_read(B, A) ((B)->start = ((B)->start + (A)) % (B)->length)
// moves up the end to at new data
#define IRingBuffer_commit_write(B, A) ((B)->end = ((B)->end + (A)) % (B)->length)
