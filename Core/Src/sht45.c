
#include "stm32l4xx_hal.h"
#include "sht45.h"

int Timeout = 100;
int Dev_Adress = 0x44;
int Buffor_Size = 6;



HAL_StatusTypeDef SHT45_InitSensor(I2C_HandleTypeDef *hi2c)
{
    return HAL_I2C_IsDeviceReady(hi2c, Dev_Adress << 1, 1, Timeout);
}



HAL_StatusTypeDef SHT45_SND_Command(I2C_HandleTypeDef *hi2c, uint8_t command)
{
    return HAL_I2C_Master_Transmit(hi2c, Dev_Adress << 1, &command, 1, Timeout);
}



HAL_StatusTypeDef SHT45_RCV_Data(I2C_HandleTypeDef *hi2c, uint8_t *data)
{

    if (Buffor_Size != 6) {
        return HAL_ERROR;
    }

    if (data == NULL) {
        return HAL_ERROR;
    }

    if (hi2c == NULL) {
        return HAL_ERROR;
    }

    return HAL_I2C_Master_Receive(hi2c, Dev_Adress << 1, data, Buffor_Size, Timeout);
}
