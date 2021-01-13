#ifndef _SINGLETON_H
#define _SINGLETON_H

#define DECLARE_SINGLETON(classname) \
private:                             \
    static classname *_instance;     \
    classname();                     \
                                     \
public:                              \
    static classname *get_instance();

#define DEFINE_SINGLETON(classname)             \
    classname *classname::_instance = nullptr;  \
    classname *classname::get_instance() \
    {                                           \
        if (_instance == nullptr)               \
            _instance = new classname();        \
        return _instance;                       \
    }

#endif