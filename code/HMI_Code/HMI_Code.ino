#include <Arduino.h>
#include <stdio.h>

#include "hal_gpio.h"
#include "led.h"
#include "uart.h"
#include "adc.h"
#include "lm35.h"
#include "Chr_LCD.h"

#define INITIAL_MAX_TEMPERATURE  25U
#define INITIAL_MIN_TEMPERATURE  20U

#define BUTTON_HOLD_TIME              1000UL
#define BUTTON_REPEAT_INTERVAL        1000UL
#define TEMPERATURE_UPDATE_INTERVAL   1000UL

int main()
{
    init();

    /* ---------------- Pin configuration ---------------- */

    static pin_config_t max_button = {
        PORTB_INDEX, GPIO_PIN1, GPIO_DIRECTION_INPUT, GPIO_LOW
    };

    static pin_config_t min_button = {
        PORTB_INDEX, GPIO_PIN0, GPIO_DIRECTION_INPUT, GPIO_LOW
    };

    static pin_config_t alert_led = {
        PORTB_INDEX, GPIO_PIN4, GPIO_DIRECTION_OUTPUT, GPIO_LOW
    };

    /* ---------------- ADC and LM35 configuration ---------------- */

    static adc_config_t adc = {
        A0, ADC_REFERENCE_DEFAULT
    };

    static lm35_t temperature_sensor = {
        &adc,
        A0,
        LM35_DEFAULT_REFERENCE_MV,
        LM35_DEFAULT_AVERAGE_SAMPLES,
        0U,
        0U
    };

    /* ---------------- LCD configuration ---------------- */

    static lcd_4bit_t lcd = {
        { PORTB_INDEX, GPIO_PIN2, GPIO_DIRECTION_OUTPUT, GPIO_LOW },
        { PORTB_INDEX, GPIO_PIN3, GPIO_DIRECTION_OUTPUT, GPIO_LOW },
        {
            { PORTD_INDEX, GPIO_PIN4, GPIO_DIRECTION_OUTPUT, GPIO_LOW },
            { PORTD_INDEX, GPIO_PIN5, GPIO_DIRECTION_OUTPUT, GPIO_LOW },
            { PORTD_INDEX, GPIO_PIN6, GPIO_DIRECTION_OUTPUT, GPIO_LOW },
            { PORTD_INDEX, GPIO_PIN7, GPIO_DIRECTION_OUTPUT, GPIO_LOW }
        },
        2U,
        16U,
    };

    /* ---------------- Application variables ---------------- */

    static uint8_t maximum_temperature = INITIAL_MAX_TEMPERATURE;
    static uint8_t minimum_temperature = INITIAL_MIN_TEMPERATURE;

    static logic_t max_button_state = GPIO_LOW;
    static logic_t old_max_button_state = GPIO_LOW;

    static logic_t min_button_state = GPIO_LOW;
    static logic_t old_min_button_state = GPIO_LOW;

    static uint16_t temperature_x10 = 0U;
    static uint8_t alarm = 0U;

    static unsigned long max_button_press_time = 0UL;
    static unsigned long max_button_last_repeat = 0UL;

    static unsigned long min_button_press_time = 0UL;
    static unsigned long min_button_last_repeat = 0UL;

    static bool max_button_hold_active = false;
    static bool min_button_hold_active = false;

    static unsigned long previous_sensor_update = 0UL;

    static char lcd_line1[17];
    static char lcd_line2[17];
    static char uart_message[96];

    /* ---------------- Initialize modules ---------------- */

    Uart_Init();

    gpio_pin_initialize(&max_button);
    gpio_pin_initialize(&min_button);

    led_initialize(&alert_led);

    ADC_Init(&adc);
    LM35_Init(&temperature_sensor);

    lcd_4bit_initialization(&lcd);

    lcd_4bit_send_string_pos(
        &lcd, 1U, 1U, "Temperature HMI"
    );

    lcd_4bit_send_string_pos(
        &lcd, 2U, 1U, "LL:20 LH:50     "
    );

    Uart_SendString("Temperature HMI Started\r\n", 26U);

    /* ---------------- Main program ---------------- */

    while (1)
    {
        /* 1. MAX / LH button */

        gpio_pin_read_logic(&max_button, &max_button_state);

        /* Detect button press */
        if (max_button_state == GPIO_HIGH &&
            old_max_button_state == GPIO_LOW)
        {
            delay(25);

            gpio_pin_read_logic(&max_button, &max_button_state);

            if (max_button_state == GPIO_HIGH)
            {
                max_button_press_time = millis();
                max_button_last_repeat = millis();
                max_button_hold_active = false;
            }
        }

        /* Detect long press */
        if (max_button_state == GPIO_HIGH)
        {
            if (!max_button_hold_active &&
                millis() - max_button_press_time >= BUTTON_HOLD_TIME)
            {
                // Hold LH button: decrease LH

                if (maximum_temperature > 0U)
                {
                    maximum_temperature--;
                }

                max_button_hold_active = true;
                max_button_last_repeat = millis();
            }
            else if (max_button_hold_active &&
                     millis() - max_button_last_repeat >= BUTTON_REPEAT_INTERVAL)
            {
                // Continue decreasing LH while holding

                if (maximum_temperature > 0U)
                {
                    maximum_temperature--;
                }

                max_button_last_repeat = millis();
            }
        }

        /* Detect normal short press */
        else if (old_max_button_state == GPIO_HIGH)
        {
            delay(25);

            gpio_pin_read_logic(&max_button, &max_button_state);

            if (max_button_state == GPIO_LOW &&
                !max_button_hold_active)
            {
                // Short press: increase LH

                if (maximum_temperature < 99U)
                {
                    maximum_temperature++;
                }
            }
        }

        old_max_button_state = max_button_state;


        /* 2. MIN / LL button */

        gpio_pin_read_logic(&min_button, &min_button_state);

        /* Detect button press */
        if (min_button_state == GPIO_HIGH &&
            old_min_button_state == GPIO_LOW)
        {
            delay(25);

            gpio_pin_read_logic(&min_button, &min_button_state);

            if (min_button_state == GPIO_HIGH)
            {
                min_button_press_time = millis();
                min_button_last_repeat = millis();
                min_button_hold_active = false;
            }
        }

        /* Detect long press */
        if (min_button_state == GPIO_HIGH)
        {
            if (!min_button_hold_active &&
                millis() - min_button_press_time >= BUTTON_HOLD_TIME)
            {
                // Hold LL button: decrease LL

                if (minimum_temperature > 0U)
                {
                    minimum_temperature--;
                }

                min_button_hold_active = true;
                min_button_last_repeat = millis();
            }
            else if (min_button_hold_active &&
                     millis() - min_button_last_repeat >= BUTTON_REPEAT_INTERVAL)
            {
                // Continue decreasing LL while holding

                if (minimum_temperature > 0U)
                {
                    minimum_temperature--;
                }

                min_button_last_repeat = millis();
            }
        }

        /* Detect normal short press */
        else if (old_min_button_state == GPIO_HIGH)
        {
            delay(25);

            gpio_pin_read_logic(&min_button, &min_button_state);

            if (min_button_state == GPIO_LOW &&
                !min_button_hold_active)
            {
                // Short press: increase LL

                if (minimum_temperature < 99U)
                {
                    minimum_temperature++;
                }
            }
        }

        old_min_button_state = min_button_state;


        /* 3. Update temperature every 1 second */

        if (millis() - previous_sensor_update >=
            TEMPERATURE_UPDATE_INTERVAL)
        {
            previous_sensor_update = millis();

            /* Read LM35 */

            LM35_ReadTemperatureX10(
                &temperature_sensor,
                &temperature_x10
            );

            /* 4. Check temperature limits */

            if (temperature_x10 <
                ((uint16_t)minimum_temperature * 10U) ||
                temperature_x10 >
                ((uint16_t)maximum_temperature * 10U))
            {
                led_turn_on(&alert_led);
                alarm = 1U;
            }
            else
            {
                led_turn_off(&alert_led);
                alarm = 0U;
            }

            /* 5. Update LCD */

            sprintf(lcd_line1, "T:%3u.%uC %-6s ",
                    (unsigned int)(temperature_x10 / 10U),
                    (unsigned int)(temperature_x10 % 10U),
                    alarm ? "ALERT" : "NORMAL");

            sprintf(lcd_line2, "LL:%2u LH:%2u     ",
                    (unsigned int)minimum_temperature,
                    (unsigned int)maximum_temperature);

            lcd_4bit_send_string_pos(
                &lcd, 1U, 1U, lcd_line1
            );

            lcd_4bit_send_string_pos(
                &lcd, 2U, 1U, lcd_line2
            );

            /* 6. Send UART reading every 1 second */

            sprintf(uart_message,
                    "ADC=%u | V=%u mV | TEMP=%u.%u C | MIN=%u | MAX=%u\r\n",
                    (unsigned int)temperature_sensor.last_adc_raw,
                    (unsigned int)temperature_sensor.last_voltage_mv,
                    (unsigned int)(temperature_x10 / 10U),
                    (unsigned int)(temperature_x10 % 10U),
                    (unsigned int)minimum_temperature,
                    (unsigned int)maximum_temperature);

            Uart_SendString(
                uart_message,
                sizeof(uart_message)
            );
        }
    }
}