#include <stdio.h>
#include <nekosys.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

void receiver_thread()
{
	sleep(5);

	thread_die(0);
}

int main(int argc, char **argv)
{
	FRAMEBUF framebuf;
	framebuf_acquire(&framebuf);

	int result = thread_create(receiver_thread);
	if (result < 0)
	{
		return 1;
	}
	else
	{
		thread_join(result);
	}

	framebuf_release();
	return 0;
}
