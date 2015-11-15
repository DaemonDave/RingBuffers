#include "ringbuffer.h"

#define STRING1 "THIS IS A STRING\0"
#define STRING2 "THIS IS ANOTHER STRING\0"
#define BLANKSTRING "               BLANK       \0"

  
#define RING_BUFFER	1000


int main(int argc, char **argv) 
{
	// test ring buffer functions
	RingBuffer *test;
  int ringbuffer_len = RING_BUFFER;
	int i = 0;
	int ret = 0;
	size_t  string_size = sizeof(STRING1);
	char 		first_string[100];
	char 		second_string[400];
	// 
	strncpy ( first_string, STRING1, string_size  );
	printf("this is the test string: %s of size %d \n", first_string, (int)string_size );
	// create a ring buffer of strings
	test = RingBuffer_create(ringbuffer_len);
  printf("this is the ring buffer size %d \n", (int) RingBuffer_getlength ( test ) );
	printf("STORING PHASE \n" );
	printf("return: %d start=%d  end: %d \n", ret, test->start, test->end);   
	// write something to offset
	ret =  RingBuffer_write( test, STRING2 , sizeof(STRING2));
	printf("return: %d start=%d  end: %d \n", ret, test->start, test->end); 
	// iterate  writing a string onto the ring buffer
	for (i =0; i < 10; i++)
	{
		ret =  RingBuffer_write( test, first_string , sizeof(STRING1));
		printf("return: %d start=%d  end: %d \n", ret, test->start, test->end); 
	}
	printf("READING PHASE \n" );
	printf("return: %d start=%d  end %d \n", ret, test->start, test->end); 	
	// read out and copy what we've written
	for (i =0; i < 10; i++)
	{
		ret =  RingBuffer_read( test, second_string, sizeof(STRING1));
		printf("return: %d start=%d  end: %d \n", ret, test->start, test->end); 
		printf ("this is the read string: [%s]\n", second_string  );
	}
	ret =  RingBuffer_read( test, second_string, sizeof(STRING2));
	printf("return: %d start=%d  end: %d \n", ret, test->start, test->end); 
	printf ("this is the read string: [%s]\n", second_string  );	
  exit(0);  // Use exit() to exit a program, do not use 'return' from main() - good advice
}
