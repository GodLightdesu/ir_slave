#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include "main.h"

// I2C Slave 初始化
void I2C_Slave_Init(I2C_HandleTypeDef *hi2c);

// 取得接收 buffer 指標（供外部讀取接收到的資料）
uint8_t* I2C_Slave_GetRxBuffer(void);

// 取得傳送 buffer 指標（供外部更新要傳送的資料）
uint8_t* I2C_Slave_GetTxBuffer(void);

#endif /* I2C_SLAVE_H */