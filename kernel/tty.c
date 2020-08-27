#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>
#include <kernel/vga.h>
#include <kernel/io.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

static size_t term_row = 0;
static size_t term_column = 0;
static size_t term_color = VGA_COLOR_WHITE;
static uint16_t* term_buffer = (uint16_t*) 0xB8000;

void tty_clear(void) {
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			tty_putentry(' ', term_color, x, y);
		}
	}
	tty_set_cursor_pos(0, 0);
}

void tty_setcolor(uint8_t color) {
	term_color = color;
}

void tty_putentry(char c, uint8_t color, size_t x, size_t y) {
	term_buffer[x + (y * VGA_WIDTH)] = (color << 8) | (unsigned char)c;
}

void tty_putchar(char c) {
	if (c == '\b') {
		if (term_column > 0){
			tty_set_cursor_pos(term_column - 1, term_row);
			tty_putentry(' ', term_color, term_column, term_row);
		}
		return;
	}

	bool is_newline = (c == '\n' || c == '\r');

	if (!is_newline)
		tty_putentry(c, term_color, term_column, term_row);

	if (++term_column == VGA_WIDTH || is_newline) {
		term_column = 0;
		if (++term_row == VGA_HEIGHT)
		{
			term_row--;
			tty_scroll();
		}
	}
	tty_set_cursor_pos(term_column, term_row);
}

void tty_write(const char *data, size_t size) {
	for (size_t i = 0; i < size; i++)
		tty_putchar(data[i]);
}

void tty_print(const char *data) {
	tty_write(data, strlen(data));
}

void tty_scroll() {
	// Discard first line
	size_t new_buf_size = 2 * (VGA_HEIGHT - 1) * VGA_WIDTH;
	memcpy(term_buffer, term_buffer + VGA_WIDTH, new_buf_size);

	// Clear last line
	for (size_t x = 0; x < VGA_WIDTH; x++) {
		tty_putentry(' ', term_color, x, term_row);
	}
}

void tty_set_cursor_pos(size_t x, size_t y) {
	term_column = x;
	term_row = y;
	uint16_t pos = y * VGA_WIDTH + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}
