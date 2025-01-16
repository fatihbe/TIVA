#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"
#include "LiquidCrystal1602.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "math.h"

void Setup();
void UARTSetup();
void TIMERSetup();
void UARTRead();
void TransferClock();

#define RedLed  0x02
#define GreenLed 0x08
#define BlueLed 0x04
#define RGB 0x0E

uint8_t ledControl=0;

#define BUFFER_SIZE 10 // Saat verisi için saklanacak tampon boyutu

char receivedData[BUFFER_SIZE]; // Gelen veriyi saklamak için tampon

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
    saatOnlar = (zaman / 36000) % 2;
    if (zaman == 86399)
        zaman = 0;
}

void TransferClock(){
    uint32_t newTime = 0;
    newTime=newTime+(receivedData[0]-48)*36000;
    newTime=newTime+(receivedData[1]-48)*3600;
    newTime=newTime+(receivedData[3]-48)*600;
    newTime=newTime+(receivedData[4]-48)*60;
    newTime=newTime+(receivedData[6]-48)*10;
    newTime=newTime+(receivedData[7]-48)+2;
    zaman=newTime;
}




void UARTRead()
{
    if (UARTCharsAvail(UART0_BASE))
    {
        uint8_t receivedIndex = 0;
        while (UARTCharsAvail(UART0_BASE)/*seri portta veri var mý? bool tipinde*/)
        {

           // GPIOPinWrite(GPIO_PORTF_BASE, RGB, (ledControl ^= RedLed));
            //delayMs(50);

            char data = UARTCharGet(UART0_BASE);
            delayMs(25);

            if (data == '\n' || receivedIndex >= BUFFER_SIZE - 1)
            {
                receivedData[receivedIndex] = '\0'; // Stringi sonlandýr

                TransferClock();

                break;
            }
            receivedData[receivedIndex++] = data;

            if (receivedData[0] == 'R')
            {
                GPIOPinWrite(GPIO_PORTF_BASE, RGB, (ledControl |= RedLed));
            }
            else if (receivedData[0] == 'X')
            {
                GPIOPinWrite(GPIO_PORTF_BASE, RGB, (ledControl &= ~RedLed));
            }
            else if (receivedData[0] == 'G')
            {
                GPIOPinWrite(GPIO_PORTF_BASE, RGB, (ledControl |= GreenLed));
            }
            else if (receivedData[0] == 'Y')
            {
                GPIOPinWrite(GPIO_PORTF_BASE, RGB, (ledControl &= ~GreenLed));
            }
            else if (receivedData[0] == 'B')
            {
                GPIOPinWrite(GPIO_PORTF_BASE, RGB, (ledControl |= BlueLed));
            }
            else if (receivedData[0] == 'Z')
            {
                GPIOPinWrite(GPIO_PORTF_BASE, RGB, (ledControl &= ~BlueLed));
            }


        }
    }
}

int main(void)
{
    Setup();
    UARTSetup();
    TIMERSetup();
    LCDSetup();
    LCDnoCrusor();







    while (1)
    {
        UARTRead();

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

void Setup(){
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    IntMasterEnable();
    IntEnable(INT_TIMER0A);

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

}

void TIMERSetup(){
    TimerConfigure(TIMER0_BASE, TIMER_CFG_A_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() - 1);

    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntRegister(TIMER0_BASE, TIMER_A, kesmefonksiyon);

    TimerEnable(TIMER0_BASE, TIMER_A);
}

void UARTSetup(){

    // sadece hangi pinleri uart olarak kullanacagýz
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600,
    UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);

    UARTEnable(UART0_BASE);
}
