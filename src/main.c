#include "usbaudio.h"
#include "ui.h"

#include <sys/timers.h>
#include <usbdrvce.h>
#include <stdio.h>

int main(void) {
    //usbaudio_Init();
    ui_Init();
    ui_Loop();

    return 0;
}
