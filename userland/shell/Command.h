#ifndef COMMAND_H
#define COMMAND_H

#include <nk/string.h>
#include <nk/vector.h>

struct Command
{
    nk::String file{};
    nk::Vector<nk::String> params{};
    bool background{false};
};

#endif