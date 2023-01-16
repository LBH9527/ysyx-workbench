/*
device trace
*/

#include "trace.h"

static FILE *trace_log_fp = NULL;
static uint32_t file_line = 0;
#define TRACE_LOG_MAX_LINE  1000

void init_dtrace(const char *log_file) 
{
    trace_log_fp = stdout;
    if (log_file != NULL) 
    {
        FILE *fp = fopen(log_file, "w");
        Assert(fp, "Can not open '%s'", log_file);
        trace_log_fp = fp;
    }
    file_line = 0;
    Log("Device Trace: %s", MUXDEF(CONFIG_DTRACE, ANSI_FMT("ON", ANSI_FG_GREEN), ANSI_FMT("OFF", ANSI_FG_RED)));    
    Log("Device trace is written to %s", log_file ? log_file : "stdout");

}

int device_trace(const char *name, paddr_t addr, int len, bool is_write)
{
    if(file_line++ < TRACE_LOG_MAX_LINE )
    {
        fprintf(trace_log_fp, "[#%03d] %s\t 0x%08x \t 0x%08x \t", file_line, name, addr, len);
        is_write == true ?  fprintf(trace_log_fp, "write \r\n") : fprintf(trace_log_fp, "read \r\n");
    }

    return 1;
}