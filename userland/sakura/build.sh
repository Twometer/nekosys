#!/bin/bash
neko-gcc -std=gnu++11 -fno-exceptions Mouse.cpp Compositor.cpp DirtyRegionManager.cpp main.cpp -lgfx -lgui -lpng -O3 -o ../../sysroot/bin/sakura