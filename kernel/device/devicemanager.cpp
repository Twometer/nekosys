#include <kernel/device/devicemanager.h>

using namespace Device;

Keyboard* DeviceManager::keyboard;        
Mouse* DeviceManager::mouse;
PIC* DeviceManager::pic;

void DeviceManager::Initialize() {
    keyboard = new Keyboard();
    keyboard->Initialize();

    mouse = new Mouse();
    mouse->Initialize();

    pic = new PIC();
    pic->Remap();
}