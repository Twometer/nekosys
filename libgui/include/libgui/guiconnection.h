#ifndef _GUI_CONNECTION_H
#define _GUI_CONNECTION_H

#include <stdint.h>
#include <sys/types.h>

namespace Gui
{
    struct PacketData
    {
        uint8_t packetId;
        pid_t source;
        size_t size;
        void *data;
    };

    class GuiConnection
    {
    private:
        int pipe;

        uint8_t *recvBuffer;

    public:
        GuiConnection();

        ~GuiConnection();

        void SendTo(uint8_t packetId, size_t size, void *data, pid_t dst);

        void Send(uint8_t packetId, size_t size, void *data);

        PacketData Receive();
    };
};

#endif