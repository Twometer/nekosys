#include <kernel/tty.h>
#include <stdio.h>

char scancode_map[128];

char map_char(unsigned int scancode) {
  if (scancode < 128)
    return scancode_map[scancode];
  else
    return '\0';
}

void new_scancode(unsigned int code, char c) {
  scancode_map[code] = c;
}

void kbd_initialize() {
  for (int i = 0; i < 128; i++)
    scancode_map[i] = '\0';

  new_scancode(2, '1');
  new_scancode(3, '2');
  new_scancode(4, '3');
  new_scancode(5, '4');
  new_scancode(6, '5');
  new_scancode(7, '6');
  new_scancode(8, '7');
  new_scancode(9, '8');
  new_scancode(10, '9');
  new_scancode(11, '0');
  //new_scancode(12, 'ß');
  //new_scancode(13, '´');
  new_scancode(14, '\b');
  new_scancode(15, '\t');
  new_scancode(16, 'q');
  new_scancode(17, 'w');
  new_scancode(18, 'e');
  new_scancode(19, 'r');
  new_scancode(20, 't');
  new_scancode(21, 'z');
  new_scancode(22, 'u');
  new_scancode(23, 'i');
  new_scancode(24, 'o');
  new_scancode(25, 'p');
  //new_scancode(26, 'ü');
  new_scancode(27, '*');
  new_scancode(28, '\n');
  new_scancode(30, 'a');
  new_scancode(31, 's');
  new_scancode(32, 'd');
  new_scancode(33, 'f');
  new_scancode(34, 'g');
  new_scancode(35, 'h');
  new_scancode(36, 'j');
  new_scancode(37, 'k');
  new_scancode(38, 'l');
  //new_scancode(39, 'ö');
  //new_scancode(40, 'ä');
  new_scancode(43, '#');
  new_scancode(86, '<');
  new_scancode(44, 'y');
  new_scancode(45, 'x');
  new_scancode(46, 'c');
  new_scancode(47, 'v');
  new_scancode(48, 'b');
  new_scancode(49, 'n');
  new_scancode(50, 'm');
  new_scancode(51, ',');
  new_scancode(52, '.');
  new_scancode(53, '-');
  new_scancode(57, ' ');
}

void kbd_handle_interrupt(unsigned int scancode) {
  if (scancode > 128)
    return;

  char chr = map_char(scancode);
  if (chr != 0)
    tty_write(&chr, 1);

  // printf("KEY: %d\n", scancode);

  // TODO correct keyboard handling
  /*if (scancode > 128) {
    printf("Release %d\n", scan_code - 128);
  } else {
    printf("Press %d\n", scan_code);
  }*/

}
