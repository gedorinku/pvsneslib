/*---------------------------------------------------------------------------------


    Capcom Logo for SNES Projects
    Author: digifox


---------------------------------------------------------------------------------*/
#include <snes.h>
#include "logo.h"

int sa1_main(void) {}
int main(void) {
    // Initialize sound engine (take some time)
    spcBoot();

    // Initialize SNES
    consoleInit();

    dmaClearVram();

    initCapcomLogo();

    setScreenOn();

    while (1) {
        if (updateCapcomLogo() == 1) {
            // The logo animation is complete
            // Paste your game code here
            // consoleNocashMessage("Start your game!");
        }

        WaitForVBlank();
    }
    return 0;
}
