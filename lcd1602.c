#include "lcd1602.h"

extern XGpio Gpio;
#define LCD_CH 1

static void lcd_pulse()
{
    XGpio_DiscreteSet(&Gpio, LCD_CH, LCD_EN);
    usleep(1);
    XGpio_DiscreteClear(&Gpio, LCD_CH, LCD_EN);
    usleep(100);
}

static void lcd_set_data(u8 nibble)
{
    XGpio_DiscreteClear(&Gpio, LCD_CH,
        LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7);

    if (nibble & 0x1) XGpio_DiscreteSet(&Gpio, LCD_CH, LCD_D4);
    if (nibble & 0x2) XGpio_DiscreteSet(&Gpio, LCD_CH, LCD_D5);
    if (nibble & 0x4) XGpio_DiscreteSet(&Gpio, LCD_CH, LCD_D6);
    if (nibble & 0x8) XGpio_DiscreteSet(&Gpio, LCD_CH, LCD_D7);

    lcd_pulse();
}

static void lcd_send(u8 data, int rs)
{
    if (rs) XGpio_DiscreteSet(&Gpio, LCD_CH, LCD_RS);
    else    XGpio_DiscreteClear(&Gpio, LCD_CH, LCD_RS);

    lcd_set_data(data >> 4);
    lcd_set_data(data & 0x0F);
    usleep(2000);
}

void lcd_cmd(u8 cmd) { lcd_send(cmd, 0); }
void lcd_data(u8 d)  { lcd_send(d, 1); }

void lcd_init()
{
    usleep(15000);

    lcd_set_data(0x3);
    usleep(5000);
    lcd_set_data(0x3);
    usleep(100);
    lcd_set_data(0x3);
    lcd_set_data(0x2);

    lcd_cmd(0x28); // 4-bit, 2 line
    lcd_cmd(0x0C); // display ON
    lcd_cmd(0x06); // entry
    lcd_cmd(0x01); // clear
}

void lcd_print(char *s)
{
    while (*s) lcd_data(*s++);
}
