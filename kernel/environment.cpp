#include <nk/inifile.h>
#include <kernel/environment.h>
#include <kernel/fs/vfs.h>
#include <kernel/panic.h>

#define CONFIG_FILE_PATH "/etc/env.ini"

using namespace Kernel;

DEFINE_SINGLETON(Environment)

Environment::Environment()
{
}

void Environment::Load()
{
    auto vfs = FS::VirtualFileSystem::GetInstance();
    auto file = vfs->GetFileMeta(CONFIG_FILE_PATH);
    if (file.type == FS::DirEntryType::Invalid)
    {
        Kernel::Panic("environment", "Missing Config file " CONFIG_FILE_PATH);
        return;
    }

    auto fd = vfs->Open(CONFIG_FILE_PATH);
    char data[file.size];
    vfs->Read(fd, 0, file.size, (uint8_t *)data);
    iniFile = new nk::IniFile(data);
    section = iniFile->GetSection("Environment");
    vfs->Close(fd);
}

const nk::String &Environment::Get(const nk::String &key)
{
    return section->GetProperty(key);
}