#include "LiquidCrystal1602.h"
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "driverlib/systick.h"

//default display ayarlarý.
//display açýk(D=1) cursor açýk(C=1) blink kapalý(B=0).(1,D,C,B-> 1110)
int displaycontrol = 0x0E;

_Bool flag = 1;
void waitus(void)
{
    flag = 0;
    //return flag;
}

void DelayFunction(uint16_t ui16DelayAmount,uint32_t ui32DelayType)
{
    uint16_t ui16CounterFor = 0;
    uint32_t ui32Period=0;
    ui32Period = (SysCtlClockGet()/ui32DelayType);

    for (ui16CounterFor = 0; ui16CounterFor <= ui16DelayAmount;
            ui16CounterFor++)
    {
        SysTickPeriodSet(ui32Period);
        SysTickEnable();
        SysTickIntEnable();
        SysTickIntRegister(waitus);
        flag = 1;
        while (flag);
        SysTickDisable();
    }
}

void delayMs(uint16_t ui16Milliseconds){
    DelayFunction(ui16Milliseconds,DELAYMS);
}

void delayUs(uint16_t ui16Microseconds){
    DelayFunction(ui16Microseconds,DELAYUS);
}



//----------------------------------------------------------------------------------------------------------//
void PulseEnable(){
    GPIOPinWrite(LCD_PORT, EN, ENABLE_LOW);
    delayUs(1);
    GPIOPinWrite(LCD_PORT, EN, ENABLE_HIGH);
    delayUs(1);
    GPIOPinWrite(LCD_PORT, EN, ENABLE_LOW);
    delayUs(100);
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
void Send(unsigned char value, uint8_t mode){
    GPIOPinWrite(LCD_PORT, RS , mode);

    GPIOPinWrite(LCD_PORT, LCD_DATA, (value & 0xf0));
    PulseEnable();
    GPIOPinWrite(LCD_PORT, LCD_DATA, ((value & 0x0f) << 4));
    PulseEnable();
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//LCD'ye komut gönderirken kullanýlacak fonksiyon.Command kýsmýna göndereceðimiz komutu yazýyoruz.
//LCD'nin ekranýný ayarlarken vb durumlarda iþe yarar.
void LCDCommandWrite(unsigned char Command)
{
    Send(Command, RS_COMMAND);
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//char olarak bir ifadenin ekrana yazmak için kullanýlýr.
void LCDWrite(unsigned char Word)
{
    Send(Word, RS_DATA);
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
void LCDStartUp()
{

    delayMs(50); // lcd ilk açýlýnca haberleþme protokolünü baþlatmak için üç kere 30 hex gönder.
    GPIOPinWrite(LCD_PORT, LCD_DATA, 0x30);
    PulseEnable();

//    LCDCommandWrite(0x30);

//    GPIOPinWrite(LCD_PORT, LCD_DATA, 0x30);
//    GPIOPinWrite(LCD_PORT, RS | RW | EN, 0x04);
//    delayUs(20);
//    GPIOPinWrite(LCD_PORT, RS | RW | EN, 0x00);

    delayMs(10); // ikinci
    GPIOPinWrite(LCD_PORT, LCD_DATA, 0x30);
    PulseEnable();


//    LCDCommandWrite(0x30);


//    GPIOPinWrite(LCD_PORT, LCD_DATA, 0x30);
//    GPIOPinWrite(LCD_PORT, RS | RW | EN, 0x04);
//    delayUs(20);
//    GPIOPinWrite(LCD_PORT, RS | RW | EN, 0x00);

    delayMs(10); // üçüncü
    GPIOPinWrite(LCD_PORT, LCD_DATA, 0x30);
    PulseEnable();

//    LCDCommandWrite(0x30);



//    GPIOPinWrite(LCD_PORT, LCD_DATA, 0x30);
//    GPIOPinWrite(LCD_PORT, RS | RW | EN, 0x04);
//    delayUs(20);
//    GPIOPinWrite(LCD_PORT, RS | RW | EN, 0x00);

    // 4 bit haberleþme modunu aç
    delayUs(100);
    GPIOPinWrite(LCD_PORT, LCD_DATA, 0x20);
    PulseEnable();

//    LCDCommandWrite(0x20);


//    GPIOPinWrite(LCD_PORT, LCD_DATA, 0x20);
//    GPIOPinWrite(LCD_PORT, RS | RW | EN, 0x04);
//    delayUs(20);
//    GPIOPinWrite(LCD_PORT, RS | RW | EN, 0x00);
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//LCD ile haberleþme ve default ayarlarýn yapýlmasýnda gerekli olacak bütün ayarlamalarý yapar.
//Kod satýrýnda ilk yazýlmasý gerekir. Bu sayede LCD ile saðlýklý bir iletiþim içinde olunur.
//int sysclk deðiþkenine " SysCtlClockGet()-1 " yazýlmasý gerekir. gecikme fonksiyonlarýnýn ve
//diðer LCD habelþemelerinin en düzgün ayarda gerçekleþmesi için gereklidir.
void LCDSetup()
{
    SysCtlPeripheralEnable(LCD_PORT_ENABLE);
    GPIOPinTypeGPIOOutput(LCD_PORT, 0xFF);
    GPIOPinWrite(LCD_PORT, 0xff, 0x00);

    LCDStartUp();

    LCDCommandWrite(LCD_FUNCTIONSET);
    LCDCommandWrite(displaycontrol);
    LCDCommandWrite(LCD_CLEARDISPLAY);
    LCDCommandWrite(LCD_ENTRYMODESET);
    LCDHome();
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//"%d" decimal (sayý) tiplerde kullanýlýr
//LCDPrint("%d", 56);
//
//"%s" String (kelime) tiplerde kullanýlýr LCDPrint
//("%s", "hello");
void LCDPrint(const char *format, ...)
{
    GPIOPinWrite(LCD_PORT, RS , RS_DATA);

    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    int i = 0;
    for (i = 0; buffer[i] != '\0'; i++)
    {

        GPIOPinWrite(LCD_PORT, LCD_DATA, (buffer[i] & 0xf0));
        PulseEnable();
        GPIOPinWrite(LCD_PORT, LCD_DATA, ((buffer[i] & 0x0f) << 4));
        PulseEnable();
    }
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//LCD imlecinin yerini ayarlar. Ýstenilen yere yazmak için satýrlar ve sütünlar arasýnda imleci konumlandýrýr.
//int row satýr verisi girilmelidir.(1-2)
//int col sütun verisi girilmelidir.(1-16)
void LCDsetCrusor(int row, int col)
{
    int row_offsets[] = { 0x00, 0x40 };

    if (row > 2)
        row = 2;
    else if (row < 1)
        row = 1;

    row = row - 1; // diziler 0'dan baþladýðýndan satýr sayýsý bir eksik olmalý.

    if (col > 16)
        col = 16;
    else if (col < 1)
        col = 1;

    col = col - 1; // diziler 0'dan baþladýðýndan satýr sayýsý bir eksik olmalý.

    LCDCommandWrite(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//LCD ekranýný tamamen temizlemek için kullanýlýr.
void LCDClear()
{
    LCDCommandWrite(LCD_CLEARDISPLAY);
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//LCD imlecini 1.satýrýn 1.sütüuna gönderir. Hýzlý kullaným açýsýndan elveriþildir.
void LCDHome()
{
    LCDCommandWrite(LCD_RETURNHOME);
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//LCD ekranýnýn açmak için kullanýlýr.
void LCDDisplay()
{
    displaycontrol |= LCD_DISPLAYON;
    LCDCommandWrite(displaycontrol);
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//LCD ekranýnýn kapamak için kullanýlýr.
void LCDnoDisplay()
{
    displaycontrol &= ~LCD_DISPLAYON;
    LCDCommandWrite(displaycontrol);
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//LCD'nin imlecini açmak için kullanýlýr.
void LCDCrusor()
{
    displaycontrol |= LCD_CURSORON;
    LCDCommandWrite(displaycontrol);
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//LCD'nin imlecini kapamak için kullanýlýr.
void LCDnoCrusor()
{
    displaycontrol &= ~LCD_CURSORON;
    LCDCommandWrite(displaycontrol);
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//LCD'nin imlecini yanýp sönmesini açmak için kullanýlýr.
void LCDBlink()
{
    displaycontrol |= LCD_BLINKON;
    LCDCommandWrite(displaycontrol);
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//LCD'nin imlecini yanýp sönmesini kapamak için kullanýlýr.
void LCDnoBlink()
{
    displaycontrol &= ~LCD_BLINKON;
    LCDCommandWrite(displaycontrol);
}
//----------------------------------------------------------------------------------------------------------//

