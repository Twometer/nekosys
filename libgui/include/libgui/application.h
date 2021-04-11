#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <libgui/window.h>
#include <libgui/guiconnection.h>

namespace Gui
{

    class Application
    {
    private:
        GuiConnection connection;

        bool exitRequested;

        Window *lastWindow;

    public:
        Application();

        void Run();

        void Exit();

        void OpenWindow(Window &window);
    };

};

#endif