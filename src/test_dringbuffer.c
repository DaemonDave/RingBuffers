#include "dringbuffer.h"

  
#define RING_BUFFER	1000

#define BIG_RING_BUFFER 35000

int main(int argc, char **argv) 
{
	// test ring buffer functions
	DRingBuffer *test;
	int ringbuffer_len = RING_BUFFER;
	int rowsize = 6;
	int i = 0;
	int ret = 0;
	// an example row
	double row[6] = { 1.2,   2.3,   3.0,   4,   5,   6.6666 	};
 
	double array[3][6] = 
	{
		{0.2,    85.4 ,  -99.9,     10,  0,   0 },
		{1557,   -70000,  -6.3245,  0,   4,   0 },
		{0.0001,   100,    10,     10,   12,  0 }
	};

	double * rowptr;
	double * rowptr2;
	

	double store [ 10 * 6];  // read out storage
	// create a ring buffer of strings
	test = DRingBuffer_create(ringbuffer_len, rowsize);
	printf("this is the double ring buffer length %d and row size %d \n", (int) DRingBuffer_getlength ( test ), (int) DRingBuffer_getrowsize ( test )  );
	printf("STORING PHASE \n" );  
	// write something to offset
	ret =  DRingBuffer_write( test, row , 1);
	// iterate  writing a string onto the ring buffer
	for (i =0; i < 10; i++)
	{
		ret =  DRingBuffer_write( test, row , 1);
		printf("return: %d start=%d  end: %d \n", ret, test->start, test->end); 
	}
	for (i =0; i < 10; i++)
	{
		ret =  DRingBuffer_write( test, row , 1);
		printf("return: %d start=%d  end: %d \n", ret, test->start, test->end); 
	}	 
	printf("READING PHASE \n" );	
	// read out and copy what we've written
	for (i =0; i < 10; i++)
	{
    DRingBuffer_printrow( test,  i );	
	}
	ret =  DRingBuffer_read( test, store , 10);
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
	printf(" DRingBuffer_getrow ... " ); 
	// int  DRingBuffer_getrow(int * out, DRingBuffer *buffer, int amount )
	ret = DRingBuffer_getrow(  rowptr, test,  i  );	
	rowptr = (int *) ret;
	for (j =0; j < 6; j++)
	{
		printf("*rowptr = [%f] and value should be [%f] return %d \n", *rowptr, test->array[12 + j], rowptr); 	
		rowptr ++;
	}	
	printf(" DRingBuffer_destroy ... " );   	
	DRingBuffer_destroy(test );
	printf(" done \n " );   
	// recreate with much larger range
	printf(" Large array ... " );   
	test = DRingBuffer_create(BIG_RING_BUFFER, rowsize);
	printf("this is the double ring buffer length %d and row size %d \n", (int) DRingBuffer_getlength ( test ), (int) DRingBuffer_getrowsize ( test )  );
	for (i =0; i < BIG_RING_BUFFER-1; i++)
	{
		ret =  DRingBuffer_write( test, row , 1);
	}
	printf("large double ring buffer wrote %d times \n", i );

	printf(" DRingBuffer_destroy ... " );   	
	DRingBuffer_destroy(test );	
	
  exit(0);  // Use exit() to exit a program, do not use 'return' from main()
}
