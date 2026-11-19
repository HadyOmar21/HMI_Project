
#include "Chr_LCD.h"
#include <Arduino.h>

/* Send one 4-bit nibble to LCD */

static void lcd_write_nibble(lcd_4bit_t *lcd, unsigned char value)
{
    unsigned char bit_index;

    for (bit_index = 0U; bit_index < 4U; bit_index++)
    {
        if (value & (1U << bit_index))
        {
            gpio_pin_write_logic(
                &lcd->lcd_data[bit_index], GPIO_HIGH
            );
        }
        else
        {
            gpio_pin_write_logic(
                &lcd->lcd_data[bit_index], GPIO_LOW
            );
        }
    }
}


/* Generate Enable pulse */

static void lcd_enable(lcd_4bit_t *lcd)
{
    gpio_pin_write_logic(&lcd->lcd_en, GPIO_LOW);
    delayMicroseconds(1);

    gpio_pin_write_logic(&lcd->lcd_en, GPIO_HIGH);
    delayMicroseconds(1);

    gpio_pin_write_logic(&lcd->lcd_en, GPIO_LOW);
    delayMicroseconds(50);
}


/* Send one complete 8-bit byte using two 4-bit transfers */

static void lcd_write_byte(lcd_4bit_t *lcd, unsigned char value)
{
    // Send upper nibble first
    lcd_write_nibble(lcd, (value >> 4) & 0x0FU);
    lcd_enable(lcd);

    // Send lower nibble
    lcd_write_nibble(lcd, value & 0x0FU);
    lcd_enable(lcd);
}


/* Send command */

void lcd_4bit_send_command(lcd_4bit_t *lcd, unsigned char command)
{
    gpio_pin_write_logic(&lcd->lcd_rs, GPIO_LOW);

    lcd_write_byte(lcd, command);

    if (command == LCD_Clear ||
        command == LCD_Return_Home)
    {
        delay(2);
    }
}


/* Send data */

void lcd_4bit_send_data(lcd_4bit_t *lcd, unsigned char data)
{
    gpio_pin_write_logic(&lcd->lcd_rs, GPIO_HIGH);

    lcd_write_byte(lcd, data);
}


/* Set LCD cursor position */

static void lcd_set_cursor(lcd_4bit_t *lcd,
                           unsigned char row,
                           unsigned char column)
{
    unsigned char address;

    if (row == 1U)
    {
        address = column - 1U;
    }
    else
    {
        address = 0x40U + column - 1U;
    }

    lcd_4bit_send_command(
        lcd,
        LCD_DDRAM_Start | address
    );
}


/* Send string */

void lcd_4bit_send_string(lcd_4bit_t *lcd, const char *str)
{
    while (*str != '\0')
    {
        lcd_4bit_send_data(lcd, (unsigned char)*str);
        str++;
    }
}


/* Send string at a specific position */

void lcd_4bit_send_string_pos(lcd_4bit_t *lcd,
                              unsigned char row,
                              unsigned char column,
                              const char *str)
{
    lcd_set_cursor(lcd, row, column);

    while (*str != '\0' && column <= lcd->columns)
    {
        lcd_4bit_send_data(lcd, (unsigned char)*str);

        str++;
        column++;
    }
}


/* Initialize LCD in 4-bit mode */

void lcd_4bit_initialization(lcd_4bit_t *lcd)
{
    unsigned char index;

    // Initialize RS and Enable pins
    gpio_pin_initialize(&lcd->lcd_rs);
    gpio_pin_initialize(&lcd->lcd_en);

    // Initialize four data pins
    for (index = 0U; index < 4U; index++)
    {
        gpio_pin_initialize(&lcd->lcd_data[index]);
    }

    gpio_pin_write_logic(&lcd->lcd_rs, GPIO_LOW);
    gpio_pin_write_logic(&lcd->lcd_en, GPIO_LOW);

    delay(50);

    // 4-bit initialization sequence

    lcd_write_nibble(lcd, 0x03U);
    lcd_enable(lcd);
    delay(5);

    lcd_write_nibble(lcd, 0x03U);
    lcd_enable(lcd);
    delayMicroseconds(150);

    lcd_write_nibble(lcd, 0x03U);
    lcd_enable(lcd);
    delayMicroseconds(150);

    // Switch to 4-bit mode
    lcd_write_nibble(lcd, 0x02U);
    lcd_enable(lcd);
    delayMicroseconds(150);

    // Function Set: 4-bit, 2 lines, 5x8 font
    lcd_4bit_send_command(lcd, LCD_4_bit_mode_2_lines);

    // Display OFF
    lcd_4bit_send_command(lcd, 0x08U);

    // Clear Display
    lcd_4bit_send_command(lcd, LCD_Clear);

    // Entry Mode
    lcd_4bit_send_command(lcd, LCD_Entry_Mode);

    // Display ON, Cursor OFF
    lcd_4bit_send_command(
        lcd, LCD_Cursor_off_Display_On
    );
}
