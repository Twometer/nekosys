#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

static size_t term_row;
static size_t term_column;
static size_t term_color;
static uint16_t* term_buffer = (uint16_t*) 0xB8000;

void tty_clear(void) {
	term_row = 0;
	term_column = 0;
	term_color = 0x0F00;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			tty_putentry(' ', term_color, x, y);
		}
	}
}

void tty_setcolor(uint8_t color) {
	term_color = color;
}

void tty_putentry(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	term_buffer[index] = color | c;
}

void tty_putchar(char c) {
	unsigned char uc = c;
	terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH || c == '\n') {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
}

void tty_write(const char *data, size_t size) {
	for (size_t i = 0; i < size; i++)
		tty_putchar(data[i]);
}

void tty_print(const char *data) {
	tty_write(data, strlen(data));
}
