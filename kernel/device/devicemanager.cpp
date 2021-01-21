#include <kernel/device/devicemanager.h>

using namespace Device;

Keyboard* DeviceManager::keyboard;        
PIC* DeviceManager::pic;

void DeviceManager::Initialize() {
    keyboard = new Keyboard();
    keyboard->Initialize();

    pic = new PIC();
    pic->Remap();
}