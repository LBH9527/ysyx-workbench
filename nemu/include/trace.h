#ifndef __TRACE_H__
#define __TRACE_H__

#include <common.h>
#include <isa.h>
 
extern void instruct_ringbuf_write(char *data, size_t size, NEMUState state);


extern int function_trace(word_t pc, word_t addr, bool is_call);
extern int device_trace(const char *name, paddr_t addr, int len, bool is_write);
extern int memory_trace(paddr_t addr, int len, bool is_write);
#endif
