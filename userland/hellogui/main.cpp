#include <libgui/application.h>

int main()
{
    Gui::Application application;

    Gui::Window window("This is a test window", 400, 300);
    application.OpenWindow(window);

    application.Run();

    return 0;
}