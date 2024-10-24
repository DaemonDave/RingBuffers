#include "iringbuffer.h"


#define RING_BUFFER	1000


int main(int argc, char **argv)
{
    // test ring buffer functions
    IRingBuffer *test;
    int ringbuffer_len = RING_BUFFER;
    int rowsize = 6;
    int i = 0;
    int ret = 0;
    // an example row
    int row[6] = { 1,   2,   3,   4,   5,   6 	};

    int  array[3][6] =
    {
        {1,    85,  -99,     10,  0,   0 },
        {1557,   -70000,  -63245,  0,   4,   0 },
        {000,   100,    10,     10,   12,  0 }
    };

    int * rowptr = NULL;
    int * rowptr2 = NULL;

    int store [ 10 * 6];  // read out storage
    // create a ring buffer of strings
    test = IRingBuffer_create(ringbuffer_len, rowsize);
    printf("this is the double ring buffer length %d and row size %d \n", (int) IRingBuffer_getlength ( test ), (int) IRingBuffer_getrowsize ( test )  );
    printf("STORING PHASE \n" );
    // write something to offset
    ret =  IRingBuffer_write( test, row, 1);
    // iterate  writing a string onto the ring buffer
    for (i =0; i < 10; i++)
    {
        ret =  IRingBuffer_write( test, row, 1);
        printf("return: %d start=%d  end: %d \n", ret, test->start, test->end);
    }
    for (i =0; i < 10; i++)
    {
        ret =  IRingBuffer_write( test, row, 1);
        printf("return: %d start=%d  end: %d \n", ret, test->start, test->end);
    }
    printf("READING PHASE \n" );
    // read out and copy what we've written
    for (i =0; i < 10; i++)
    {
        IRingBuffer_printrow( test,  i );
    }
    ret =  IRingBuffer_read( test, store, 10);
    int j = 0;
    for (i =0; i < 10; i++)
    {
        for (j =0; j < 6; j++)
        {
            printf(" <%d>", store[ j + i ] );
        }
        printf("\n" );
    }
    // testing get row
    i = 2;
    printf(" IRingBuffer_getrow ... " );
    // int  IRingBuffer_getrow(int * out, IRingBuffer *buffer, int amount )
    ret = IRingBuffer_getrow(  rowptr, test,  i  );
    rowptr = (int *) ret;
    for (j =0; j < 6; j++)
    {
        printf("*rowptr = [%d] and value should be [%d] return %d \n", *rowptr, test->array[12 + j], rowptr);
        rowptr ++;
    }





    exit(0);  // Use exit() to exit a program, do not use 'return' from main()
}
