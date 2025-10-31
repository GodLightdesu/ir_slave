#ifndef DATA_H
#define DATA_H

#define VOLT_BUFFER_SIZE 16 // (1 Vref + 7 ir) * 2 bytes
#define EYE_NUM 7

#include "adc.h"
#include <string.h>

extern uint8_t voltBuffer[VOLT_BUFFER_SIZE];
extern uint16_t adc1_value[4];  // Vref, ch2, ch1, ch5
extern uint16_t adc2_value[3];  // ch3, ch4, ch15
extern uint16_t adc5_value[1];  // ch1

// 函數宣告
void adcInit(void);
void dataSave();
void dataProcess(uint8_t sampling_times);
void arrangeData();

#endif /* DATA_H */