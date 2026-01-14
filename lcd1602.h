#ifndef LCD1602_H
#define LCD1602_H

#include "xgpio.h"
#include "sleep.h"

#define LCD_RS (1 << 0)
#define LCD_EN (1 << 1)
#define LCD_D4 (1 << 2)
#define LCD_D5 (1 << 3)
#define LCD_D6 (1 << 4)
#define LCD_D7 (1 << 5)

void lcd_init();
void lcd_cmd(u8 cmd);
void lcd_data(u8 data);
void lcd_print(char *s);

#endif
