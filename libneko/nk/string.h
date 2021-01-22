#ifndef _NK_STRING_H
#define _NK_STRING_H

#include <string.h>
#include <stddef.h>

namespace nk
{

    class String
    {
    private:
        const char *cstring = nullptr;
        size_t length = 0;

    private:
        String(const char *cstring, size_t len)
        {
            length = len;
            auto buf = new char[len + 1];
            memcpy(buf, cstring, length);
            buf[len] = 0;
            this->cstring = buf;
        }

    public:
        String() : cstring(nullptr), length(0) {}

        String(const char *cstring)
        {
            length = strlen(cstring);
            auto buf = new char[length + 1];
            memcpy(buf, cstring, length + 1);
            this->cstring = buf;
        }

        String(const String &other)
        {
            *this = other;
        }

        ~String()
        {
            delete cstring;
        }

        const char *CStr() const
        {
            return cstring;
        }

        bool StartsWith(const String &other) const
        {
            if (other.length > length)
                return false;

            for (size_t i = 0; i < other.length; i++)
                if (cstring[i] != other.cstring[i])
                    return false;

            return true;
        }

        char operator[](size_t index) const
        {
            if (index >= length) // FIXME: Should not fail silently
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

        String Append(const char other) const
        {
            auto newlen = length + 2;
            char tmpbuf[newlen];
            if (cstring != nullptr)
                memcpy(tmpbuf, cstring, length);
            tmpbuf[newlen - 2] = other;
            tmpbuf[newlen - 1] = 0;
            return String(tmpbuf);
        }

        String Append(const String &other) const
        {
            auto newlen = length + other.length + 1;
            char tmpbuf[newlen];
            if (cstring != nullptr)
                memcpy(tmpbuf, cstring, length);
            memcpy(tmpbuf + length, other.cstring, other.length);
            tmpbuf[newlen - 1] = 0;
            return String(tmpbuf);
        }

        String Substring(size_t offset) const
        {
            return String(&cstring[offset], length - offset);
        }

        String &operator=(const String &other)
        {
            if (this != &other)
            {
                delete cstring;

                length = other.length;
                auto str = new char[other.length + 1];
                memcpy(str, other.cstring, length + 1);
                cstring = str;
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