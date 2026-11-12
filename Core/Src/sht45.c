
#include "stm32l4xx_hal.h"
#include "sht45.h"

//static const bo te dane mają być tylko tutaj i w czasie rozwoju biloteki
//dla reszty czujników bedą sie powtarzać
static const uint32_t Timeout = 100;
static const uint16_t Dev_Adress = 0x44;
static const uint16_t Buffor_Size = 6; // 6 dlatego że otrzymujemy 6 bajtów od czujnika 4 z danymi 2 z CRC


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

    if (data == NULL) {
        return HAL_ERROR;
    }

    if (hi2c == NULL) {
        return HAL_ERROR;
    }

    return HAL_I2C_Master_Receive(hi2c, Dev_Adress << 1, data, Buffor_Size, Timeout);
}

/**
 * @brief  Oblicza nam CRC na postawie algorytmu z czyjnika SHT45.
 * @param  data jest wskjażnik do arrya gdzie jest otrzymana data.
 * pod wskaźnikiem muszą być 3 bajty,można przekazać środek tablicy np &data[3]
 * a tablica posida 6 elemtów , fuckja beðzie dizłać na tych elemtach
 * data[3] - dane MSB
 * data[4] - ane LSB
 * data[5] - CRC przysłane przez czujnik
 * gdzie 5 to CRC z czyjnika , po przejsciu prze wszyskie bity dla data[3] oraz
 * data[4] porówujemy nasze CRC_check z data[5]
 * @return HAL_OK gdy CRC się zgadza, HAL_ERROR przy złym CRC.
 */
HAL_StatusTypeDef SHT45_ChkCRC(uint8_t *data){


	uint8_t CRC_check;
	CRC_check = 0xFF; // wartośc starotwa z dokumetacji

	for (int i = 0 ; i < 2; i++) {

		CRC_check ^= data[i];

		for (int t = 0 ; t < 8; t++){

			if (CRC_check & 0x80) { // maska dla najstarszego bitu

				CRC_check <<= 1;
				CRC_check ^= 0x31;// Plunomial z datashetu


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

float SHT45_CalcT(uint8_t *data){


	uint16_t T_RAW = ((uint16_t)data[0] << 8) | data[1] ;

	return -45 + 175 * T_RAW/65535.0;

}


float SHT45_CalcRh(uint8_t *data){

	uint16_t RH_RAW = ((uint16_t)data[0] << 8) | data[1] ;

	return -6 +125 * RH_RAW/65535.0;

}



