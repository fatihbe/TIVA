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
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);//mikroiþlemcinin(MCU) saat ayarlarý
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // f portunu aktifleþtirme

    HWREG(GPIO_PORTF_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY;// portf in 0.ýncý pini (pf0) ý default ayarýnda kapalýdýr . bunu açarýz bu iki satýr kodla
    HWREG(GPIO_PORTF_BASE+GPIO_O_CR) |= GPIO_PIN_0;

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2| GPIO_PIN_3 ); //pin1 i outputa ayarlýyoruz. led yakmak için
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4);// pin4ve pin0 input ayarlarýz. switch okuma için
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);// basýnca 4 mA akýyor. butona basýlmadýðý sürece MCU nun pininde lojik 1 var, butona basýnca lojik 0. PU basýlmadýðýnda lojik 1, PD olsaydý basýlmadýðýnda lojik 0




int a =1000000; // a süreyi þimdilik 1sn ayarladým
int i;
    while(1){ // sonsuz döngüm

        for( i=2;i<=8;i=i*2)
        {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2| GPIO_PIN_3, i);//ledi yakýyorum. 2^1 (pin1)
       SysCtlDelay(a); //a kadar bekle

       GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2| GPIO_PIN_3, 0);//ledi söndürdüm
       SysCtlDelay(a);
        }
       if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0 && a<4000000000){// pin0 switch 2, o eðer 0 sa yani basýldýysa ve  a bekleme sürem 4milyarden küçükse
           a=a+200000 ;// a süresini uzatýyorum yanýp sönme yavaþlýyor.

       }
       if (GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0 && a>200000){// eðer pin 4 yani switch 1 0 sa yani b asýldýysa ve a süresi 200binden büyükse
               a=a-200000 ;// a süresini kýsaltýyorum yanýp sönme hýzlanýyor
       }
       }
    }

