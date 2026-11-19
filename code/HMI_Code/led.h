#ifndef LED_H
#define LED_H

#include "hal_gpio.h"

void led_initialize(const pin_config_t *led_pin);
void led_turn_on(const pin_config_t *led_pin);
void led_turn_off(const pin_config_t *led_pin);

#endif
