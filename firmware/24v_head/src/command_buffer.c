/*
    buffer - taken from the AVR forums. Not sure how it works yet, still testing 

    TO USE: include these in your code.
     
      volatile bool command;      // Command line active? 
      volatile bool quit_early;   // Abort processing. 
      rbuf_t  rbuf;
      char line[BUFFER_SIZE];

*/


#include <avr/io.h>
#include <util/atomic.h>

#include "command_buffer.h"


rbuf_count_t rbuf_getcount(rbuf_t *);
rbuf_data_t rbuf_remove(rbuf_t *);



void rbuf_init(rbuf_t* const buffer)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        buffer->in    = buffer->buffer;
        buffer->out   = buffer->buffer;
        buffer->count = 0;
    }
    return;
}


rbuf_count_t rbuf_getcount(rbuf_t* const buffer)
{
    rbuf_count_t count;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        count = buffer->count;
    }
    return count;
}


bool rbuf_isempty(rbuf_t* buffer)
{
    return (rbuf_getcount(buffer) == 0);
}


void rbuf_insert(rbuf_t* const buffer, const rbuf_data_t data)
{
    *buffer->in = data;

    if (++buffer->in == &buffer->buffer[BUFFER_SIZE])
        buffer->in = buffer->buffer;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        buffer->count++;
    }
}


rbuf_data_t rbuf_remove(rbuf_t* const buffer)
 {
    rbuf_data_t data = *buffer->out;

    if (++buffer->out == &buffer->buffer[BUFFER_SIZE])
        buffer->out = buffer->buffer;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        buffer->count--;
    }

    return data;
}

