/*
 * LCD.h
 *
 *  Created on: Jan 30, 2021
 *      Author: Everton A. Gomes
 *
 *      Test display contents: http://avtanski.net/projects/lcd/
 *
 *      Special thanks to Controllerstech. Visit his website:
 *      https://controllerstech.com/interface-lcd-16x2-with-stm32-without-i2c/
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

//Includes
#include "stm32f1xx_hal.h"


//Pin Definitions
#define LCD_PIN_RS	GPIO_PIN_0	//RS
#define LCD_PIN_EN	GPIO_PIN_1	//EN
#define LCD_PIN_DB4	GPIO_PIN_4	//DB4
#define LCD_PIN_DB5	GPIO_PIN_5	//DB5
#define LCD_PIN_DB6	GPIO_PIN_6	//DB6
#define LCD_PIN_DB7	GPIO_PIN_7	//DB7

//define LCD_2LINES		//uncomment this to use just 2 lines on LCD

#ifndef LCD_2LINES
#define LCD_4LINES
#endif

#ifdef LCD_4LINES
#define LCD_1ST_LINE	0x00	//1st line, 1st position
#define LCD_2ND_LINE	0x40	//First DDRAM address of 2nd line
#define LCD_3RD_LINE	0x14	//In fact, this is the continuation of 1st line
#define LCD_4TH_LINE	0x54	//Continuation of 2nd line
#endif

#ifdef LCD_2LINES
#define LCD_1ST_LINE 	0x00
#define LCD_2ND_LINE	0x14
#endif

//Variables
extern TIM_HandleTypeDef htim1;		//TIM1 handler


//Function headers
void lcd_pulse_enable(void);
void lcd_envia_nibble(char nb);
void lcd_envia_byte(char b);
void lcd_function_set(void);
void lcd_on_off_control(char on_off_value);
void lcd_clear_display(void);
void lcd_entry_mode_set(void);
void lcd_reset(void);
void lcd_write(char c);
void lcd_init(void);
void lcd_pos_xy(unsigned char x, unsigned char y);
void delay_us(uint16_t us);
void lcd_write_string(char *str, uint8_t pos_x, uint8_t pos_y);


#endif /* INC_LCD_H_ */
