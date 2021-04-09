#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <nekosys.h>
#include <signal.h>

#include <nk/string.h>

#include "CommandParser.h"

void start_process(const Command &cmd)
{
	pid_t pid = 0;

	const char **args = new const char *[cmd.params.Size()];
	for (size_t i = 0; i < cmd.params.Size(); i++)
		args[i] = cmd.params[i].CStr();

	if (spawnp(&pid, cmd.file.CStr(), cmd.params.Size(), args))
	{
		printf("nsh: error: Could not start\n");
	}
	else if (!cmd.background)
	{
		int retcode = waitp(pid);
		if (retcode > RCSIGBASE)
		{
			int signal = retcode - RCSIGBASE;
			switch (signal)
			{
			case SIGSEGV:
				printf("Segmentation fault\n");
				break;
			case SIGABRT:
				printf("Aborted\n");
				break;
			case SIGILL:
				printf("Illegal instruction\n");
				break;
			case SIGFPE:
				printf("Arithmetic error\n");
				break;
			default:
				break;
			}
		}
	}
}

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

		if (command == "exit") // Handle
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
		else if (command.Length() > 0)
		{
			start_process(parsedCommand);
		}
	}

	return 0;
}