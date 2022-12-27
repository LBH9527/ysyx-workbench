#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>

typedef struct watchpoint WP;

extern WP *scan_watchpoint(void);
extern int list_watchpoint(void);
extern int set_watchpoint(char *e);
extern bool delete_watchpoint(int NO);
bool scan_watchpoint_display(void);

#endif
