#ifndef __BREAKPOINT_H__
#define __BREAKPOINT_H__

#include <common.h>

typedef struct breakpoint
{
	uint8_t ori_byte : 8;
	bool enable : 1;
	bool in_use : 1;
	int NO : 22;

	union {
		vaddr_t addr;
		struct
		{
			char *expr;
			uint32_t old_val;
			uint32_t new_val;
		};
	};
	int type;
	struct breakpoint *next;

	/* TODO: Add more member if necessary */

} BP;



void init_bp_list();
BP *find_breakpoint(vaddr_t addr);
void list_breakpoint();
void delete_all_breakpoint();
bool display_found_breakpoint(vaddr_t addr);
int set_breakpoint(vaddr_t addr);

#endif
