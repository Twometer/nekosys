#include <stdio.h>
#include <nekosys.h>

int main(int argc, char **argv)
{
	MOUSEPACKET packet;
	while (1)
	{
		if (mouse_poll(&packet) == 0)
		{
			printf("%d %d %d\n", packet.dx, packet.dy, packet.buttons);
		}
	}

	return 0;
}
