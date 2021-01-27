#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

#include "Command.h"

class CommandParser
{
public:
    static Command parse(const nk::String &input);
};

#endif