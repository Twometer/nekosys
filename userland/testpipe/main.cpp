#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <nekosys.h>

int pipe = 0;

void test_thread()
{
	printf("hello from the thread\n");
	pid_t src;
	uint8_t *buf = new uint8_t[128];

	printf("listening to the pipe\n");

	while (true)
	{
		int result = pipe_recv(pipe, &src, 128, buf);
		if (result == -EPIPE)
		{
			printf("pipe broken\n");
			break;
		}
		else if (result == -ENOENT)
		{
			printf("pipe does not exist\n");
			break;
		}

		printf("packet data: %s\n", buf);
	}

	delete[] buf;
	thread_die(0);
}

int main(int argc, char **argv)
{
	printf("[ Pipe Test ]\n");
	pipe = pipe_open("testpipe");
	thread_create(test_thread);

	sleep(2);

	printf("Sending packet!\n");
	auto test = "test packet! :3";
	pipe_send(pipe, 0, strlen(test) + 1, (uint8_t *)test);

	sleep(5);

	printf("Destryoing pipe!\n");
	pipe_close(pipe);

	printf("Shutting down!\n");
	sleep(2);

	printf("bye ~\n");
	return 0;
}
