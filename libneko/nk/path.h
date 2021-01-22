#ifndef NK_PATH
#define NK_PATH

#include "vector.h"
#include "string.h"

namespace nk
{

    class Path
    {
    private:
        Vector<String> *parts = new Vector<String>();

        Path() {}

    public:
        Path(const String &path)
        {
            String temp;
            for (size_t i = 0; i < path.Length(); i++)
            {
                if (path[i] == '/')
                {
                    if (temp.Length() > 0)
                        parts->Add(temp);
                    temp = String();
                }
                else
                {
                    temp = temp.Append(path[i]);
                }
            }

            if (temp.Length() > 0)
                parts->Add(temp);
        }

        ~Path()
        {
            delete parts;
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