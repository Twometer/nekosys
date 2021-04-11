#include <libgui/application.h>

int main()
{
    Gui::Application application;

    Gui::Window window("Test", 128, 128);
    application.OpenWindow(window);

    application.Run();

    return 0;
}