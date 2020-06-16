int line = 0;
short *vga_mem = (short*)0xb8000;

void println(const short color, const char* data, int len) {
	for (int i = 0; i < len; i++)
		vga_mem[i + (line * 80)] = color | data[i];
	line++;
}

void newln() {
	line++;
}

/* nekosys Kernel entry point */
void nkmain() {
	println(0x0D00, "nekosys Kernel 0.01", 19);
	println(0x0700, "(c) 2020 Twometer Applications", 30);
	newln();
	println(0x0F00, "Booting...", 10);
}
