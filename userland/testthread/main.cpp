#include <stdio.h>
#include <errno.h>
#include <nekosys.h>
#include <unistd.h>

void test_thread()
{
	printf("hello from thread 2\n");

	for (int i = 0; i < 2; i++)
	{
		printf("[2] Sleep seconds %d / 10\n", i);
		sleep(1);
	}

	printf("thread 2 stopped\n");

	thread_die(0);
	printf("shouldnt see this message\n");
}

int main(int argc, char **argv)
{
	printf("hello from thread 1\n");

	thread_create(test_thread);

	for (int i = 0; i < 3; i++)
	{
		printf("[1] Sleep seconds %d / 11\n", i);
		sleep(1);
	}

	return 0;
}
