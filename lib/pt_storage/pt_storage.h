#ifndef __PT_STORAGE_FLASH_H
#define __PT_STORAGE_FLASH_H


/*
 *   file  : pt_storage_flash.h
 *   Author: Bill
 *   Ver   : 1.0.0
 *   Date  : 2020/09/10
 *   Brief : W/R storage flash
 */


#define STORAGE_START		(uint32_t)0x0800E800
#define STORAGE_END			(uint32_t)0x0800F7FC
#define STORAGE_SIZE		4
#define PAGES				1	//1page=1KBytes


/* head */
#define HEAD				0x7E

/* key id */
#define APP_CRC				0x00
#define ID_RELAY1			0x01
#define ID_RELAY2			0x02
#define ID_RELAY3			0x03
#define ID_RELAY4			0x04
#define ID_LED1				0x05
#define ID_LED2				0x06
#define ID_LED3				0x07
#define ID_LED4				0x08
#define ID_RELAY_SETUP		0x09
#define ID_LIGHT_SETUP		0x0A



/*==================================*/
/* data format
 * 7E 00 00 XX(key id), value(4byte) */
/*==================================*/


/**
  * @brief  write flash storage, memory data follow data format
  * @param  key_id
  * 		value
  * @retval none
  */
void write_storage(uint8_t key_id, uint32_t value);

/**
  * @brief  read flash storage, memory data follow data format
  * @param  key_id
  * @retval value
  */
uint32_t read_storage(uint8_t key_id);

/**
  * @brief  erase storage(follow your defined STORAGE address and pages)
  * @param  none
  * @retval none
  */
void erase_storage(void);

#endif /* __PT_STORAGE_FLASH_H */
