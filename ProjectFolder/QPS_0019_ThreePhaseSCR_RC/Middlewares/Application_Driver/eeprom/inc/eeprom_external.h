/*
 * eeprom_external.h
 *
 *  Created on: Aug 17, 2022
 *      Author: vishweshgm
 */

#ifndef APPLICATION_DRIVER_EEPROM_INC_EEPROM_EXTERNAL_H_
#define APPLICATION_DRIVER_EEPROM_INC_EEPROM_EXTERNAL_H_

#include "main.h"

#define EEPROM_USED 1
// Define the Page Size and number of pages
#if EEPROM_USED == 1 /*AT24CM02*/
#define ATMEL_EEP_CHIP_PAGE_SIZE 256     // in Bytes
#define ATMEL_EEP_CHIP_PAGE_NUM  1024    // number of pages
#elif EEPROM_USED == 2
#endif

typedef enum{
	EXTEEP_OK,
	EXTEEP_BUSY,
	EXTEEP_FAILED
}ExtEEP_Error;

ExtEEP_Error EEPROM_Write (uint16_t page, uint16_t offset, uint8_t *data, uint16_t size,uint8_t eepchipaddress);
ExtEEP_Error EEPROM_Read (uint16_t page, uint16_t offset, uint8_t *data, uint16_t size,uint8_t eepchipaddress);
ExtEEP_Error EEPROM_PageErase (uint16_t page,uint8_t eepchipaddress);

void EEPROM_Write_NUM (uint16_t page, uint16_t offset, float  fdata,uint8_t eepchipaddress);
float EEPROM_Read_NUM (uint16_t page, uint16_t offset,uint8_t eepchipaddress);

#endif /* APPLICATION_DRIVER_EEPROM_INC_EEPROM_EXTERNAL_H_ */
