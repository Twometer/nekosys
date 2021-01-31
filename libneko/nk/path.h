#ifndef NK_PATH
#define NK_PATH

#include "vector.h"
#include "string.h"

namespace nk
{

    class Path
    {
    private:
        bool isAbsolute = false;

        Vector<String> *parts = new Vector<String>();

        Path() {}

    public:
        Path(const char *path) : Path(String(path))
        {
        }

        bool IsAbsolute()
        {
            return isAbsolute;
        }

        Path(const String &path)
        {
            parts->EnsureCapacity(8);
            String temp;
            for (size_t i = 0; i < path.Length(); i++)
            {
                if (path[i] == '/')
                {
                    if (temp.Length() > 0)
                    {
                        parts->Add(temp);
                    }
                    else if (parts->Size() == 0)
                    {
                        isAbsolute = true;
                    }
                    temp = String();
                }
                else
                {
                    temp += path[i];
                }
            }

            if (temp.Length() > 0)
                parts->Add(temp);
        }

        ~Path()
        {
            delete parts;
        }

        static Path Join(const Path &a, const Path &b)
        {
            Path result;
            result.isAbsolute = true;
            for (size_t i = 0; i < a.GetParts().Size(); i++)
                result.parts->Add(a.GetParts().At(i));

            for (size_t i = 0; i < b.GetParts().Size(); i++)
                result.parts->Add(b.GetParts().At(i));

            return result;
        }

        static Path *Resolve(const Path &base, const Path &path)
        {
            // FIXME: This is not good
            if (path.parts->Size() == 0)
                return new Path(base.ToString());
            if (path.isAbsolute)
                return new Path(path.ToString());

            Path *result = new Path();
            result->isAbsolute = true;
            size_t j = 0;
            for (size_t i = 0; i < base.GetParts().Size(); i++, j++)
                result->parts->Add(base.GetParts().At(i));

            for (size_t i = 0; i < path.GetParts().Size(); i++, j++)
            {
                auto part = path.parts->At(i);
                if (part == ".")
                    continue;

                if (part == ".." && result->parts->Size() > 0)
                {

                    result->parts->Remove(j);
                    j--;
                    continue;
                }
                result->parts->Add(part);
            }

            return result;
        }

        String ToString() const
        {
            String result;
            if (isAbsolute)
                result += "/";
            for (size_t i = 0; i < parts->Size(); i++)
            {
                auto &part = parts->At(i);
                result += part + "/";
            }
            return result;
        }

        const Vector<String> &GetParts() const
        {
            return *parts;
        }

        const String &GetFilename() const
        {
            return parts->At(parts->Size() - 1);
        }

        Path GetDirectoryPath() const
        {
            Path other;
            for (size_t i = 0; i < parts->Size() - 1; i++)
            {
                other.parts->Add(parts->At(i));
            }
            return other;
        }

        bool IsRoot() const
        {
            return parts->Size() == 0;
        }
    };

} // namespace nk

#endif