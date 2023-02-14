#include <common.h>

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_YIELD :
    printf("system call , event id is [YIELD] \r\n");
    break;
    // case EVENT_SYSCALL:
    // break;
    // case EVENT_PAGEFAULT:
    // break;
    // case EVENT_ERROR:
    // break;
    // case EVENT_IRQ_TIMER:
    // break;
    // case EVENT_IRQ_IODEV:
    // break;

    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
