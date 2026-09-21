#include <stdint.h>

int64_t pow(int64_t value, uint8_t n) {
    int64_t current = 1;
    for (int i = 0; i < n; i++) {
        current *= value;
    }
    return current;
}
