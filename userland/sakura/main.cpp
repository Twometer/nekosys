#include <stdio.h>
#include <nekosys.h>
#include <stdint.h>

int main(int argc, char **argv)
{
	FRAMEBUF buf;
	framebuf_acquire(&buf);
	((uint32_t*)(buf.buffer))[0] = 0xFFFFFFFF;
	framebuf_flush_all();
	
	framebuf_release();
	printf("Framebuffer: %d x %d\n", buf.width, buf.height);
	return 0;
}
