#ifndef _NK_STRING_H
#define _NK_STRING_H

#include <string.h>
#include <stddef.h>

namespace nk
{

    class String
    {
    private:
        size_t length;
        const char *cstring;
        bool ownsBuf = false;

    public:
        String() : cstring(nullptr), length(0) {}

        String(const char *cstring) : cstring(cstring), length(strlen(cstring)) {}

        String(const String &other)
        {
            *this = other;
        }

        ~String()
        {
            if (ownsBuf)
                delete cstring;
        }

        const char *CStr() const
        {
            return cstring;
        }

        bool StartsWith(const String &other) const
        {
            if (other.length < length)
                return false;

            for (size_t i = 0; i < other.length; i++)
                if (cstring[i] != other.cstring[i])
                    return false;

            return true;
        }

        char operator[](size_t index) const
        {
            if (index >= length) // todo exception
                return 0;
            return cstring[index];
        }

        bool operator==(const String &other) const
        {
            if (length != other.length)
                return false;

            return EqualsImpl(other.CStr());
        }

        bool operator==(const char *other) const
        {
            if (length != strlen(other))
                return false;

            return EqualsImpl(other);
        }

        String &operator=(const String &other)
        {
            if (this != &other)
            {
                if (ownsBuf)
                {
                    delete cstring;
                }

                length = other.length;
                auto str = new char[other.length + 1];
                memcpy(str, other.cstring, length + 1);
                cstring = str;
                ownsBuf = true;
            }
            return *this;
        }

        size_t Length() const
        {
            return length;
        }

    private:
        bool EqualsImpl(const char *other) const
        {
            for (size_t i = 0; i < length; i++)
            {
                if (cstring[i] != other[i])
                    return false;
            }
            return true;
        }
    };

} // namespace nk

#endif