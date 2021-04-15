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
#include <gfx/gfx.h>
#include <gfx/bitmap.h>
#include <gfx/font.h>
#include "Mouse.h"
#include "Compositor.h"

using namespace Gui;

Bitmap *framebuffer;
GuiConnection *connection;
Compositor *compositor;

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
		auto rawPacket = connection->Receive();
		SWITCH_PACKET
		{
			CASE_PACKET(PCreateWindow, {
				printf("Creating window '%s' with size %dx%d\n", packet->title, packet->width, packet->height);

				// Create shared buffer for framebuffer
				uint8_t *shbuf;
				auto shbufId = shbuf_create(packet->width * packet->height * 3);
				shbuf_map(shbufId, (void **)&shbuf);

				// Create window
				auto bitmap = new Bitmap(packet->width, packet->height, packet->width * 3, shbuf, PixelFormat::Rgb24);
				auto window = new WindowInfo(packet->title, packet->x, packet->y, packet->width, packet->height, shbufId, bitmap);
				compositor->AddWindow(window);

				// Send reply
				PWindowCreated reply;
				reply.shbufId = shbufId;
				connection->SendTo(reply, rawPacket.source);
			})
		}
	}

	thread_die(0);
}

int main(int argc, char **argv)
{
	gfx_initialize();

	FRAMEBUF framebuf;
	framebuf_acquire(&framebuf);
	framebuffer = new Bitmap(framebuf.width, framebuf.height, framebuf.pitch, framebuf.buffer, PixelFormat::Bgr32);

	// Load config
	size_t iniFileSize;
	uint8_t *inifileData = read_file("/etc/sakura.ini", &iniFileSize);
	nk::IniFile config((char *)inifileData);
	auto conf = config.GetSection("Sakura");

	// Init renderer
	Bitmap wallpaper(conf->GetProperty("Wallpaper"));
	compositor = new Compositor(framebuffer, &wallpaper);
	connection = new GuiConnection();

	// Create receiever thread
	int threadId = thread_create(receiver_thread);
	if (threadId < 0)
	{
		return 1;
	}

	// Autostart
	spawnp(nullptr, conf->GetProperty("StartupApp").CStr(), 0, nullptr);

	// Render loop
	while (true)
	{
		auto rect = compositor->RenderFrame();
		auto size = rect.size();
		if (size.width != 0 && size.height != 0)
		{
			framebuf_flush(rect.x0, rect.y0, size.width, size.height);
		}

		usleep(1000);
	}

	// Shutdown (..?)
	connection->Close();
	thread_join(threadId);
	framebuf_release();

	delete framebuffer;
	delete compositor;
	delete connection;
	return 0;
}
