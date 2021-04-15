#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <nk/vector.h>
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

        nk::Vector<Window*> windows {};

    public:
        Application();

        void Run();

        void Exit();

        void OpenWindow(Window &window);
    };

};

#endif