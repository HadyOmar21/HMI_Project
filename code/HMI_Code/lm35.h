
#ifndef LM35_H
#define LM35_H

#include "adc.h"

#define LM35_DEFAULT_REFERENCE_MV    5000U
#define LM35_DEFAULT_AVERAGE_SAMPLES 64U

typedef struct
{
    adc_config_t *adc;

    unsigned char adc_channel;
    unsigned int reference_voltage_mv;
    unsigned char average_samples;

    unsigned int last_adc_raw;
    unsigned int last_voltage_mv;

} lm35_t;


void LM35_Init(lm35_t *sensor);

void LM35_ReadRaw(lm35_t *sensor,
                  unsigned int *adc_value);

void LM35_ReadTemperatureX10(lm35_t *sensor,
                             unsigned int *temperature_x10);

#endif
