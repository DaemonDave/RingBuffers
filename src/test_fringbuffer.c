#include "fringbuffer.h"

  
#define RING_BUFFER	1000


int main(int argc, char **argv) 
{
	// test ring buffer functions
	FRingBuffer *test;
  int ringbuffer_len = RING_BUFFER;
  int rowsize = 6;
	int i = 0;
	int ret = 0;
	// an example row
	float row[6] = { 1.2,   2.3,   3.0,   4,   5,   6.6666 	};
 
 	float row2[6] = { 1.22,   2.3,   3.0,   4,   5,   6.6666 	};
	float row3[6] = { 1.2000001,   2.3,   3.0,   4,   5,   6.6666 	}; 	
 
 
	float array[3][6] = 
	{
		{0.2,    85.4 ,  -99.9,     10,  0,   0 },
		{1557,   -70000,  -6.3245,  0,   4,   0 },
		{0.0001,   100,    10,     10,   12,  0 }
	};
	

	float * rowptr;
	float * rowptr2;
		

	float store [ 10 * 6];  // read out storage
	// create a ring buffer of strings
	test = FRingBuffer_create(ringbuffer_len, rowsize);
  printf("this is the double ring buffer length %d and row size %d \n", (int) FRingBuffer_getlength ( test ), (int) FRingBuffer_getrowsize ( test )  );
	printf("STORING PHASE \n" );  
	// write something to offset
	ret =  FRingBuffer_write( test, row , 1);
	// iterate  writing a string onto the ring buffer
	for (i =0; i < 10; i++)
	{
		ret =  FRingBuffer_write( test, row , 1);
		printf("return: %d start=%d  end: %d \n", ret, test->start, test->end); 
	}
		ret =  FRingBuffer_write( test, row2 , 1);		
	for (i =0; i < 10; i++)
	{
		ret =  FRingBuffer_write( test, row3 , 1);
		printf("return: %d start=%d  end: %d \n", ret, test->start, test->end); 
	}	 
	// fuse the rows
	FRingBuffer_fuserows( test,  3,  NULL );
	printf("READING PHASE \n" );	
	// read out and copy what we've written
	for (i =0; i < 10; i++)
	{
    FRingBuffer_printrow( test,  i );	
	}
	ret =  FRingBuffer_read( test, store , 10);
	int j = 0;
	for (i =0; i < 10; i++)
	{
    	for (j =0; j < 6; j++)
			{
						printf(" <%f>", store[ j + i ] ); 	
			}
			printf("\n" ); 
	}
	i = 2;
	printf(" FRingBuffer_getrow ... " ); 
	// int  FRingBuffer_getrow(int * out, FRingBuffer *buffer, int amount )
	rowptr = FRingBuffer_getrow(  test,  i  );	

  for (j =0; j < 6; j++)
	{
		printf("*rowptr = [%f] and value should be [%f] return %d \n", *rowptr, test->array[12 + j], (int) rowptr); 	
		rowptr ++;
	}		
	
	
	
	
	
  exit(0);  // Use exit() to exit a program, do not use 'return' from main()
}
