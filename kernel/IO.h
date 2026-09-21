#include <stdint.h>

static inline void outb(uint16_t port, uint8_t data) {
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    asm volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static inline void io_wait() {
    outb(0x80, 0);
}

static inline void disable_pic() {
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
}
