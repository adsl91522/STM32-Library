#include "stm32f0xx_hal.h"
#include "pt_storage.h"

#define DEBUG				0

#if (DEBUG == 1)
#include <stdio.h>
#endif


/* if storage is full
 * 1. copy current keys
 * 2. copy current value
 * 3. erash storage
 * 4. wirte current keys & value
 */
static void refresh_storage()
{
	uint8_t cp_flag = 0;
	uint8_t buf[4];
	uint8_t temp_key[256] = {0};
	uint32_t temp_value[256] = {0};
	uint16_t index = 0;
	uint32_t address = STORAGE_START;

	while(address <= STORAGE_END){
		buf[3] = (*(__IO uint32_t*)address)>>24;
		buf[2] = (*(__IO uint32_t*)address)>>16;
		buf[1] = (*(__IO uint32_t*)address)>>8;
		buf[0] = *(__IO uint32_t*)address;
		/* get and compare key id */
		for(int i=0; i<=index; i++){
			if(HEAD == buf[3] && temp_key[i] == buf[0]){
				cp_flag=0;
				break;
			}else{
				cp_flag=1;
			}
		}
		if(cp_flag == 1){
			temp_key[index]=buf[0];
			/* read key address value */
			temp_value[index] = read_storage(temp_key[index]);
#if(DEBUG == 1)
			printf("current key %02x\r\n", temp_key[index]);
			printf("current value %08x\r\n", (unsigned int)temp_value[index]);
#endif
			index++;
		}
		address+=8;
	}

#if(DEBUG == 1)
	printf("storage flash is full, current flash will write to refresh storage\r\n");
#endif

	erase_storage();

	for(int i=0; i<index; i++){
		/* write value */
		write_storage(temp_key[i] ,temp_value[i]);
	}
}

void write_storage(uint8_t key_id, uint32_t value)
{
	uint32_t address = STORAGE_START;
	uint32_t temp_u32 = (HEAD<<24)+(0x00<<16)+(0x00<<8)+key_id;

#if(DEBUG == 1)
	printf("write to storage flash, key %d, value %ld \r\n", key_id, value);
#endif

	if(*(__IO uint32_t*)STORAGE_END != 0xFFFFFFFF){
		refresh_storage();
		return;
	}

	while(address <= STORAGE_END){
		if(*(__IO uint32_t*)address == 0xFFFFFFFF){
			HAL_FLASH_Unlock();
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, temp_u32);
			address+=4;
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, value);
			address+=4;
			HAL_FLASH_Lock();
			break;
		}
		address+=4;
	}
}

uint32_t read_storage(uint8_t key_id)
{
	uint8_t buf[4];
	uint32_t address = STORAGE_START;
	uint32_t value;

#if(DEBUG == 1)
	printf("read storage flash, key %d\r\n", key_id);
#endif

	while(address <= STORAGE_END){
		buf[3] = (*(__IO uint32_t*)address)>>24;
		buf[2] = (*(__IO uint32_t*)address)>>16;
		buf[1] = (*(__IO uint32_t*)address)>>8;
		buf[0] = *(__IO uint32_t*)address;

		if(buf[3] == HEAD && buf[0] == key_id){
			address+=4;
			value=*(__IO uint32_t*)address;
		}
		address+=4;
	}
	return value;
}

void erase_storage(void)
{
	FLASH_EraseInitTypeDef f;
	uint32_t PageError = 0;

	HAL_FLASH_Unlock();

	f.TypeErase = FLASH_TYPEERASE_PAGES;
	f.PageAddress = STORAGE_START;
	f.NbPages = PAGES / STORAGE_SIZE;

	HAL_FLASHEx_Erase(&f, &PageError);
#if(DEBUG == 1)
	printf("erase %lX, rst=%lX\r\n", STORAGE_START, PageError);
#endif

	HAL_FLASH_Lock();
}
