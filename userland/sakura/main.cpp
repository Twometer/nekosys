#include <stdio.h>
#include <nekosys.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <libgui/guiconnection.h>
#include <libgui/packets.h>

using namespace Gui;

GuiConnection *connection;

void receiver_thread()
{
	while (true)
	{
		auto packetData = connection->Receive();

		switch (packetData.packetId)
		{
		case ID_PCreateWindow:
		{
			auto packet = (PCreateWindow *)packetData.data;
			printf("Creating window '%s' with size %dx%d\n", packet->title, packet->width, packet->height);
			break;
		}
		default:
			break;
		}
	}

	thread_die(0);
}

int main(int argc, char **argv)
{
	FRAMEBUF framebuf;
	//framebuf_acquire(&framebuf);

	connection = new GuiConnection();
	int result = thread_create(receiver_thread);
	if (result < 0)
	{
		return 1;
	}
	else
	{
		printf("[info] sakura window server started.");

		thread_join(result);
	}

	//framebuf_release();
	delete connection;
	return 0;
}
