#include <nekosys.h>
#include <libgui/application.h>
#include <libgui/packets.h>

using namespace Gui;

Application::Application()
{
}

void Application::Run()
{
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
                lastWindow->framebuffer = new Bitmap(lastWindow->width, lastWindow->height, fmt->GetBpp() * lastWindow->width, lastWindow->buffer, fmt);
            })

            CASE_PACKET(PRepaint, {})
        }
    }
}

void Application::Exit()
{
    exitRequested = true;
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