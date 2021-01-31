#ifndef _NK_STRING_H
#define _NK_STRING_H

#include <string.h>
#include <stddef.h>
#include <stdio.h>

namespace nk
{

    class String
    {
    private:
        const char *cstring = nullptr;
        size_t length = 0;

    private:
        void LoadFrom(const char *oldBuf, size_t numChars)
        {
            delete[] cstring;

            char *newBuf = new char[numChars + 1];
            if (oldBuf != nullptr)
            {
                memcpy(newBuf, oldBuf, numChars);
            }
            newBuf[numChars] = 0x00;

            this->cstring = newBuf;
            this->length = numChars;
        }

    public:
        String()
        {
            LoadFrom(nullptr, 0);
        }

        String(const char *buf)
        {
            LoadFrom(buf, strlen(buf));
        }

        String(const char *buf, size_t length)
        {
            LoadFrom(buf, length);
        }

        String(const String &other)
        {
            LoadFrom(other.cstring, other.length);
        }

        ~String()
        {
            delete[] cstring;
        }

        const char *CStr() const
        {
            return cstring;
        }

        bool Empty() const
        {
            return length == 0;
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

            return *this == other.CStr();
        }

        bool operator==(const char *other) const
        {
            if (length != strlen(other))
                return false;

            for (size_t i = 0; i < length; i++)
                if (cstring[i] != other[i])
                    return false;

            return true;
        }

        String operator+(const char other) const
        {
            return Append(other);
        }

        String operator+(const String &other) const
        {
            return Append(other);
        }

        String Append(const char other) const
        {
            auto newLength = length + 1;
            char newBuf[newLength + 1];

            if (cstring != nullptr)
                memcpy(newBuf, cstring, length);

            newBuf[newLength - 1] = other;
            newBuf[newLength] = 0;
            return String(newBuf);
        }

        String Append(const String &other) const
        {
            auto newLength = length + other.length;
            char newBuf[newLength + 1];

            if (cstring != nullptr)
                memcpy(newBuf, cstring, length);

            if (other.cstring != nullptr)
                memcpy(newBuf + length, other.cstring, other.length);

            newBuf[newLength] = 0;
            return String(newBuf);
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

        String Substring(size_t offset) const
        {
            return String(&cstring[offset], length - offset);
        }

        String &operator+=(char chr)
        {
            return *this = *this + chr;
        }

        String &operator+=(const String &other)
        {
            return *this = *this + other;
        }

        String &operator=(const String &other)
        {
            if (this != &other)
            {
                LoadFrom(other.cstring, other.length);
            }
            return *this;
        }

        size_t Length() const
        {
            return length;
        }

        /* C++ iterator stuff */
        char front() const
        {
            return cstring[0];
        }

        char back() const
        {
            return cstring[length - 1];
        }

        const char *begin() const
        {
            return &cstring[0];
        }

        const char *end() const
        {
            return &cstring[length];
        }
    };

} // namespace nk

#endif