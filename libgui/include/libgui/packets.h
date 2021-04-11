#ifndef _PACKETS_H
#define _PACKETS_H

#include <stdint.h>

#define ID_PCreateWindow 1

namespace Gui
{

    struct PCreateWindow
    {
        int width;
        int height;
        char title[128];
    };

};

#endif