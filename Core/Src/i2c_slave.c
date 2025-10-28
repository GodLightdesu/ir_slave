#include "i2c_slave.h"
#include "stm32g4xx_hal_i2c.h"

#define SLAVE_RX_BUF_SIZE 32    // 8 * 4 bytes
#define SLAVE_TX_BUF_SIZE 32    // 8 * 4 bytes
static uint8_t Rxbuffer[SLAVE_RX_BUF_SIZE] = {0};
static uint8_t Txbuffer[SLAVE_TX_BUF_SIZE] = {0};

static I2C_HandleTypeDef* m_hi2c;

// 初始化 I2C 從設備
void I2C_Slave_Init(I2C_HandleTypeDef *hi2c) {
  m_hi2c = hi2c;
  HAL_I2C_EnableListen_IT(m_hi2c);

  // 預先準備tx資料
  for (uint8_t i = 0; i < SLAVE_TX_BUF_SIZE; i++) {
    Txbuffer[i] = i;
  }
}

// 取得接收 buffer 指標
uint8_t* I2C_Slave_GetRxBuffer(void) {
  return Rxbuffer;
}

// 取得傳送 buffer 指標
uint8_t* I2C_Slave_GetTxBuffer(void) {
  return Txbuffer;
}

/* I2C event callback */
void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode) {
  // if (hi2c->Instance != m_hi2c->Instance) { return; }

  // master 將寫資料到 slave → slave 應準備接收
  if (TransferDirection == I2C_DIRECTION_TRANSMIT) {
    // master 將寫資料到 slave → slave 應準備接收
    HAL_I2C_Slave_Seq_Receive_IT(
      m_hi2c, 
      Rxbuffer, 
      SLAVE_RX_BUF_SIZE,
      I2C_FIRST_AND_LAST_FRAME
    );
  } else {
    // master 從 slave 讀取資料 → slave 應準備傳送
    HAL_I2C_Slave_Seq_Transmit_IT(
      m_hi2c, 
      Txbuffer, 
      SLAVE_TX_BUF_SIZE,
      I2C_FIRST_AND_LAST_FRAME
    );
  }
}

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c) {
  // if (hi2c->Instance != m_hi2c->Instance) { return; }

  // 重新啟用 Listen 模式
  HAL_I2C_EnableListen_IT(m_hi2c);
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
  if (hi2c->Instance != m_hi2c->Instance) { return; }

  // 在此處處理接收到的資料，資料存儲在 Rxbuffer 中
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c) {
  if (hi2c->Instance != m_hi2c->Instance) { return; }

  // 在此處準備下一批要傳送的資料，資料存儲在 Txbuffer 中
  // 例如：更新感測器數值、狀態資訊等
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
  if (hi2c->Instance != m_hi2c->Instance) { return; }

  // I2C 錯誤處理
  // 可能的錯誤：HAL_I2C_ERROR_BERR, HAL_I2C_ERROR_ARLO, HAL_I2C_ERROR_AF 等
  
  // 重新啟用 Listen 模式
  HAL_I2C_EnableListen_IT(m_hi2c);
}