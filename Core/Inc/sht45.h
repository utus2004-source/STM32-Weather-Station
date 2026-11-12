#ifndef SHT45_H // to jest inculde Guard ma na celu zabezpieczyć fukcje
#define SHT45_H
#include "stm32l4xx_hal_def.h"
#include "stm32l4xx_hal_i2c.h"

HAL_StatusTypeDef SHT45_InitSensor(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef SHT45_SND_Command(I2C_HandleTypeDef *hi2c ,uint8_t command);
HAL_StatusTypeDef SHT45_RCV_Data(I2C_HandleTypeDef *hi2c ,uint8_t *data);
float SHT45_CalcT(uint8_t *data);
float SHT45_CalcRh(uint8_t *data);
HAL_StatusTypeDef SHT45_ChkCRC(uint8_t *data);

#endif
