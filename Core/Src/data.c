#include "data.h"

uint8_t voltBuffer[VOLT_BUFFER_SIZE] = {0};
uint16_t adc1_value[4] = {0};  // ADC1: 4 channels
uint16_t adc2_value[3] = {0};  // ADC2: 3 channels  
uint16_t adc5_value[1] = {0};  // ADC5: 1 channel

void dataProcess() {
  // 禁用中斷以確保資料一致性
  __disable_irq();

  // 簡單的DMA數據複製 (8通道完整系統)
  memcpy(&voltBuffer[0], adc1_value, 8);   // ADC1: channels 0-3
  memcpy(&voltBuffer[8], adc2_value, 6);   // ADC2: channels 4-6
  memcpy(&voltBuffer[14], adc5_value, 2);  // ADC5: channel 7

  __enable_irq();
}