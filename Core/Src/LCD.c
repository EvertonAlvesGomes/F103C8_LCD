/*
 * LCD.c
 *
 *  Created on: Jan 30, 2021
 *      Author: Everton A. Gomes
 */


#include "LCD.h"
#include "stm32f1xx_hal.h"


void delay_us(uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while(__HAL_TIM_GET_COUNTER(&htim1) < us);
}


void lcd_pulse_enable()
{
	HAL_GPIO_WritePin(GPIOA, LCD_PIN_EN, GPIO_PIN_SET);
	delay_us(50);
	HAL_GPIO_WritePin(GPIOA, LCD_PIN_EN, GPIO_PIN_RESET);
	delay_us(50);
}


void lcd_envia_nibble(char nb)
{
	GPIO_PinState nibble[4];
	uint8_t i;
	for(i=0; i<4; i++) {
		if(nb & (0x01<<i))
			nibble[i] = GPIO_PIN_SET;
		else
			nibble[i] = GPIO_PIN_RESET;
	}

	//Writing bits
	HAL_GPIO_WritePin(GPIOA, LCD_PIN_DB7, nibble[3]);
	HAL_GPIO_WritePin(GPIOA, LCD_PIN_DB6, nibble[2]);
	HAL_GPIO_WritePin(GPIOA, LCD_PIN_DB5, nibble[1]);
	HAL_GPIO_WritePin(GPIOA, LCD_PIN_DB4, nibble[0]);
	lcd_pulse_enable();
}


void lcd_envia_byte(char b)
{
	lcd_envia_nibble(b >> 4);
	lcd_envia_nibble(b & 0x0F);
}


void lcd_function_set()
{
	//attempting 3 times to put LCD in 8-bit mode

	HAL_GPIO_WritePin(GPIOA, LCD_PIN_RS, GPIO_PIN_RESET);
	lcd_envia_nibble(0x03);
	HAL_Delay(5);
	HAL_GPIO_WritePin(GPIOA, LCD_PIN_RS, GPIO_PIN_RESET);
	lcd_envia_nibble(0x03);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, LCD_PIN_RS, GPIO_PIN_RESET);
	lcd_envia_nibble(0x03);
	HAL_Delay(10);

	//4-bit mode
	HAL_GPIO_WritePin(GPIOA, LCD_PIN_RS, GPIO_PIN_RESET);
	lcd_envia_byte(0x20);
	HAL_Delay(1);
	//4 lines to be used
	#ifdef LCD_4LINES
		HAL_GPIO_WritePin(GPIOA, LCD_PIN_RS, GPIO_PIN_RESET);
		lcd_envia_byte(0x28);
		HAL_Delay(1);
	#endif
	#ifdef LCD_2LINES
		HAL_GPIO_WritePin(GPIOA, LCD_PIN_RS, GPIO_PIN_RESET);
		lcd_envia_byte(0x20);
		HAL_Delay(1);
	#endif
}


void lcd_on_off_control(char on_off_value)
{
	HAL_GPIO_WritePin(GPIOA, LCD_PIN_RS, GPIO_PIN_RESET);
	lcd_envia_byte(on_off_value);
	HAL_Delay(1);
}



void lcd_clear_display()
{
	HAL_GPIO_WritePin(GPIOA, LCD_PIN_RS, GPIO_PIN_RESET);
	lcd_envia_byte(0x01);
	HAL_Delay(200);
}


void lcd_entry_mode_set()
{
	HAL_GPIO_WritePin(GPIOA, LCD_PIN_RS, GPIO_PIN_RESET);
	lcd_envia_byte(0x06);	//increment by 1, shift is off
	HAL_Delay(1);
}


void lcd_reset()
{
	//Put all pins at low
	HAL_GPIO_WritePin(GPIOA, LCD_PIN_DB4, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, LCD_PIN_DB5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, LCD_PIN_DB6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, LCD_PIN_DB7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, LCD_PIN_RS, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, LCD_PIN_EN, GPIO_PIN_RESET);
	HAL_Delay(50);
}


void lcd_init()
{
	HAL_Delay(10);	//initialization delay
	lcd_reset();
	lcd_function_set();
	lcd_on_off_control(0x08); //display off, cursor and blinking off
	lcd_clear_display();

	lcd_entry_mode_set();
	lcd_on_off_control(0x0C);	//display on, cursor and blinking off

	HAL_Delay(10);
}


void lcd_write(char c)
{
	HAL_GPIO_WritePin(GPIOA, LCD_PIN_RS, GPIO_PIN_SET);
	lcd_envia_byte(c);
}



void lcd_pos_xy(uint8_t x, uint8_t y)
{
	unsigned char line_add = 0;
	unsigned char pos_xy = 0;

	//If column passed by argument is less than 0 or greater than 19
	if(y <= 0) y = 1;
	if(y > 20) y = 20;

	if(x <= 0) x = 1;
	if(x > 4) x = 4;

#ifdef LCD_4LINES
	switch(x) {
	case 1 :
		pos_xy = y-1;
		break;
	case 2 :
		pos_xy = LCD_2ND_LINE + y - 1;
		break;
	case 3 :
		pos_xy = LCD_3RD_LINE + y - 1;
		break;
	case 4 :
		pos_xy = LCD_4TH_LINE + y - 1;
		break;
	}


#endif

#ifdef LCD_2LINES
	if(x == 1) {
		//first line
		line_add = y-1;
		pos_xy = line_add;
	}
	else if(x == 3) {
		//third line
		line_add = LCD_2ND_LINE + y-1;
		pos_xy = line_add;
	}

#endif

	//pos_xy = line_add + y;
	HAL_GPIO_WritePin(GPIOA, LCD_PIN_RS, GPIO_PIN_RESET);
	lcd_envia_byte(0x80 | pos_xy);
}


void lcd_write_string(char *str, uint8_t pos_x, uint8_t pos_y)
{
	int i = 0;
	lcd_pos_xy(pos_x, pos_y);
	while(str[i] != 0x00) {
		lcd_write(str[i]);
		i++;
		pos_y++;
		lcd_pos_xy(pos_x, pos_y);
	}
}
