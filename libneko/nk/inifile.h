#ifndef _NK_INI_FILE_H
#define _NK_INI_FILE_H

#include "vector.h"
#include "string.h"

namespace nk
{
    // FIXME: A HashMap would be a nice data structure here. We should implement one.

    struct IniProperty
    {
        nk::String key;
        nk::String value;
    };

    class IniSection
    {
    private:
        nk::String name;
        nk::Vector<IniProperty *> *properties;

    public:
        IniSection()
            : name(""), properties(new nk::Vector<IniProperty *>()) {}

        ~IniSection()
        {
            for (size_t i = 0; i < properties->Size(); i++)
                delete properties->At(i);
            delete properties;
        }

        const nk::String &GetProperty(const nk::String &key)
        {
            for (size_t i = 0; i < properties->Size(); i++)
                if (properties->At(i)->key == key)
                    return properties->At(i)->value;
            return "";
        }

        nk::Vector<IniProperty *> *GetProperties()
        {
            return properties;
        }

        nk::String &GetName()
        {
            return name;
        }
    };

    class IniFile
    {
    private:
        nk::Vector<IniSection *> *sections;

        enum ParserState
        {
            Comment,
            Text,
            SectionName,
            PropertyValue,
            Drop
        };

    public:
        IniFile(const nk::String &buf)
            : sections(new nk::Vector<IniSection *>())
        {
            IniSection *currentSection = nullptr;
            IniProperty *currentProperty = nullptr;

            ParserState state = Text;

            for (size_t i = 0; i < buf.Length(); i++)
            {
                auto chr = buf[i];
                if (chr == '\r')
                    continue;

                if (chr == '\n')
                {
                    if (currentProperty != nullptr && currentProperty->key.Length() > 0 && currentSection != nullptr)
                    {
                        currentSection->GetProperties()->Add(currentProperty);
                        currentProperty = new IniProperty();
                    }
                    state = Text;
                }
                else if (chr == '[' && state == Text)
                {
                    if (currentSection = nullptr)
                        sections->Add(currentSection);
                    currentSection = new IniSection();
                    state = SectionName;
                }
                else if (chr == ']' && state == SectionName)
                {
                    state = Drop;
                    if (currentProperty == nullptr)
                        currentProperty = new IniProperty();
                }
                else if (chr == '=')
                {
                    state = PropertyValue;
                }
                else if (chr == ';')
                {
                    state = Comment;
                }
                else
                {
                    if (currentSection != nullptr && state == SectionName)
                    {
                        currentSection->GetName() = currentSection->GetName().Append(chr);
                    }
                    if (currentProperty != nullptr)
                    {
                        if (state == Text)
                        {
                            currentProperty->key = currentProperty->key.Append(chr);
                        }
                        else if (state == PropertyValue)
                        {
                            currentProperty->value = currentProperty->value.Append(chr);
                        }
                    }
                }
            }

            if (currentSection != nullptr)
            {
                if (currentProperty != nullptr && currentProperty->key.Length() > 0)
                {
                    currentSection->GetProperties()->Add(currentProperty);
                }
                else
                {
                    delete currentProperty;
                }

                if (currentSection->GetProperties()->Size() != 0)
                {
                    sections->Add(currentSection);
                }
                else
                {
                    delete currentSection;
                }
            }
        }

        ~IniFile()
        {
            for (size_t i = 0; i < sections->Size(); i++)
                delete sections->At(i);
            delete sections;
        }

        IniSection *GetSection(const nk::String &name)
        {
            for (size_t i = 0; i < sections->Size(); i++)
            {
                if (sections->At(i)->GetName() == name)
                    return sections->At(i);
            }
            return nullptr;
        }
    };

}; // namespace nk

#endif