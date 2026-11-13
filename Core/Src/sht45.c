#include "sht45.h"


// These values are used only inside this file.
// They cannot be changed during program work.
static const uint32_t timeout = 100;
static const uint16_t dev_adress = 0x44;
static const uint16_t buff_size = 6; // this is the size of data we get from sensor 6 bajts

/**
 * @brief Checks if the SHT45 sensor is connected and working.
 * @param hi2c Pointer to the I2C handle.
 * @return HAL_OK       -> operation successful
 *         HAL_ERROR    -> operation failed
 *         HAL_BUSY     -> I2C peripheral busy (another transfer in progress)
 *         HAL_TIMEOUT  -> operation did not complete within the timeout
 */
HAL_StatusTypeDef SHT45_InitSensor(I2C_HandleTypeDef *hi2c)
{
    return HAL_I2C_IsDeviceReady(hi2c, dev_adress << 1, 1, timeout);
}
/**
 * @brief Sends a command to the SHT45 sensor.
 * @param hi2c Pointer to the I2C handle.
 * @param command is values i hex that we send to sensor e.g.:
 * 0x94 -> Soft reset
 * 0xFD -> measure T & RH with high precision (high repeatability)
 * 0xF6 -> measure T & RH with medium precision (medium repeatability)
 * 0xE0 -> measure T & RH with lowest precision (low repeatability)
 * 0x39 -> activate heater with 200mW for 1s,
 * including a high precision measurement just before deactivation
 * 0x2F -> activate heater with 110mW for 1s
 * including a high precision measurement just before deactivation
 * for the rest of commends check datasheet of SHT45
 * @return HAL_OK       -> operation successful
 *         HAL_ERROR    -> operation failed
 *         HAL_BUSY     -> I2C peripheral busy (another transfer in progress)
 *         HAL_TIMEOUT  -> operation did not complete within the timeout .
 */
HAL_StatusTypeDef SHT45_SND_Command(I2C_HandleTypeDef *hi2c, uint8_t command)
{
    return HAL_I2C_Master_Transmit(hi2c, dev_adress << 1, &command, 1, Timeout);
}

/**
 * @brief  Reads measurement data from the SHT45 sensor
 * @param  hi2c Pointer to the I2C handle.
 * @param  data to the buffer where received data will be stored.
 * from sensor we get [2 * 8-bit T-data; 8-bit CRC; 2 * 8-bit RH-data; 8-bit CRC]
 * the requierd size for the array is 6
 * Data format:
 * [0] Temperature MSB
 * [1] Temperature LSB
 * [2] Temperature CRC
 * [3] Humidity MSB
 * [4] Humidity LSB
 * [5] Humidity CRC
 * before SHT45_RCV_Data is needed to wait specifed time after the measurment:
 * for low    repeatability -> 1.6ms
 * for medium repeatability -> 4.5ms
 * for high   repeatability -> 8.3ms
 * when measuring with heater we need to wait 1.1s also the heater can not work longer than 10%
 * of sensor cycle  e.g.: when measuring for 1s with heater then this is 10% of 10s
 * so next measurment with it can happen in 10 s
 * this values are MAX ones
 * @return HAL_OK       -> operation successful
 *         HAL_ERROR    -> operation failed
 *         HAL_BUSY     -> I2C peripheral busy (another transfer in progress)
 *         HAL_TIMEOUT  -> operation did not complete within the timeout .
 */
HAL_StatusTypeDef SHT45_RCV_Data(I2C_HandleTypeDef *hi2c, uint8_t *data)
{

    if (data == NULL) {
        return HAL_ERROR;
    }

    if (hi2c == NULL) {
        return HAL_ERROR;
    }

    return HAL_I2C_Master_Receive(hi2c, dev_adress << 1, data, buff_size, timeout);
}

/**
 * @brief  Check the CRC of the received data.
 * @param  data is an pointer to 3 bytes
 * data[0] - MSB(Most Significant Byte)
 * data[1] - LSB(Lest Significant Byte)
 * data[2] - CRC from sensor
 * example: &rx[0] for temperature, &rx[3] for humidity.
 * @return HAL_OK if CRC is ok else HALL_ERROR.
 */
HAL_StatusTypeDef SHT45_ChkCRC(const uint8_t *data){


	uint8_t CRC_check;
	CRC_check = 0xFF; //Start value from document

	for (int i = 0 ; i < 2; i++) {
		// For every byte XOR it with CRC_check value

		CRC_check ^= data[i];

		for (int t = 0 ; t < 8; t++){
			//For every MSB bit check if its 1,if 1 : shift left and XOR with the polynomial
			//else just shift left.

			if (CRC_check & 0x80) { // mask for MSB

				CRC_check <<= 1;
				CRC_check ^= 0x31;// Polynomial from documentation


			}else {

				CRC_check <<= 1;
			}

		}
	}


	if (CRC_check == data[2] ) {
		return HAL_OK;
	}else {
		return HAL_ERROR;
	}

}
/**
 * @brief Calculates the Temperature[T]
 * @param data it's pointer to the received data.
 * the pointer data needs at least 2 bytes, it is possible to point into
 * middle of the array data
 * @return Temperature in °C.
 */
float SHT45_CalcT(const uint8_t *data){


	uint16_t t_raw = ((uint16_t)data[0] << 8) | data[1] ;

	return -45 + 175 * t_raw/65535.0f;

}

/**
 * @brief Calculates the Humidity[Rh]
 * @param data it's pointer to the received data.
 * the pointer data needs at least 2 bytes it is possible to point into
 * middle of the array data
 * @return Humidity in %RH.
 */
float SHT45_CalcRh( const uint8_t *data){

	uint16_t rh_raw = ((uint16_t)data[0] << 8) | data[1] ;
	float rh_cal = -6 +125 * rh_raw/65535.0f;


	if (rh_cal > 100){

		rh_cal = 100.0f;

	}else if (rh_cal < 0){

		rh_cal = 0.0f;
	}

	return rh_cal;
}

