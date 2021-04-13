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

using namespace Gui;

struct window_info
{
	nk::String title;
	int x, y, width, height;
	int fbuf_id;
	uint8_t *fbuf;
};

FRAMEBUF framebuf;
GuiConnection *connection;
nk::Vector<window_info> windows;

int mouse_x = 0;
int mouse_y = 0;

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
	framebuf_acquire(&framebuf);

	size_t iniFileSize;
	uint8_t *inifileData = read_file("/etc/sakura.ini", &iniFileSize);
	nk::IniFile config((char *)inifileData);
	auto conf = config.GetSection("Sakura");

	size_t wallpaperSize;
	uint8_t *wallpaperPng = read_file(conf->GetProperty("Wallpaper").CStr(), &wallpaperSize);
	printf("wallpaper: %s\n", conf->GetProperty("Wallpaper").CStr());

	unsigned char *image;
	unsigned int width, height;

	unsigned error = lodepng_decode_memory(&image, &width, &height, wallpaperPng, wallpaperSize, LodePNGColorType::LCT_RGB, 8);

	if (error)
	{
		printf("decoder error: %s\n", lodepng_error_text(error));
		return 1;
	}

	connection = new GuiConnection();
	int result = thread_create(receiver_thread);
	if (result < 0)
	{
		return 1;
	}
	else
	{
		printf("[info] sakura window server started.\n");

		spawnp(nullptr, conf->GetProperty("StartupApp").CStr(), 0, nullptr);
		printf("[info] demo application started.\n");

		MOUSEPACKET mouse{};

		/* compositor test */
		while (true)
		{
			size_t dOffset = 0;
			size_t sOffset = 0;
			for (size_t row = 0; row < height; row++)
			{
				for (size_t col = 0; col < width; col++)
				{
					framebuf.buffer[dOffset + col * 4 + 2] = image[sOffset + col * 3 + 0];
					framebuf.buffer[dOffset + col * 4 + 1] = image[sOffset + col * 3 + 1];
					framebuf.buffer[dOffset + col * 4 + 0] = image[sOffset + col * 3 + 2];
				}
				dOffset += framebuf.pitch;
				sOffset += width * 3;
			}

			for (size_t i = 0; i < windows.Size(); i++)
			{
				auto &window = windows.At(i);

				sOffset = 0;
				dOffset = window.y * framebuf.pitch + window.x * framebuf.pixelStride;

				for (size_t row = 0; row < window.height; row++)
				{
					for (size_t col = 0; col < window.width; col++)
					{
						framebuf.buffer[dOffset + col * 4 + 2] = window.fbuf[sOffset + col * 3 + 0];
						framebuf.buffer[dOffset + col * 4 + 1] = window.fbuf[sOffset + col * 3 + 1];
						framebuf.buffer[dOffset + col * 4 + 0] = window.fbuf[sOffset + col * 3 + 2];
					}
					dOffset += framebuf.pitch;
					sOffset += window.width * 3;
				}
			}

			while (mouse_poll(&mouse) == 0)
			{
				float scale = mouse.dx + mouse.dy;

				mouse_x += mouse.dx;
				mouse_y -= mouse.dy;
				if (mouse_x < 0)
					mouse_x = 0;
				if (mouse_y < 0)
					mouse_y = 0;
				if (mouse_x > width - 1)
					mouse_x = width - 1;
				if (mouse_y > height - 1)
					mouse_y = height - 1;
			}

			

			size_t baseidx = mouse_y * framebuf.pitch + mouse_x * 4;
			framebuf.buffer[baseidx] = 0xff;
			framebuf.buffer[baseidx + 1] = 0x00;
			framebuf.buffer[baseidx + 2] = 0x00;

			framebuf_flush_all();
			usleep(1000);
		}

		thread_join(result);
	}

	framebuf_release();
	delete connection;
	return 0;
}
