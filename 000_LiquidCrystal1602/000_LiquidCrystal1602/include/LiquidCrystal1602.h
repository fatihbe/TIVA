/*
 * LiquidCrystal1602.h
 *
 *  Created on: 10 Kas 2023
 *      Author: LCD
 */

#ifndef LIQUIDCRYSTAL1602_H_
#define LIQUIDCRYSTAL1602_H_

#include <stdint.h>

#define LCD_PORT_ENABLE SYSCTL_PERIPH_GPIOB
#define LCD_PORT GPIO_PORTB_BASE

#define LCD_DATA 0xF0               //LCD ports-> PB4, PB5, PB6, PB7
#define RS GPIO_PIN_0                     //RS -> PB0
#define RW GPIO_PIN_1                    //RW -> PB1
#define EN GPIO_PIN_2                  //EN -> PB2

#define SEND_COMMAND0x01
#define SEND_DATA 0x00
#define ENABLE_HIGH 0x04
#define ENABLE_LOW 0x00
#define RS_COMMAND 0x00
#define RS_DATA 0x01

#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x06
#define LCD_FUNCTIONSET 0x28
#define LCD_SETDDRAMADDR 0x80

#define LCD_DISPLAYON 0x04
#define LCD_CURSORON 0x02
#define LCD_BLINKON 0x01


#define DELAYMS 1000
#define DELAYUS 1000000


void LCDSetup();

void LCDWrite(unsigned char word);
void LCDCommandWrite(unsigned char Command);
void LCDPrint(const char *arg,...);
void LCDsetCrusor(int row, int col);

void LCDClear();
void LCDHome();


void LCDDisplay();
void LCDnoDisplay();
void LCDCrusor();
void LCDnoCrusor();
void LCDBlink();
void LCDnoBlink();


void delayMs(uint16_t ui16Milliseconds);
void delayUs(uint16_t ui16Microseconds);

#endif /* LIQUIDCRYSTAL1602_H_ */
