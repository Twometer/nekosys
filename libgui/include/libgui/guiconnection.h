#ifndef _GUI_CONNECTION_H
#define _GUI_CONNECTION_H

#include <stdint.h>
#include <sys/types.h>
#include "packets.h"

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

        template <typename T>
        void SendTo(const T &packet, pid_t dst)
        {
            auto packetId = PacketId<T>::value;
            SendTo(packetId, sizeof packet, (void *)&packet, dst);
        }

        template <typename T>
        void Send(const T &packet)
        {
            auto packetId = PacketId<T>::value;
            Send(packetId, sizeof packet, (void *)&packet);
        }

        void Close();

        PacketData Receive();
    };
};

#endif