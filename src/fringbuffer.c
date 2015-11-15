#include "fringbuffer.h"

#define DEBUG

// return element at row column
float FRingBuffer_at(float * array, unsigned int row, unsigned int col, unsigned int width)
{
	return array[ row * width + col  ];
}


float * create_farray( unsigned int length,  unsigned int row)
{
    return malloc( length * row * sizeof(float) );
} 

FRingBuffer *FRingBuffer_create(unsigned int length,  unsigned int row  )
{
		int i =0;
    FRingBuffer *buffer = malloc( sizeof(FRingBuffer) );
    // the length of rows involved
    buffer->length  = length;
    // the size ofthe rows
    buffer->rowsize =  row;
    buffer->start = 0;
    buffer->end = 0;
    unsigned long int element = 0;
    element  =  sizeof(float);
    buffer->rowoffset = row * element;
		// allocate - for row * length * float
		buffer->array   = create_farray(  length,  row);
		if ( buffer->array  <= 0  )
		{
			printf("ERROR: allocating arrays");
			exit( -1);
			buffer->start_ptr = buffer->array;
			buffer->end_ptr = buffer->array;			
		}
		// really confusing same named element and struct buffer buffer
		// from Zed Shaw
    return buffer;
}


int FRingBuffer_getrowsize (FRingBuffer *buffer )
{
	return buffer->rowsize;
}

/*! \fn FRingBuffer_getlength
 *
 *  returns current  ring buffer row size of floats
 * 
 * */
int FRingBuffer_getlength ( FRingBuffer *buffer )
{
	return buffer->length;
}

void FRingBuffer_destroy(FRingBuffer *buffer)
{
    if(buffer) 
		{
      free(buffer->array);
      free(buffer);
    }
}

//!  \todo - make it account for overlapping writes - over the top to the older data near start
int FRingBuffer_write(FRingBuffer *buffer, float *data, int length)
{
		// check if there are enough rows in the buffer to consume data
    if(FRingBuffer_available_data(buffer) == 0) 
		{
        buffer->start = buffer->end = 0;
    }
    if(length > FRingBuffer_available_space(buffer))
		{
			//! \todo  reallocate to larger space at end
			// void *realloc(void *ptr, size_t size);
			// I know from previous instances that it adds to the end of the array.
    	printf( "Not enough space: %d request, %d available \n",  FRingBuffer_available_data(buffer), length);
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
    FRingBuffer_commit_write(buffer, length);
    buffer->write_count ++;
	  // success return the amount of rows added
    return buffer->end;
error:
    return -1;
}

//! \todo - make it account for overlapping reads
int FRingBuffer_read(FRingBuffer *buffer, float *target, int amount)
{
		// fixed - if the amount to read out is more than existing the read will fail  / undefined behaviour
    if(amount >= FRingBuffer_available_data(buffer))
		{
    	printf("Not enough in the buffer: has %d, needs %d \n", FRingBuffer_available_data(buffer), amount);
		}
		// his process is memcopy
		// compute address of target location
		buffer->start_ptr =  (buffer->array + (buffer->start * buffer->rowsize	));
    void *result = memcpy(target, (void *)buffer->start_ptr , amount * buffer->rowoffset);
    if(result == NULL)
		{
			printf( "Failed to write buffer into data.");
		}
		// commit the read to the start counter
    FRingBuffer_commit_read(buffer, amount);
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
float *  FRingBuffer_getrow(  FRingBuffer *buffer, int amount )
{
    if( (buffer->rowsize) > 0 && (  FRingBuffer_available_data(buffer)  )  )
    {
			
			#ifdef DEBUG
				printf (" array: %d  out: %d  rowsize %d  amount * rowsize: %d  sizeof(int): %d \n",  buffer->array, (buffer->array + (amount * buffer->rowsize)), buffer->rowsize,  amount * buffer->rowsize ,  sizeof(float));			
			#endif
			return (float *)  ( buffer->array + (amount * buffer->rowsize)  );
		}
		else
		   return -1;
}

/*! \fn  FRingBuffer_printrow
 * 
 * prints out the values from a single row of entries
 * size based on current rowsize
 * 
*/ 
int FRingBuffer_printrow(FRingBuffer *buffer,  unsigned int row  )
{
		int i =0;
		if (row < buffer->length)
		{
			for (i = 0; i < buffer->rowsize; i++ )
			{ 
				printf("%f ",  FRingBuffer_at( buffer->array, row, i, buffer->rowsize ) );
			}
			printf(" \n" );
		}
		else
		{
			return -1;// number out of range
		}
		return 0;
}
// default Fusion function
// compare rows a and b then if within tolerance - fuse into a
int  FuseFcn ( float * a, float * b, unsigned int start, unsigned int end, unsigned int rowsize  )
{
	float  delta;
	int diff;
	float * inputa = a;
	float * inputb = b;
	unsigned int i;
	int j = 0;
	diff = end - start;
	delta = TOLERANCE;
	
	printf( "GOT into fusion %d \n", a );
	// check for all zeros in a - default empty - then exit
	for (i = 0; i < diff ; i++ )
	{
		if (*inputa == 0.0 ) j++;		
	  printf( "[%d]= %f", i, *inputa );
		inputa++; // post decrement address
	}
	// escape if one of them is all zeros
	if ( j == rowsize -1 ) return 0;// successful comparison
	for (i = 0; i < end; i++ )
	{
		// must compare all and get to end to be almost identical
		if ( abs(*inputa) - abs(*b) < delta  )
		{
			j ++;
		}
		else // any comparison inequal then exit
		{
			return -1;
		}
		inputa++;
		inputb++;
	}
	if (j == diff ) // if all are within tolerance then zero out one row
	{
		inputa = a;
		for (i = 0; i < rowsize -1; i++ )
		{
			*inputa = 0.0;
			inputa++;
		}
		return 0; // comparatively  the same within tolerance
	}
	return -1;
}




/*!  \fn  FRingBuffer_fuserows
 * 
 * 	Fuse the contents of ring buffer using conflation function / data fusion function 
 *  specified by callback  ( \var fcn callback that does fusion )
 *  that seeks through rows and looks at certain values specified 
 *  by \var range [ from 0 to range values in comparison  ] for comparison.
 *  
 * \var fcn can be declared or default to basic function
 * 	
 * */
int FRingBuffer_fuserows(FRingBuffer *buffer,  unsigned int range,  FRingBuffer_FuseFcn fcn ) 
{
	FRingBuffer_FuseFcn * output;
	// check for data 
	if (FRingBuffer_available_data(buffer) )
	{
		int i = 0;
		int ret;
		// row pointers
		float * a;
		float * b;
		unsigned int removed_rows=0; // count up the number of rows removed
		
		if ( fcn == NULL ) // use default function if none given
		{
				output = FuseFcn;
		}
		int j = (buffer->end  -1);
		// simple upper lower compare for now
		// compare top to bottom till halfway
		for (i = 0; i <  ( buffer->end/2)  ; i++ )
		{
			 a = FRingBuffer_getrow( buffer, i );
			 b = FRingBuffer_getrow( buffer, j );
			 // run the fusion function
			 ret = output (  a,  b, 0 , range, buffer->rowsize ); 
			 if (ret == 0)// successful fusion
			 {
				 removed_rows++;
			 }
			 j--;// decrement top counter
		}
		return removed_rows;
	}
	else
	{
		return -1; // no data
	}
	
}

