#include "adc.h"
#include <Arduino.h>
#include <avr/io.h>


/* Get ADC channel index */

static unsigned char ADC_GetChannelIndex(unsigned char channel)
{
    return (unsigned char)(channel - A0);
}


/* Configure analog input pin */

static void ADC_ConfigureAnalogPin(unsigned char channel)
{
    unsigned char channel_index = ADC_GetChannelIndex(channel);

    pin_config_t analog_pin;

    analog_pin.port = PORTC_INDEX;
    analog_pin.pin = (pin_index_t)channel_index;
    analog_pin.direction = GPIO_DIRECTION_INPUT;
    analog_pin.logic = GPIO_LOW;

    gpio_pin_initialize(&analog_pin);

    DIDR0 |= (unsigned char)(1U << channel_index);
}


/* Initialize ADC */

void ADC_Init(adc_config_t *adc)
{
    unsigned char channel_index;
    unsigned char reference_bits;

    channel_index = ADC_GetChannelIndex(adc->adc_channel);

    if (adc->reference == ADC_REFERENCE_INTERNAL_1V1)
    {
        reference_bits = (1U << REFS1) | (1U << REFS0);
    }
    else if (adc->reference == ADC_REFERENCE_EXTERNAL)
    {
        reference_bits = 0U;
    }
    else
    {
        reference_bits = (1U << REFS0);
    }

    ADMUX = (unsigned char)(reference_bits | channel_index);

    ADCSRA = (1U << ADEN) |
             (1U << ADPS2) |
             (1U << ADPS1) |
             (1U << ADPS0);

    ADC_ConfigureAnalogPin(adc->adc_channel);

    delay(2);
}


/* Select ADC channel */

void ADC_SelectChannel(adc_config_t *adc,
                       unsigned char adc_channel)
{
    unsigned char channel_index;

    channel_index = ADC_GetChannelIndex(adc_channel);

    adc->adc_channel = adc_channel;

    ADMUX = (unsigned char)((ADMUX & 0xF0U) | channel_index);

    ADC_ConfigureAnalogPin(adc_channel);
}


/* Get conversion result */

void ADC_Get_ConversionResult(adc_config_t *adc,
                              unsigned int *conversion_result)
{
    ADCSRA |= (1U << ADSC);

    while (ADCSRA & (1U << ADSC))
    {
    }

    *conversion_result = ADC;
}


/* Perform ADC conversion */

void ADC_Get_Conversion(adc_config_t *adc,
                        unsigned char adc_channel,
                        unsigned int *conversion_result)
{
    ADC_SelectChannel(adc, adc_channel);

    ADC_Get_ConversionResult(adc, conversion_result);
}
