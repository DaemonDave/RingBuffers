#include "iringbuffer.h"

// return element at row column
int IRingBuffer_at(int * array, unsigned int row, unsigned int col, unsigned int width)
{
    return array[ row * width + col  ];
}


float * create_iarray( unsigned int length,  unsigned int row)
{
    return malloc( length * row * sizeof(int) );
}

IRingBuffer *IRingBuffer_create(unsigned int length,  unsigned int row  )
{
    int i =0;
    IRingBuffer *buffer = malloc( sizeof(IRingBuffer) );
    // the length of rows involved
    buffer->length  = length;
    // the size ofthe rows
    buffer->rowsize =  row;
    buffer->start = 0;
    buffer->end = 0;
    unsigned long int element = 0;
    element  =  sizeof(float);
    buffer->rowoffset = row * element;
    // allocate - for row * length * int
    buffer->array   = create_iarray(  length,  row);
    if ( buffer->array  <= 0  )
    {
        printf("ERROR: allocating arrays");
        exit( -1);
        buffer->start_ptr = buffer->array;
        buffer->end_ptr = buffer->array;
    }
    // really confusing same named element and struct
    // from Zed Shaw
    return buffer;
}


int IRingBuffer_getrowsize (IRingBuffer *buffer )
{
    return buffer->rowsize;
}

/*! \fn IRingBuffer_getlength
 *
 *  returns current  ring buffer row size of floats
 *
 * */
int IRingBuffer_getlength ( IRingBuffer *buffer )
{
    return buffer->length;
}

void IRingBuffer_destroy(IRingBuffer *buffer)
{
    if(buffer)
    {
        free(buffer->array);
        free(buffer);
    }
}

//!  \todo - make it account for overlapping writes - over the top to the older data near start
int IRingBuffer_write(IRingBuffer *buffer, int *data, int length)
{
    // check if there are enough rows in the buffer to consume data
    if(IRingBuffer_available_data(buffer) == 0)
    {
        buffer->start = buffer->end = 0;
    }
    if(length > IRingBuffer_available_space(buffer))
    {
        //! \todo  reallocate to larger space at end
        // void *realloc(void *ptr, size_t size);
        // I know from previous instances that it adds to the end of the array.
        printf( "Not enough space: %d request, %d available \n",  IRingBuffer_available_data(buffer), length);
        goto error;
    }
    // compute pointer of target location
    buffer->end_ptr =  (buffer->array + (buffer->end * buffer->rowsize	));
    void *result = memcpy( (void *) buffer->end_ptr, data, length * buffer->rowoffset );
    if(result == NULL)
    {
        printf( "Failed to write data into buffer.");
    }
    // commit the write to the end count
    IRingBuffer_commit_write(buffer, length);
    buffer->write_count ++;
    // success return the amount of rows added
    return buffer->end;
error:
    return -1;
}

//! \todo - make it account for overlapping reads
int IRingBuffer_read(IRingBuffer *buffer, int *target, int amount)
{
    // fixed - if the amount to read out is more than existing the read will fail  / undefined behaviour
    if(amount >= IRingBuffer_available_data(buffer))
    {
        printf("Not enough in the buffer: has %d, needs %d \n", IRingBuffer_available_data(buffer), amount);
    }
    // his process is memcopy
    // compute address of target location
    buffer->start_ptr =  (buffer->array + (buffer->start * buffer->rowsize	));
    void *result = memcpy(target, (void *)buffer->start_ptr, amount * buffer->rowoffset);
    if(result == NULL)
    {
        printf( "Failed to write buffer into data.");
    }
    // commit the read to the start counter
    IRingBuffer_commit_read(buffer, amount);
    if(buffer->end == buffer->start)
    {
        buffer->start = buffer->end = 0;
    }
    return amount;
error:
    return -1;
}
// get a row of items as a pointer to data
// this should be an external function for computation
int  IRingBuffer_getrow(int * out, IRingBuffer *buffer, int amount )
{
    if( (buffer->rowsize) > 0 )
    {
        out = buffer->array;
        out += (amount * buffer->rowsize);
        printf (" array: %d  out: %d  rowsize %d  amount * rowsize: %d  sizeof(int): %d \n",  buffer->array,  out, buffer->rowsize,  amount * buffer->rowsize,  sizeof(int));
        return (int) out;
    }
    else
        return 0;
}

/*! \fn  IRingBuffer_printrow
 *
 * prints out the values from a single row of entries
 * size based on current rowsize
 *
*/
int IRingBuffer_printrow(IRingBuffer *buffer,  unsigned int row  )
{
    int i =0;
    if (row < buffer->length)
    {
        for (i = 0; i < buffer->rowsize; i++ )
        {
            printf("%d ",  IRingBuffer_at( buffer->array, row, i, buffer->rowsize ) );
        }
        printf(" \n" );
    }
    else
    {
        return -1;// number out of range
    }
    return 0;
}

