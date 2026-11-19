
#include "lm35.h"
#include <Arduino.h>


/* Initialize LM35 */

void LM35_Init(lm35_t *sensor)
{
    ADC_SelectChannel(sensor->adc, sensor->adc_channel);
}


/* Read raw ADC value and calculate average */

void LM35_ReadRaw(lm35_t *sensor, unsigned int *adc_value)
{
    unsigned long adc_sum = 0UL;

    unsigned int sample_value;
    unsigned char sample;

    ADC_SelectChannel(sensor->adc, sensor->adc_channel);

    // Discard the first conversion after selecting A0.
    ADC_Get_ConversionResult(sensor->adc, &sample_value);

    // Read samples and calculate their average.
    for (sample = 0U;
         sample < sensor->average_samples;
         sample++)
    {
        ADC_Get_ConversionResult(sensor->adc, &sample_value);

        adc_sum += sample_value;

        delayMicroseconds(200);
    }

    *adc_value = (unsigned int)(
        (adc_sum + (sensor->average_samples / 2U)) /
        sensor->average_samples
    );

    sensor->last_adc_raw = *adc_value;
}


/* Read LM35 temperature */

void LM35_ReadTemperatureX10(lm35_t *sensor,
                             unsigned int *temperature_x10)
{
    unsigned int adc_value;
    unsigned long voltage_mv;

    LM35_ReadRaw(sensor, &adc_value);

    voltage_mv =
        ((unsigned long)adc_value *
         sensor->reference_voltage_mv + 511UL)
        / ADC_MAX_VALUE;

    sensor->last_voltage_mv = (unsigned int)voltage_mv;

    // LM35 output = 10 mV per degree Celsius.
    // Temperature X10 = Voltage in mV.

    *temperature_x10 = (unsigned int)voltage_mv;
}
