#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <nekosys.h>

#include <nk/string.h>

#include "CommandParser.h"

int main(int argc, char **argv)
{
	nk::String cwd = "/";
	char *buf = new char[512];

	for (;;)
	{
		printf("neko:%s $ ", cwd.CStr()); // Prompt

		size_t read = readln(buf, 512); // Read
		buf[read] = 0x00;

		nk::String command(buf); // Parse
		if (command == "exit")
		{
			break;
		}

		auto parsedCommand = CommandParser::parse(command);	

		pid_t pid = 0; 
		if (spawnp(&pid, parsedCommand.file.CStr(), nullptr, nullptr)) // Execute
		{
			printf("nsh: error: File not found\n");
		}
		waitp(pid);
	}

	return 0;
}