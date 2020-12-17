namespace Device
{
    class Keyboard
    {
    private:
        static char scancode_map[128];

    public:
        static void Initialize();

        static void HandleInterrupt(unsigned int scancode);

    private:
        static void NewScancode(unsigned int code, char c);

        static char MapChar(unsigned int scancode);
    };
} // namespace Device