#ifndef _PACKETS_H
#define _PACKETS_H

#include <stdint.h>

#define ID_PCreateWindow 1
#define ID_PWindowBuf 2

namespace Gui
{

    struct PCreateWindow
    {
        int x;
        int y;
        int width;
        int height;
        char title[128];
    };

    struct PWindowFbuf
    {
        int shbufId;
    };

};

#endif