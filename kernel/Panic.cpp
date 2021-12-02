#include "Panic.hpp"

void panic(const char *tag, const char *msg) {
    asm("cli");
    asm("hlt");
}
