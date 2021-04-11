#include <nekosys.h>
#include <string.h>
#include <libgui/guiconnection.h>

using namespace Gui;

GuiConnection::GuiConnection()
{
    recvBuffer = new uint8_t[16384];
    pipe = pipe_open("sakura");
}

GuiConnection::~GuiConnection()
{
    delete[] recvBuffer;
    pipe_close(pipe);
}

void GuiConnection::Send(uint8_t packetId, size_t size, void *data)
{
    SendTo(packetId, size, data, 0);
}

void GuiConnection::SendTo(uint8_t packetId, size_t size, void *data, pid_t dst)
{
    uint8_t *packet = new uint8_t[size + 1];
    packet[0] = packetId;
    memcpy(packet + 1, data, size);
    pipe_send(pipe, dst, size + 1, packet);
    delete[] packet;
}

PacketData GuiConnection::Receive()
{
    PacketData data{};
    int size = pipe_recv(pipe, &data.source, 16384, recvBuffer) - 1;
    data.packetId = recvBuffer[0];
    data.size = size;
    data.data = recvBuffer + 1;
    return data;
}