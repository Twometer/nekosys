/* nekosys Kernel entry point */

void nkmain() {
    const short color = 0x0F00;
    const char* hello = "nekosys Kernel 0.01";
    short* vga = (short*)0xb8000;
    for (int i = 0; i < 19;++i)
        vga[i+80] = color | hello[i];
}