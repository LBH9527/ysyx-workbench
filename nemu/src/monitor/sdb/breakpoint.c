#include "sdb.h"
#include "breakpoint.h"

#define NR_BP 32


static BP bp_list[NR_BP];
static BP *head, *free_;

void init_bp_list()
{
	int i;
	for (i = 0; i < NR_BP; i++)
	{
		bp_list[i].NO = i;
		bp_list[i].in_use = false;
		bp_list[i].next = &bp_list[i + 1];
	}
	bp_list[NR_BP - 1].next = NULL;

	head = NULL;
	free_ = bp_list;
}

static BP *new_BP()
{
    BP *pBP;
    assert(free_);
    pBP  = free_;
    free_ = free_->next;
    assert(pBP->in_use == false);
    pBP->in_use = true;
    return pBP;

}

static void free_BP(BP *p)
{
    assert(p >= &bp_list[0] && p < &bp_list[NR_BP]);
    assert(p->in_use == true);
    p->next = free_;
    free_ = p;
    p->in_use = false;
}

int set_breakpoint(vaddr_t addr)
{
    BP *pBP;
    pBP = new_BP();
    pBP->addr = addr;
    pBP->enable = true;
    pBP->next = head;
    head = pBP;

    return pBP->NO;
}

bool delete_breakpoint(int NO)
{
    BP *pBP;
    bool is_find = false;
    
    if(head->NO == NO)
    {
        pBP = head;
        head = pBP->next;
        free_BP(pBP);
        is_find = true;        
    }
    else
    {
        for(pBP = head; pBP != NULL; pBP = pBP->next)
        {            
            if(pBP->next->NO == NO)
            {
                free_BP(pBP->next);
                pBP->next = pBP->next->next;
                is_find = true;
                break;
            }     

        }
    }
    return is_find;
}

 
void delete_all_breakpoint()
{
    BP *pBP;
    BP *pnextBP;

    if (head == NULL)
    {
        return;
    }
    for(pBP = head; pBP != NULL; pBP = pnextBP)
    {
        pnextBP = pBP->next;
        free(pBP);
    }

}

void list_breakpoint()
{
    BP *pBP;

	if (head == NULL)
	{
		printf("No breakpoints\n");
		return;
	}

	printf("%8s\t%6s\t%8s\t%8s\n", "NO", "Type", "Address", "Enable");

    for(pBP = head; pBP != NULL; pBP = pBP->next)
    {
        // printf("%8d\t%6s\t%#08x\t%8s\t%x\n", pBP->NO, "B", pBP->addr, pBP->enable ? "Yes" : "No", pBP->ori_byte);
    }

}

BP *find_breakpoint(vaddr_t addr)
{
    BP *pBP;
    bool is_find = false;

    for(pBP = head; pBP != NULL; pBP = pBP->next)
    {
        assert(pBP->in_use == true);
        if(pBP->addr == addr)
        {
            is_find = true;
            break;
        }
    }
    if (is_find)
        return pBP;
    else
        return NULL;
}

bool display_found_breakpoint(vaddr_t addr)
{
    if(find_breakpoint(addr) != NULL)
    {
        return true;
    }
    return false;
}

void resume_breakpoints()
{

}

void mask_breakpoints()
{

}