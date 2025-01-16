#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/sysctl.c"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/gpio.c"


int main(void){
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);//mikroi�lemcinin(MCU) saat ayarlar�
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // f portunu aktifle�tirme

    HWREG(GPIO_PORTF_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY;// portf in 0.�nc� pini (pf0) � default ayar�nda kapal�d�r . bunu a�ar�z bu iki sat�r kodla
    HWREG(GPIO_PORTF_BASE+GPIO_O_CR) |= GPIO_PIN_0;

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2| GPIO_PIN_3 ); //pin1 i outputa ayarl�yoruz. led yakmak i�in
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4);// pin4ve pin0 input ayarlar�z. switch okuma i�in
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);// bas�nca 4 mA ak�yor. butona bas�lmad��� s�rece MCU nun pininde lojik 1 var, butona bas�nca lojik 0. PU bas�lmad���nda lojik 1, PD olsayd� bas�lmad���nda lojik 0




int a =1000000; // a s�reyi �imdilik 1sn ayarlad�m
int i;
    while(1){ // sonsuz d�ng�m

        for( i=2;i<=8;i=i*2)
        {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2| GPIO_PIN_3, i);//ledi yak�yorum. 2^1 (pin1)
       SysCtlDelay(a); //a kadar bekle

       GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2| GPIO_PIN_3, 0);//ledi s�nd�rd�m
       SysCtlDelay(a);
        }
       if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0 && a<4000000000){// pin0 switch 2, o e�er 0 sa yani bas�ld�ysa ve  a bekleme s�rem 4milyarden k���kse
           a=a+200000 ;// a s�resini uzat�yorum yan�p s�nme yava�l�yor.

       }
       if (GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0 && a>200000){// e�er pin 4 yani switch 1 0 sa yani b as�ld�ysa ve a s�resi 200binden b�y�kse
               a=a-200000 ;// a s�resini k�salt�yorum yan�p s�nme h�zlan�yor
       }
       }
    }

