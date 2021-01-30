#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <nekosys.h>

#include <nk/string.h>

#include "CommandParser.h"

int main(int argc, char **argv)
{
	char cwd[PATH_MAX];
	char *buf = new char[512];

	for (;;)
	{
		getcwd(cwd, PATH_MAX);
		printf("neko:%s $ ", cwd); // Prompt

		size_t read = readln(buf, 512); // Read
		buf[read] = 0x00;

		nk::String command(buf); // Parse
		auto parsedCommand = CommandParser::parse(command);

		if (command == "exit")
		{
			break;
		}
		else if (parsedCommand.file == "cd")
		{
			if (chdir(parsedCommand.params[0].CStr()))
			{
				printf("nsh: cd: No such directory\n");
			}
			continue;
		}

		pid_t pid = 0;
		if (spawnp(&pid, parsedCommand.file.CStr(), nullptr, nullptr)) // Execute
		{
			printf("nsh: error: File not found\n");
		}
		waitp(pid);
	}

	return 0;
}