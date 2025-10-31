#include "data.h"

#define MAX_SAMPLES 10  // Maximum sampling buffer

uint8_t voltBuffer[VOLT_BUFFER_SIZE] = {0};
uint16_t adc1_value[4] = {0};  // ADC1: 4 channels
uint16_t adc2_value[3] = {0};  // ADC2: 3 channels
uint16_t adc5_value[1] = {0};  // ADC5: 1 channel

// Multi-sampling buffers
static uint16_t SampleBuffer[MAX_SAMPLES][EYE_NUM + 1] = {0};
static uint8_t SampleIndex = 0;
static uint8_t SampleCount = 0;

void dataSave() {
  // 禁用中斷以確保資料一致性
  __disable_irq();

  // 簡單的DMA數據複製 (8通道完整系統)
  memcpy(&voltBuffer[0], adc1_value, 8);   // ADC1: channels 0-3
  memcpy(&voltBuffer[8], adc2_value, 6);   // ADC2: channels 4-6
  memcpy(&voltBuffer[14], adc5_value, 2);  // ADC5: channel 7

  __enable_irq();
}

void dataProcess(uint8_t sampling_times) {
  // ==================== Step 1: Validate Parameters ====================
  if (sampling_times > MAX_SAMPLES) {
    sampling_times = MAX_SAMPLES;  // Limit to buffer size
  }
  if (sampling_times == 0) {
    return;  // Nothing to process
  }
  
  // ==================== Step 2: Store Current Sample ====================  
  // Copy 7 sensor values from ProcessBuffer to SampleBuffer (skip Vref at bytes 0-1)
  for (int sensor = 0; sensor < EYE_NUM; sensor++) {
    int byte_pos = (sensor + 1) * 2;  // Skip Vref (bytes 0-1), each sensor is 2 bytes
    uint8_t lsb = voltBuffer[byte_pos];
    uint8_t msb = voltBuffer[byte_pos + 1];
    SampleBuffer[SampleIndex][sensor] = (msb << 8) | lsb;  // Combine to 16-bit
  }
  
  // ==================== Step 3: Check Sample Collection Status ====================  
  if (SampleCount < sampling_times) {
    // Not enough samples yet - accumulate and wait
    SampleCount++;
    SampleIndex = (SampleIndex + 1) % sampling_times;  // Move to next position
    return;  // Exit early - no processing yet
  }
  
  // ==================== Step 4: Find Global Minimum (Ambient Light) ====================
  uint16_t global_min = 0xFFFF;  // Start with max value
  
  // Scan all samples of all sensors to find the minimum value (ambient light baseline)
  for (int sensor = 0; sensor < EYE_NUM; sensor++) {
    for (int sample = 0; sample < sampling_times; sample++) {
      uint16_t value = SampleBuffer[sample][sensor];
      if (value < global_min) {
        global_min = value;
      }
    }
  }
  
  // ==================== Step 5: Process Latest Data & Write Back ====================
  // Use SampleIndex because that's where we just wrote the latest sample
  for (int sensor = 0; sensor < EYE_NUM; sensor++) {
    // Get latest reading for this sensor
    uint16_t latest_value = SampleBuffer[SampleIndex][sensor];
    
    // Remove ambient light (subtract global minimum)
    uint16_t processed_value = (latest_value > global_min) ? (latest_value - global_min) : 0;
    
    // Write back to ProcessBuffer in little-endian format
    int byte_pos = (sensor + 1) * 2;  // Preserve Vref at bytes 0-1
    voltBuffer[byte_pos]     = processed_value & 0xFF;        // LSB
    voltBuffer[byte_pos + 1] = (processed_value >> 8) & 0xFF; // MSB
  }
  arrangeData();
  
  // ==================== Step 6: Update Index for Next Write ====================
  SampleIndex = (SampleIndex + 1) % sampling_times;  // Circular buffer
}

/*
eye: 1, 2, 3, 4, 5, 6, 7
buf: 1, 2, 4, 5, 3, 6, 7
*/
void arrangeData() {
  uint8_t arrangedBuf[VOLT_BUFFER_SIZE] = {0};

  // Copy Vref (bytes 0-1)
  arrangedBuf[0] = voltBuffer[0];
  arrangedBuf[1] = voltBuffer[1];

  // Rearranging according to the specified order
  for (int i = 0; i < EYE_NUM; i++) {
    int srcIndex;
    switch (i) {
      case 0: srcIndex = 1; break; // eye 1
      case 1: srcIndex = 2; break; // eye 2
      case 2: srcIndex = 4; break; // eye 3
      case 3: srcIndex = 5; break; // eye 4
      case 4: srcIndex = 3; break; // eye 5
      case 5: srcIndex = 6; break; // eye 6
      case 6: srcIndex = 7; break; // eye 7
      default: srcIndex = i + 1; break;
    }
    // Each sensor data is 2 bytes
    arrangedBuf[(i + 1) * 2]     = voltBuffer[srcIndex * 2];     // LSB
    arrangedBuf[(i + 1) * 2 + 1] = voltBuffer[srcIndex * 2 + 1]; // MSB
  }

  // Copy back to ProcessBuffer
  memcpy(voltBuffer, arrangedBuf, VOLT_BUFFER_SIZE);
}