#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <avr/io.h>

#define PORT_PIN_MAX_NUMBER  8U
#define PORT_MAX_NUMBER      3U

#define SET_BIT(REG, BIT)     ((REG) |= (unsigned char)(1U << (BIT)))
#define CLEAR_BIT(REG, BIT)   ((REG) &= (unsigned char)~(1U << (BIT)))
#define TOGGLE_BIT(REG, BIT)  ((REG) ^= (unsigned char)(1U << (BIT)))
#define READ_BIT(REG, BIT)    (((REG) >> (BIT)) & 1U)


typedef enum
{
    GPIO_LOW = 0,
    GPIO_HIGH

} logic_t;


typedef enum
{
    GPIO_DIRECTION_INPUT = 0,
    GPIO_DIRECTION_OUTPUT

} direction_t;


typedef enum
{
    GPIO_PIN0 = 0,
    GPIO_PIN1,
    GPIO_PIN2,
    GPIO_PIN3,
    GPIO_PIN4,
    GPIO_PIN5,
    GPIO_PIN6,
    GPIO_PIN7

} pin_index_t;


typedef enum
{
    PORTB_INDEX = 0,
    PORTC_INDEX,
    PORTD_INDEX

} port_index_t;


typedef struct
{
    port_index_t port;
    pin_index_t pin;
    direction_t direction;
    logic_t logic;

} pin_config_t;


void gpio_pin_direction_initialize(const pin_config_t *pin_config);

void gpio_pin_write_logic(const pin_config_t *pin_config, logic_t logic);

void gpio_pin_read_logic(const pin_config_t *pin_config, logic_t *logic);

void gpio_pin_toggle_logic(const pin_config_t *pin_config);

void gpio_pin_initialize(const pin_config_t *pin_config);

#endif
