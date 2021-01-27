#include "CommandParser.h"

Command CommandParser::parse(const nk::String &input) {
    Command cmd;

    bool filenameFinished = false;
    bool inQuotes = false;
    nk::String parambuf;

    for (auto chr : input) {
        if (chr == input.back() && chr == '&') {
            cmd.background = true;
            break;
        }

        if (!filenameFinished) {
            if (chr == ' ')
                filenameFinished = true;
            else
                cmd.file += chr;
        } else {
            if (chr == '"') {
                inQuotes = !inQuotes;
                continue;
            }

            if (!inQuotes && chr == ' ' && !parambuf.Empty()) {
                cmd.params.Add(parambuf);
                parambuf = "";
            } else {
                parambuf += chr;
            }
        }
    }
    if (!parambuf.Empty())
        cmd.params.Add(parambuf);

    return cmd;
}
