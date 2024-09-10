#ifndef __adc_t__h
#define __adc_t__h

#include <zephyr/logging/log.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/init.h>


typedef struct 
{
    int16_t adc_value;
}adc_data_t;


extern const struct adc_dt_spec adc_channel;
extern int16_t buf;
extern struct adc_sequence sequence;
extern  int val_mv ;
void adc_init(void);

#endif
