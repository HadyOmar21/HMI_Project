
#ifndef CHR_LCD_H
#define CHR_LCD_H

#include "hal_gpio.h"

#define LCD_Clear                         0x01U
#define LCD_Return_Home                   0x02U
#define LCD_Entry_Mode                    0x06U
#define LCD_Cursor_off_Display_On         0x0CU
#define LCD_4_bit_mode_2_lines            0x28U
#define LCD_DDRAM_Start                   0x80U

typedef struct
{
    pin_config_t lcd_rs;
    pin_config_t lcd_en;
    pin_config_t lcd_data[4];

    unsigned char rows;
    unsigned char columns;

} lcd_4bit_t;


void lcd_4bit_initialization(lcd_4bit_t *lcd);

void lcd_4bit_send_command(lcd_4bit_t *lcd,
                           unsigned char command);

void lcd_4bit_send_data(lcd_4bit_t *lcd,
                        unsigned char data);

void lcd_4bit_send_string(lcd_4bit_t *lcd,
                          const char *str);

void lcd_4bit_send_string_pos(lcd_4bit_t *lcd,
                              unsigned char row,
                              unsigned char column,
                              const char *str);

#endif
