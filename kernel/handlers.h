#include <stdint.h>
void interrupt_dispatcher();
extern void commonstub();
extern uint64_t isr[256];

