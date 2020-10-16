#include "main.h"

#define DEBUG				0

#if (DEBUG == 1)
#include <stdio.h>
#endif


static uint32_t address = STORAGE_START;


void write_storage(void)
{
	uint16_t temp_u16;
	uint32_t temp_u32;

	nvs_key_id.f_head = 0x7E;
	nvs_key_id.s_head = 0x7D;
	nvs_key_id.chk_sum = 0x00;

	if(*(__IO uint32_t*)(STORAGE_END-4) != 0xFFFFFFFF){
		erase_storage();
		address = STORAGE_START;
	}

	while(address <= STORAGE_END-4){
		if(*(__IO uint32_t*)address == 0xFFFFFFFF){
			HAL_FLASH_Unlock();

#if(DEBUG == 1)
	printf("write storage flash, address %08x\r\n", (unsigned int)address);
#endif

			temp_u16 = (nvs_key_id.f_head<<8)+nvs_key_id.s_head;
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, temp_u16);
			address+=2;

			temp_u32 = (nvs_key_id.app_crc<<24)+(nvs_key_id.app_crc<<16)+(nvs_key_id.app_crc<<8)+nvs_key_id.app_crc;
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, temp_u32);
			address+=4;

			temp_u32 = (nvs_key_id.id_relay1<<24)+(nvs_key_id.id_relay2<<16)+(nvs_key_id.id_relay3<<8)+nvs_key_id.id_relay4;
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, temp_u32);
			address+=4;

			temp_u32 = (nvs_key_id.relay_setup<<24)+(nvs_key_id.light_setup<<16)+(nvs_key_id.chk_sum<<8)+nvs_key_id.chk_sum;
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, temp_u32);
			address+=4;

			HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, 0x00);
			address+=2;

			HAL_FLASH_Lock();
			break;
		}
		address+=4;
	}

	if(address > STORAGE_END-4){
		address=STORAGE_START;
	}

}

void read_storage(void)
{
	uint32_t temp_address=STORAGE_END-4;

	nvs_key_id.f_head = 0x7E;
	nvs_key_id.s_head = 0x7D;
	nvs_key_id.chk_sum = 0x00;

	while(temp_address > STORAGE_START){
		if(*(__IO uint8_t*)temp_address == nvs_key_id.f_head){
			if(*(__IO uint8_t*)(temp_address-1) == nvs_key_id.s_head){

#if(DEBUG == 1)
	printf("read storage flash, address %08x\r\n", (unsigned int)temp_address);
#endif
				/* next 4 byte */
				temp_address+=4;
				nvs_key_id.app_crc = (*(__IO uint8_t*)(temp_address))>>24;
				nvs_key_id.app_crc = (*(__IO uint8_t*)(temp_address-1))>>16;
				nvs_key_id.app_crc = (*(__IO uint8_t*)(temp_address-2))>>8;
				nvs_key_id.app_crc = *(__IO uint8_t*)(temp_address-3);

				/* next 4 byte */
				temp_address+=4;
				nvs_key_id.id_relay1 = *(__IO uint8_t*)(temp_address);
				nvs_key_id.id_relay2 = *(__IO uint8_t*)(temp_address-1);
				nvs_key_id.id_relay3 = *(__IO uint8_t*)(temp_address-2);
				nvs_key_id.id_relay4 = *(__IO uint8_t*)(temp_address-3);

				/* next 4 byte */
				temp_address+=4;
				nvs_key_id.relay_setup = *(__IO uint8_t*)(temp_address);
				nvs_key_id.light_setup = *(__IO uint8_t*)(temp_address-1);
				nvs_key_id.chk_sum = (*(__IO uint8_t*)(temp_address-2))>>8;
				nvs_key_id.chk_sum = *(__IO uint8_t*)(temp_address-3);
			}
			break;
		}
		temp_address-=1;
	}
}

void erase_storage(void)
{
	FLASH_EraseInitTypeDef f;
	uint32_t PageError = 0;

	HAL_FLASH_Unlock();

	f.TypeErase = FLASH_TYPEERASE_PAGES;
	f.PageAddress = STORAGE_START;
	f.NbPages = STORAGE_SIZE / PAGES;

	HAL_FLASHEx_Erase(&f, &PageError);
#if(DEBUG == 1)
	printf("erase %lX, rst=%lX\r\n", STORAGE_START, PageError);
#endif

	HAL_FLASH_Lock();
}
