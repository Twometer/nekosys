#ifndef _PACKETS_H
#define _PACKETS_H

#include <stdint.h>

#define ASSIGN_PACKET_ID(type, id)   \
    template <>                      \
    struct PacketId<type>            \
    {                                \
        static const int value = id; \
    };

#define PACKET_ID_OF(type) (PacketId<type>::value)

#define SWITCH_PACKET \
    switch (rawPacket.packetId)

#define CASE_PACKET(type, code)               \
    case PACKET_ID_OF(type):                  \
    {                                         \
        auto packet = (type *)rawPacket.data; \
        code;                                 \
        break;                                \
    }

namespace Gui
{

    template <typename T>
    struct PacketId
    {
        static const uint8_t value = 0;
    };

    struct PCreateWindow
    {
        int x;
        int y;
        int width;
        int height;
        char title[128];
    };
    ASSIGN_PACKET_ID(PCreateWindow, 1);

    struct PWindowCreated
    {
        int windowId;
        int shbufId;
    };
    ASSIGN_PACKET_ID(PWindowCreated, 2);

    struct PRepaint
    {
        int windowId;
    };
    ASSIGN_PACKET_ID(PRepaint, 3);

    struct PInvalidateRegion
    {
        int windowId;
        int x;
        int y;
        int w;
        int h;
    };
    ASSIGN_PACKET_ID(PInvalidateRegion, 4);

};

#endif