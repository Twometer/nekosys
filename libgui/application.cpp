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
        auto packet = connection.Receive();
        if (packet.packetId == ID_PWindowBuf)
        {
            auto shbuf = ((PWindowFbuf *)packet.data)->shbufId;
            lastWindow->shbufId = shbuf;
            shbuf_map(shbuf, (void **)&lastWindow->framebuffer);

            lastWindow->framebuffer[0] = 255;
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
    connection.Send(ID_PCreateWindow, sizeof(packet), &packet);
    lastWindow = &win;
}