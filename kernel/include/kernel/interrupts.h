#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include <nk/vector.h>

struct IDT_entry
{
    unsigned short int offset_lowerbits;
    unsigned short int selector;
    unsigned char zero;
    unsigned char type_attr;
    unsigned short int offset_higherbits;
};

struct interrupt_frame
{
    unsigned int ip;
    unsigned int cs;
    unsigned int flags;
    unsigned int sp;
    unsigned int ss;
};

namespace Kernel
{
    class InterruptHandler
    {
    public:
        virtual void HandleInterrupt(unsigned int interrupt) = 0;
    };

    struct InterruptHandlerEntry
    {
        unsigned int interrupt;
        InterruptHandler *handler;
    };

    class Interrupts
    {
    private:
        static nk::Vector<InterruptHandlerEntry> entries;
        
    public:
        static void SetupIdt();

        static void Enable();

        static void Disable();

        static void HandleException(unsigned int vector, struct interrupt_frame *frame);

        static void SetIdtEntry(unsigned int interrupt, unsigned char type, unsigned long address);

        static void HandleInterrupt(unsigned int interrupt);

        static void AddHandler(unsigned int interrupt, InterruptHandler *handler);
    };

}; // namespace Kernel

#endif