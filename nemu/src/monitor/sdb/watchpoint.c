/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "sdb.h"

#define NR_WP 32
#define EXPR_SIZE 128

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  
  /* TODO: Add more members if necessary */
  bool in_use;
  struct
  {
    char expr[EXPR_SIZE];
    uint32_t old_val;
    uint32_t new_val;
  };

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].in_use = false;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
static WP* new_wp()
{
    assert(free_->next != NULL);
    WP *p = free_;
    free_ = free_->next ;
    assert(p->in_use == false);
    p->in_use = true;
  
    return p;
}
void free_wp(WP *wp)
{
  assert(wp >= &wp_pool[0] && wp < &wp_pool[NR_WP - 1]);
  assert(wp->in_use == true);


  wp->in_use = false;
  wp->next = free_;
  free_ = wp;
}

WP *scan_watchpoint(void)
{
  WP *p;
  bool success;

  for (p = head; p!= NULL; p = p->next)
  {
    assert(p->in_use == true);
    
    p->new_val = expr(p->expr, &success);
    if (p->old_val != p->new_val)
    {
      return p;
    }
  }

  return NULL;
}

int set_watchpoint(char *e)
{
	uint32_t val;
	bool success;
	val = expr(e, &success);
	if (!success)
		return -1;
    
  WP *p = new_wp();
  assert((strlen(e) + 1) <= EXPR_SIZE);
  strcpy(p->expr, e);
	p->old_val = val;

  p->next = head;
  head = p;

  return p->NO;

}

bool delete_watchpoint(int NO)
{
    WP *p;
    WP *p_prev;

    if (head->NO == NO)
    {
        p = head;
        free_wp(p);
        head = head->next;
        return true;
    }
    else
    {
        p = head->next;
        p_prev = head;
        while (p!= NULL)
        {
          if (p->NO != NO)
          {
            p = p->next;
            p_prev = p_prev->next;
          }
          else
          {
              p_prev->next = p->next;  
              free_wp(p);
              return true;
          }
          
        }
    }

    return false;
}

int list_watchpoint(void)
{
  WP *p;
  // bool success;

  for (p = head; p!= NULL; p = p->next)
  {
    assert(p->in_use == true);
    printf("%s \r\n" ,p->expr);
  }

  return 0;
}

bool scan_watchpoint_display(void)
{
  WP* wp;
  bool find_watchpoint = false;

  wp = scan_watchpoint();
  if (wp != NULL)
  {
    // printf("\n\nHit watchpoint %d at address 0x%08x, expr = %s\n", wp->NO, cpu.eip - instr_len, wp->expr);
    printf("old value = %#08x\nnew value = %#08x\n", wp->old_val, wp->new_val);
    wp->old_val = wp->new_val;
    find_watchpoint = true;
  }

  return find_watchpoint;
}