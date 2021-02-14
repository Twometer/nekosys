#include <kernel/memory/shbuf.h>

using namespace Memory;

uint32_t SharedBuffer::idCounter = 0;

SharedBuffer::SharedBuffer(pageframe_t base, size_t numPages)
    : bufid(idCounter++), baseFrame(base), numPages(numPages) {}