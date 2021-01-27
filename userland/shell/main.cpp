#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <nekosys.h>

int main(int argc, char **argv)
{
	char *buf = new char[512];

	for (;;)
	{
		printf("neko:/ $ "); // Prompt

		size_t read = readln(buf, 512); // Command
		buf[read] = 0x00;

		if (streq(buf, "exit") == 0)
		{
			break;
		}

		pid_t pid = 0;
		if (spawnp(&pid, buf, nullptr, nullptr))
		{
			printf("nsh: error: Cannot spawn\n");
		}
		waitp(pid);
	}

	return 0;
}