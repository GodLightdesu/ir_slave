#ifndef DATA_H
#define DATA_H

#define VOLT_BUFFER_SIZE 16 // (1 Vref + 7 ir) * 2 bytes
#define EYE_NUM 7
#define ADC_CHANNELS 3

#include "adc.h"
#include <string.h>
#include "tim.h"

extern uint8_t voltBuffer[VOLT_BUFFER_SIZE];

// 函數宣告
void adcInit(void);
void dataSave();
void dataProcess();
void arrangeData();

#endif /* DATA_H */