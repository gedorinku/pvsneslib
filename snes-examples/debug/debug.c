/*---------------------------------------------------------------------------------


    Simple debug demo
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

//---------------------------------------------------------------------------------
int sa1_main(void) {}
int main(void)
{
    // Initialize SNES
    consoleInit();

    // Draw a wonderful text :P
    consoleNocashMessage("JUST COUNT VBL\r");

    // Now Put in 16 color mode and disable Bgs except current
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Wait for nothing :P
    setScreenOn();

    while (1)
    {
        consoleNocashMessage("COUNTER=%04u\r\n", (u16)snes_vblank_count);
        WaitForVBlank();
    }
    return 0;
}
