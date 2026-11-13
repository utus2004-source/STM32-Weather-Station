#ifndef SHT45_H // include guard
#define SHT45_H
#include "stm32l4xx_hal.h"

//SHT45 temperature and humidity sensor driver

HAL_StatusTypeDef SHT45_InitSensor(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef SHT45_SND_Command(I2C_HandleTypeDef *hi2c ,uint8_t command);
HAL_StatusTypeDef SHT45_RCV_Data(I2C_HandleTypeDef *hi2c ,uint8_t *data);
float SHT45_CalcT(const uint8_t *data);
float SHT45_CalcRh(const uint8_t *data);
HAL_StatusTypeDef SHT45_ChkCRC(const uint8_t *data);

#endif
