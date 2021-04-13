#include <stdio.h>
#include <nekosys.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <libgui/guiconnection.h>
#include <libgui/packets.h>
#include <nk/string.h>
#include <nk/inifile.h>
#include <nk/vector.h>
#include <png/lodepng.h>
#include <gfx/bitmap.h>
#include "Mouse.h"

using namespace Gui;

struct window_info
{
	nk::String title;
	int x, y, width, height;
	int fbuf_id;
	uint8_t *fbuf;
	Bitmap *bitmap;
};

Bitmap *framebuffer;
GuiConnection *connection;
nk::Vector<window_info> windows;

uint8_t *read_file(const char *path, size_t *size)
{
	FILE *fd = fopen(path, "r");
	if (!fd)
	{
		return nullptr;
	}

	fseek(fd, 0, SEEK_END);
	size_t len = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	uint8_t *data = new uint8_t[len];
	fread(data, 1, len, fd);
	fclose(fd);

	*size = len;

	return data;
}

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

			uint8_t *fbuf;
			auto bufid = shbuf_create(packet->width * packet->height * 3);
			shbuf_map(bufid, (void **)&fbuf);

			windows.Add({packet->title, packet->x, packet->y, packet->width, packet->height, bufid, fbuf});

			PWindowFbuf reply;
			reply.shbufId = bufid;
			connection->SendTo(ID_PWindowBuf, sizeof(PCreateWindow), &reply, packetData.source);
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
	framebuf_acquire(&framebuf);
	framebuffer = new Bitmap(framebuf.width, framebuf.height, framebuf.pitch, framebuf.buffer, PixelFormat::Bgr32);

	size_t iniFileSize;
	uint8_t *inifileData = read_file("/etc/sakura.ini", &iniFileSize);
	nk::IniFile config((char *)inifileData);
	auto conf = config.GetSection("Sakura");

	Bitmap wallpaper(conf->GetProperty("Wallpaper"));

	connection = new GuiConnection();
	int result = thread_create(receiver_thread);
	if (result < 0)
	{
		return 1;
	}
	else
	{
		spawnp(nullptr, conf->GetProperty("StartupApp").CStr(), 0, nullptr);

		Mouse mouse(framebuffer->width, framebuffer->height);

		/* compositor test */
		while (true)
		{
			framebuffer->Blit(wallpaper, Rectangle(0, 0, framebuffer->width, framebuffer->height));

			mouse.Update();
			framebuffer->SetPixel(mouse.GetPosX(), mouse.GetPosY(), {255, 255, 0});

			framebuf_flush_all();
			usleep(1000);
		}

		thread_join(result);
	}

	framebuf_release();
	delete connection;
	return 0;
}
