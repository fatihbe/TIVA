#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
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
#include "driverlib/adc.h"

void Setup();
void UARTSetup();
void TIMERSetup();
void ADC0Setup();
void UARTRead();
void UARTSend();
void TransferClock();
uint16_t ADCRead();

#define RedLed  0x02
#define GreenLed 0x08
#define BlueLed 0x04
#define RGB 0x0E
uint8_t ledControl=0;

#define BUFFER_SIZE 16 // veri için saklanacak tampon boyutu
char receivedData[BUFFER_SIZE]; // Gelen veriyi saklamak için tampon
char transmitData[BUFFER_SIZE]; // Giden veriyi saklamak için tampon

uint8_t saniyeBirler, saniyeOnlar, dakikaBirler, dakikaOnlar, saatBirler,saatOnlar;
uint32_t zaman = 0,eski_zaman=86399;

uint32_t ADCData[4];
uint16_t TemperatureAverage, LastTemperatureAverage;
uint16_t TemperatureValueC, LastTemperatureValueC;

void kesmefonksiyon()
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    zaman++;

    saniyeBirler = zaman % 10;
    saniyeOnlar = (zaman / 10) % 6;
    dakikaBirler = (zaman / 60) % 10;
    dakikaOnlar = (zaman / 600) % 6;
    saatBirler = (zaman / 3600) % 10;
    saatOnlar = (zaman / 36000) % 3;
    if (zaman == 86399)
        zaman = 0;
}

void TransferClock(){
    uint32_t newTime = 0;
    newTime=newTime+(receivedData[2]-48)*36000;  //dizilerin deðerleri +2 olucak sharpdevolp'da kodu yazýnca !!!!!!!+
    newTime=newTime+(receivedData[3]-48)*3600;
    newTime=newTime+(receivedData[5]-48)*600;
    newTime=newTime+(receivedData[6]-48)*60;
    newTime=newTime+(receivedData[8]-48)*10;
    newTime=newTime+(receivedData[9]-48);
    zaman=newTime;
}

void UARTRead()
{
    if (UARTCharsAvail(UART0_BASE))
    {
        uint8_t receivedIndex = 0;
        uint8_t counter=0;

        for (counter=0;counter <= BUFFER_SIZE-1;counter++){
            receivedData[counter]='\0';
        }

        while (UARTCharsAvail(UART0_BASE)/*seri portta veri var m? bool tipinde*/)
        {

            char data = UARTCharGet(UART0_BASE);
            delayMs(25);

            while (data == '\n' || receivedIndex >= BUFFER_SIZE - 1)
            {
                receivedData[receivedIndex] = '\0'; // Stringi sonlandir
                break;
            }
            receivedData[receivedIndex++] = data;
        }

        if (receivedData[0] == '0' & receivedData[1] == '0') //+
        {
            TransferClock();
        }

        else if (receivedData[0] == 'R' & receivedData[1] == '0')
        {
            GPIOPinWrite(GPIO_PORTF_BASE, RGB, (ledControl |= RedLed));
        }
        else if (receivedData[0] == 'X' & receivedData[1] == '0')
        {
            GPIOPinWrite(GPIO_PORTF_BASE, RGB, (ledControl &= ~RedLed));
        }
        else if (receivedData[0] == 'G' & receivedData[1] == '0')
        {
            GPIOPinWrite(GPIO_PORTF_BASE, RGB, (ledControl |= GreenLed));
        }
        else if (receivedData[0] == 'Y' & receivedData[1] == '0')
        {
            GPIOPinWrite(GPIO_PORTF_BASE, RGB, (ledControl &= ~GreenLed));
        }
        else if (receivedData[0] == 'B' & receivedData[1] == '0')
        {
            GPIOPinWrite(GPIO_PORTF_BASE, RGB, (ledControl |= BlueLed));
        }
        else if (receivedData[0] == 'Z' & receivedData[1] == '0')
        {
            GPIOPinWrite(GPIO_PORTF_BASE, RGB, (ledControl &= ~BlueLed));
        }
        if (receivedData[0] == 'T' & (receivedData[1] == 'X'))
        {
            uint8_t i;
            LCDsetCrusor(2, 1);
            for (i=1;i<13;i++){
                LCDsetCrusor(2, i);
                LCDWrite(' ');
            }
            LCDsetCrusor(2, 1);
            for(i=2;receivedData[i] != '\n';i++){
            LCDWrite((char)receivedData[i]);
            }
        }
    }
}

void UARTSend(char *data){
    while(*data++)
        UARTCharPut(UART0_BASE, *data);
}

int main(void)
{
    Setup();
    UARTSetup();
    TIMERSetup();
    ADC0Setup();
    LCDSetup();
    LCDnoCrusor();

    while (1)
    {
        UARTRead();
        ADCRead();
        TemperatureValueC = (147.5 - ((75.0 * 3.3 * TemperatureAverage)) / 4096.0);

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


            LCDsetCrusor(1, 13);
            LCDPrint("%d", TemperatureAverage);
//            sprintf(transmitData, "%d", TemperatureAverage);
////            UARTSend('A'+'1'+(char*)TemperatureAverage);
////            UARTSend("A");
////            UARTSend("1");
////            UARTSend((char)TemperatureAverage);
////            uint8_t i=0;
////            for(i=2;transmitData[i] != '\n';i++){
////                transmitData[i] == '\0';
////            }
//                UARTSend(transmitData);

            UARTCharPut(UART0_BASE, 'c');
            LCDsetCrusor(2, 15);
            LCDPrint("%d", TemperatureValueC);
        }
    }
}

void Setup(){
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

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

    // sadece hangi pinleri uart olarak kullanacagz
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600,
    UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);

    UARTEnable(UART0_BASE);
}

void ADC0Setup(){
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 1);
    ADCProcessorTrigger(ADC0_BASE, 1); //setb conv
}

uint16_t ADCRead(){
    ADCProcessorTrigger(ADC0_BASE, 1);

    while (!ADCIntStatus(ADC0_BASE, 1, 0));

    ADCSequenceDataGet(ADC0_BASE, 1, ADCData);
    TemperatureAverage = (ADCData[0] + ADCData[1] + ADCData[2] + ADCData[3] + 2) / 4;
    return TemperatureAverage;
}
