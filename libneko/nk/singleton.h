#ifndef _SINGLETON_H
#define _SINGLETON_H

#define DECLARE_SINGLETON(classname) \
private:                             \
    static classname *_instance;     \
    classname();                     \
                                     \
public:                              \
    static classname *GetInstance();

#define DEFINE_SINGLETON(classname)             \
    classname *classname::_instance = nullptr;  \
    classname *classname::GetInstance() \
    {                                           \
        if (_instance == nullptr)               \
            _instance = new classname();        \
        return _instance;                       \
    }

#endif