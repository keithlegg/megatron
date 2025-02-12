#ifndef _COMMAND_BUFFER_
#define _COMMAND_BUFFER_




#define BUFFER_SIZE 80
#define TRUE    1
#define FALSE   0

typedef uint8_t rbuf_data_t;
typedef uint8_t rbuf_count_t;

typedef struct {
    rbuf_data_t buffer[80];
    rbuf_data_t *in;
    rbuf_data_t *out;
    rbuf_count_t    count;
} rbuf_t;

void rbuf_init(rbuf_t *);
bool rbuf_isempty(rbuf_t *);
void rbuf_insert(rbuf_t *, const rbuf_data_t);





#endif

