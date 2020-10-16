#ifndef __PT_STORAGE_FLASH_H
#define __PT_STORAGE_FLASH_H


/*
 *   file  : pt_storage_flash.h
 *   Author: Bill
 *   Ver   : 1.1.0
 *   Date  : 2020/09/21
 *   Brief : W/R storage flash
 */


#define STORAGE_START		(uint32_t)0x0801E000
#define STORAGE_END			(uint32_t)0x0801F000

#define STORAGE_SIZE		4096

#define PAGES				2048	//1page=2KBytes


/* non volatile memory key id */
struct NVS_KEY_ID{
	uint8_t f_head;
	uint8_t s_head;
	uint32_t app_crc;
	uint8_t id_relay1;
	uint8_t id_relay2;
	uint8_t id_relay3;
	uint8_t id_relay4;
	uint8_t relay_setup;
	uint8_t light_setup;
	uint16_t chk_sum;
}nvs_key_id;

/*==================================*/
/* data format
 * 7D 7E key_id_0, key_id_1, key_id_2, key_id_3..., last 2 byte(chk_sum) 0x00 */
/*==================================*/


/**
  * @brief  write flash storage, memory data follow data format
  * @param  key_id
  * 		value
  * @retval none
  */
void write_storage(void);

/**
  * @brief  read flash storage, memory data follow data format
  * @param  key_id
  * @retval value
  */
void read_storage(void);

/**
  * @brief  erase storage(follow your defined STORAGE address and pages)
  * @param  none
  * @retval none
  */
void erase_storage(void);

#endif /* __PT_STORAGE_FLASH_H */
