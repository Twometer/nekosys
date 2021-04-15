#include <nekosys.h>
#include <gfx/gfx.h>
#include <libgui/application.h>
#include <libgui/packets.h>

using namespace Gui;

Application::Application()
{
}

void Application::Run()
{
    gfx_initialize();
    while (!exitRequested)
    {
        auto rawPacket = connection.Receive();

        SWITCH_PACKET
        {
            CASE_PACKET(PWindowCreated, {
                auto shbufId = packet->shbufId;
                shbuf_map(shbufId, (void **)&lastWindow->buffer);

                auto fmt = PixelFormat::Rgb24;
                lastWindow->shbufId = shbufId;
                lastWindow->framebuffer = new Bitmap(lastWindow->width, lastWindow->height, lastWindow->width * fmt->GetBpp(), lastWindow->buffer, fmt);
                lastWindow->id = packet->windowId;
                lastWindow->Draw();
                windows.Add(lastWindow);
            })

            CASE_PACKET(PRepaint, {
                for (size_t i = 0; i < windows.Size(); i++)
                {
                    auto window = windows.At(i);
                    if (window->id == packet->windowId)
                    {
                        window->Draw();
                        break;
                    }
                }
            })
        }
    }
}

void Application::Exit()
{
    exitRequested = true;
    connection.Close();
}

void Application::OpenWindow(Window &win)
{
     PCreateWindow packet{};
    packet.width = win.width;
    packet.height = win.height;
    packet.x = win.x;
    packet.y = win.y;
    memcpy(packet.title, win.title.CStr(), win.title.Length());
    connection.Send(packet);
    lastWindow = &win;
}