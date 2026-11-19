#ifndef ADC_H
#define ADC_H

#include "hal_gpio.h"

typedef enum
{
    ADC_REFERENCE_DEFAULT = 0,
    ADC_REFERENCE_INTERNAL_1V1,
    ADC_REFERENCE_EXTERNAL

} adc_reference_t;


typedef struct
{
    unsigned char adc_channel;
    adc_reference_t reference;

} adc_config_t;


#define ADC_MAX_VALUE 1023U


void ADC_Init(adc_config_t *adc);

void ADC_SelectChannel(adc_config_t *adc,
                       unsigned char adc_channel);

void ADC_Get_ConversionResult(adc_config_t *adc,
                              unsigned int *conversion_result);

void ADC_Get_Conversion(adc_config_t *adc,
                        unsigned char adc_channel,
                        unsigned int *conversion_result);

#endif