#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"
#include "LiquidCrystal1602.h"

uint8_t saniyeBirler, saniyeOnlar, dakikaBirler, dakikaOnlar, saatBirler,saatOnlar;
uint32_t zaman = 0,eski_zaman=86399;

void kesmefonksiyon()
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    zaman++;

    saniyeBirler = zaman % 10;
    saniyeOnlar = (zaman / 10) % 6;
    dakikaBirler = (zaman / 60) % 10;
    dakikaOnlar = (zaman / 600) % 6;
    saatBirler = (zaman / 3600) % 10;
    saatOnlar = (zaman / 36000) % 10;
    if (zaman == 86399)
        zaman = 0;
}

int main(void)
{
    SysCtlClockSet(
    SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    IntMasterEnable();
    IntEnable(INT_TIMER0A);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    TimerConfigure(TIMER0_BASE, TIMER_CFG_A_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()-1);

    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntRegister(TIMER0_BASE, TIMER_A, kesmefonksiyon);

    TimerEnable(TIMER0_BASE, TIMER_A);


    LCDSetup();
    LCDnoCrusor();

    while (1)
    {
        if (zaman != eski_zaman)
        {
            LCDsetCrusor(1, 1);
            LCDPrint("%d", saatOnlar);
            LCDsetCrusor(1, 2);
            LCDPrint("%d", saatBirler);

            LCDsetCrusor(1, 3);
            LCDPrint("%s", ".");

            LCDsetCrusor(1, 4);
            LCDPrint("%d", dakikaOnlar);
            LCDsetCrusor(1, 5);
            LCDPrint("%d", dakikaBirler);

            LCDsetCrusor(1, 6);
            LCDPrint("%s", ".");

            LCDsetCrusor(1, 7);
            LCDPrint("%d", saniyeOnlar);
            LCDsetCrusor(1, 8);
            LCDPrint("%d", saniyeBirler);
            eski_zaman = zaman;
        }
    }
}
