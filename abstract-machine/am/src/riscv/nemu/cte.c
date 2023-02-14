#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

static void display_context(Context *c)
{
  printf("----cte context display start---- \r\n");
  for(uint32_t i=0; i<sizeof(regs)/sizeof(regs[0]); i++)
  {
      if (i % 4  == 0)
         printf("\r\n");
      printf("%5s = 0x%016x,", regs[i], c->gpr[i]);      
  }
  printf("\r\n");
  printf("mcause is 0x%x, mstatus is 0x%x, mepc is 0x%x", c->mcause, c->mstatus, c->mepc);
  printf("\r\n");
  printf("----cte context display end---- \r\n");
}


static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case 11 :  ev.event = EVENT_YIELD; break;
      default: ev.event = EVENT_ERROR; break;
    }
    display_context(c);
    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  return NULL;
}

void yield() {
  asm volatile("li a7, 11; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
