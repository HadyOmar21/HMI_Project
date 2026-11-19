
#include "hal_gpio.h"


static volatile unsigned char * const ddr_registers[PORT_MAX_NUMBER] = {
    &DDRB, &DDRC, &DDRD
};


static volatile unsigned char * const port_registers[PORT_MAX_NUMBER] = {
    &PORTB, &PORTC, &PORTD
};


static volatile unsigned char * const pin_registers[PORT_MAX_NUMBER] = {
    &PINB, &PINC, &PIND
};


void gpio_pin_direction_initialize(const pin_config_t *pin_config)
{
    if (pin_config->direction == GPIO_DIRECTION_OUTPUT)
        SET_BIT(*ddr_registers[pin_config->port], pin_config->pin);
    else
        CLEAR_BIT(*ddr_registers[pin_config->port], pin_config->pin);
}


void gpio_pin_write_logic(const pin_config_t *pin_config, logic_t logic)
{
    if (logic == GPIO_HIGH)
        SET_BIT(*port_registers[pin_config->port], pin_config->pin);
    else
        CLEAR_BIT(*port_registers[pin_config->port], pin_config->pin);
}


void gpio_pin_read_logic(const pin_config_t *pin_config, logic_t *logic)
{
    *logic = (logic_t)READ_BIT(
        *pin_registers[pin_config->port],
        pin_config->pin
    );
}


void gpio_pin_toggle_logic(const pin_config_t *pin_config)
{
    TOGGLE_BIT(*port_registers[pin_config->port], pin_config->pin);
}


void gpio_pin_initialize(const pin_config_t *pin_config)
{
    gpio_pin_write_logic(pin_config, pin_config->logic);

    gpio_pin_direction_initialize(pin_config);
}
