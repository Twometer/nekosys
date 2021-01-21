#include <sys/syscall.h>

int main(int argc, char **argv)
{
	const char *data = "Hello world :3\n";
	syscall(SYS_PRINT, data);
	return 0;
}
