void cpu_halt() {
  asm("cli");
  
  for (;;)
    asm("hlt");
}
