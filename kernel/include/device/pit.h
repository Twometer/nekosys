#ifndef _PIT_H
#define _PIT_H

namespace Device
{

    class PIT
    {
    public:
        enum AccessMode
        {
            Latch = 0,
            LowByte = 1,
            HighByte = 2,
            LowAndHigh = 3
        };

        enum OperatingMode
        {
            InterruptOnTerminalCount = 0,
            HardwareOneShot = 1,
            RateGenerator = 2,
            SquareWaveGenerator = 3,
            SoftwareStrobe = 4,
            HardwareStrobe = 5
        };

    public:
        static void Configure(int channel, AccessMode accessMode, OperatingMode operatingMode, bool useBcd);

        static void SetSpeed(int channel, int freq);
    };

} // namespace Device

#endif