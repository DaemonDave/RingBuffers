#include "ringbuffer.h"

RingBuffer *RingBuffer_create(int length)
{
    RingBuffer *buffer = calloc(1, sizeof(RingBuffer));
    buffer->length  = length + 1;
    buffer->start = 0;
    buffer->end = 0;
    // fixed calloc void *calloc(size_t nmemb, size_t size);
    buffer->buffer = malloc( buffer->length );

    // protect overwrites with final char
    buffer->buffer[buffer->length] = '\0';
    // really confusing same named element and struct
    return buffer;
}

int RingBuffer_getlength (RingBuffer *buffer )
{
    return buffer->length;
}

void RingBuffer_destroy(RingBuffer *buffer)
{
    if(buffer)
    {
        free(buffer->buffer);
        free(buffer);
    }
}

int RingBuffer_write(RingBuffer *buffer, char *data, int length)
{
    if(RingBuffer_available_data(buffer) == 0)
    {
        buffer->start = buffer->end = 0;
    }
    if(length >= RingBuffer_available_space(buffer))
    {
        printf( "Not enough space: %d request, %d available \n",  RingBuffer_available_data(buffer), length);
        goto error;
    }
    void *result = memcpy(RingBuffer_ends_at(buffer), data, length);
    if(result == NULL)
    {
        printf( "Failed to write data into buffer.");
    }
    //RingBuffer_commit_write(buffer, length);
    buffer->end = (buffer->end + length) % buffer->length;

    return buffer->end;
error:
    return -1;
}

int RingBuffer_read(RingBuffer *buffer, char *target, int amount)
{
    // fixed - if the amount to read out is more than existing the read will fail  / undefined behaviour
    if(amount >= RingBuffer_available_data(buffer))
    {
        printf("Not enough in the buffer: has %d, needs %d \n", RingBuffer_available_data(buffer), amount);
    }
    void *result = memcpy(target, RingBuffer_starts_at(buffer), amount);
    if(result == NULL)
    {
        printf( "Failed to write buffer into data.");
    }

    RingBuffer_commit_read(buffer, amount);

    if(buffer->end == buffer->start)
    {
        buffer->start = buffer->end = 0;
    }

    return amount;
error:
    return -1;
}

char * RingBuffer_gets(RingBuffer *buffer, int amount)
{
    if(amount > 0, "Need more than 0 for gets, you gave: %d ", amount);
    if(amount <= RingBuffer_available_data(buffer))
    {
        printf( "Not enough in the buffer.");
    }

    char * result =  malloc( sizeof(amount) );
    strncpy( result, RingBuffer_starts_at(buffer), (size_t) amount );
    if(result != NULL)
    {
        printf( "Failed to create gets result.");
    }
    if(sizeof(result) == amount)
    {
        printf("Wrong result length. \n");
    }
    RingBuffer_commit_read(buffer, amount);
    assert(RingBuffer_available_data(buffer) >= 0 && "Error in read commit.");

    return result;
error:
    return NULL;
}
