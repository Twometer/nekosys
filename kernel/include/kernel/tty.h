#ifndef _TTY_H
#define _TTY_H

#include <stddef.h>
#include <stdint.h>

void tty_clear(void);
void tty_setcolor(uint8_t color);
void tty_putentry(char c, uint8_t color, size_t x, size_t y);
void tty_putchar(char c);
void tty_scroll();
void tty_write(const char* data, size_t size);
void tty_print(const char* string);

#endif
