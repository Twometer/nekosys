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
        connection.Receive();
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
    memcpy(packet.title, win.title.CStr(), win.title.Length());
    connection.Send(ID_PCreateWindow, sizeof(packet), &packet);
}