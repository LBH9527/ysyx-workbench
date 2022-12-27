#include <common.h>

#define RINGBUF_SIZE    8 * 128
typedef struct {
    char logbuf[RINGBUF_SIZE];
    uint32_t size;
    uint32_t head;
    uint32_t tail;

}ringbuf_t;


int ringbuf_init(ringbuf_t *r)
{
    memset(r->logbuf,0, sizeof(r->logbuf));
    r->size =  sizeof(r->logbuf)/sizeof(char);
    r->head = r->tail = 0;

    return 0;
}


int ringbuf_size(ringbuf_t *r)
{
    return ((r->head + r->size) % r->tail);
    
}


int ringbuf_read(ringbuf_t *r, char *data, size_t size)
{
    assert(r != NULL);

    int read_size = ringbuf_size(r);

    read_size = read_size > size ? size : read_size;

    while(read_size--)
    {
        *data = r->logbuf[r->tail];
        data++;
        r->tail++;
        if(r->tail >= r->size)
            r->tail = 0;
    }

    return read_size;
}

// 缓冲区满， 覆盖旧数据
int ringbuf_write(ringbuf_t *r, char *data, size_t size)
{
    assert(r != NULL);

    while(size--)
    {
        r->logbuf[r->head] = *data;
        r->head++;
        data++;
        if(r->head >= r->size)
            r->head = 0;
    }

    return size;
}


ringbuf_t g_instruct_buf;

void init_instruct_ringbuf(void) 
{
    ringbuf_init(&g_instruct_buf);

}

void instruct_ringbuf_write(char *data, size_t size, NEMUState state)
{
    char fix[6];
    // uint32_t i;

    if(state.state != NEMU_RUNNING)
    {
        sprintf(fix, " --> ");
    }
    else
    {
        sprintf(fix, "     "); 
    }
        
    ringbuf_write(&g_instruct_buf, fix, strlen(fix));
    ringbuf_write(&g_instruct_buf, data, size);
    sprintf(fix, "\r\n"); 
    ringbuf_write(&g_instruct_buf, fix, strlen(fix));

    if(state.state != NEMU_RUNNING)
    {
        printf("%s", g_instruct_buf.logbuf);
    }
}
