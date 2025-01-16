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

//default display ayarlar�.
//display a��k(D=1) cursor a��k(C=1) blink kapal�(B=0).(1,D,C,B-> 1110)
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
//LCD'ye komut g�nderirken kullan�lacak fonksiyon.Command k�sm�na g�nderece�imiz komutu yaz�yoruz.
//LCD'nin ekran�n� ayarlarken vb durumlarda i�e yarar.
void LCDCommandWrite(unsigned char Command)
{
    Send(Command, RS_COMMAND);
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//char olarak bir ifadenin ekrana yazmak i�in kullan�l�r.
void LCDWrite(unsigned char Word)
{
    Send(Word, RS_DATA);
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
void LCDStartUp()
{

    delayMs(50); // lcd ilk a��l�nca haberle�me protokol�n� ba�latmak i�in �� kere 30 hex g�nder.
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

    delayMs(10); // ���nc�
    GPIOPinWrite(LCD_PORT, LCD_DATA, 0x30);
    PulseEnable();

//    LCDCommandWrite(0x30);



//    GPIOPinWrite(LCD_PORT, LCD_DATA, 0x30);
//    GPIOPinWrite(LCD_PORT, RS | RW | EN, 0x04);
//    delayUs(20);
//    GPIOPinWrite(LCD_PORT, RS | RW | EN, 0x00);

    // 4 bit haberle�me modunu a�
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
//LCD ile haberle�me ve default ayarlar�n yap�lmas�nda gerekli olacak b�t�n ayarlamalar� yapar.
//Kod sat�r�nda ilk yaz�lmas� gerekir. Bu sayede LCD ile sa�l�kl� bir ileti�im i�inde olunur.
//int sysclk de�i�kenine " SysCtlClockGet()-1 " yaz�lmas� gerekir. gecikme fonksiyonlar�n�n ve
//di�er LCD habel�emelerinin en d�zg�n ayarda ger�ekle�mesi i�in gereklidir.
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
//"%d" decimal (say�) tiplerde kullan�l�r
//LCDPrint("%d", 56);
//
//"%s" String (kelime) tiplerde kullan�l�r LCDPrint
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
//LCD imlecinin yerini ayarlar. �stenilen yere yazmak i�in sat�rlar ve s�t�nlar aras�nda imleci konumland�r�r.
//int row sat�r verisi girilmelidir.(1-2)
//int col s�tun verisi girilmelidir.(1-16)
void LCDsetCrusor(int row, int col)
{
    int row_offsets[] = { 0x00, 0x40 };

    if (row > 2)
        row = 2;
    else if (row < 1)
        row = 1;

    row = row - 1; // diziler 0'dan ba�lad���ndan sat�r say�s� bir eksik olmal�.

    if (col > 16)
        col = 16;
    else if (col < 1)
        col = 1;

    col = col - 1; // diziler 0'dan ba�lad���ndan sat�r say�s� bir eksik olmal�.

    LCDCommandWrite(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//LCD ekran�n� tamamen temizlemek i�in kullan�l�r.
void LCDClear()
{
    LCDCommandWrite(LCD_CLEARDISPLAY);
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//LCD imlecini 1.sat�r�n 1.s�t�una g�nderir. H�zl� kullan�m a��s�ndan elveri�ildir.
void LCDHome()
{
    LCDCommandWrite(LCD_RETURNHOME);
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//LCD ekran�n�n a�mak i�in kullan�l�r.
void LCDDisplay()
{
    displaycontrol |= LCD_DISPLAYON;
    LCDCommandWrite(displaycontrol);
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//LCD ekran�n�n kapamak i�in kullan�l�r.
void LCDnoDisplay()
{
    displaycontrol &= ~LCD_DISPLAYON;
    LCDCommandWrite(displaycontrol);
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//LCD'nin imlecini a�mak i�in kullan�l�r.
void LCDCrusor()
{
    displaycontrol |= LCD_CURSORON;
    LCDCommandWrite(displaycontrol);
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//LCD'nin imlecini kapamak i�in kullan�l�r.
void LCDnoCrusor()
{
    displaycontrol &= ~LCD_CURSORON;
    LCDCommandWrite(displaycontrol);
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//LCD'nin imlecini yan�p s�nmesini a�mak i�in kullan�l�r.
void LCDBlink()
{
    displaycontrol |= LCD_BLINKON;
    LCDCommandWrite(displaycontrol);
}
//----------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------//
//LCD'nin imlecini yan�p s�nmesini kapamak i�in kullan�l�r.
void LCDnoBlink()
{
    displaycontrol &= ~LCD_BLINKON;
    LCDCommandWrite(displaycontrol);
}
//----------------------------------------------------------------------------------------------------------//

