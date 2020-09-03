#ifndef __PL7413_H
#define __PL7413_H


/*
 *   file  : pl7413.h
 *   Author: Bill
 *   Ver   : 1.0.0
 *   Date  : 2020/08/27
 *   Brief : pl7413 meter ic drive
 */

#define HW_MODE							0
#define SW_MODE							1

#define PL7413_MODE						HW_MODE

#if (PL7413_MODE == HW_MODE)
#define PL7143_SPI						hspi1
#define PL7413_TIMEOUT_VALUE			0xFFFF

extern SPI_HandleTypeDef hspi1;

#else
#define PL7413_CLK(value) 				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, value)
#define PL7413_MOSI(value) 				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, value)
#define PL7413_MISO		 				HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6)
#endif

#define PL7413_RST(value) 				HAL_GPIO_WritePin(GPIOF, GPIO_PIN_5, value)
#define PL7413_CS(value) 				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, value)


/* reg 3803
 * Bit15 = INST_IC_EN
 * Bit14 = INST_IB_EN
 * Bit13 = HANDSHK_EN
 * Bit12 = OCPC_EN
 * Bit11 = CLEAR_FLAG
 * Bit10 = INST_IA_EN
 * Bit9 = OCPB_EN
 * Bit8 = CF_CNTC_EN
 * Bit7 = CF_CNTB_EN
 * Bit6 = NOLOAD_EN
 * Bit5 = OCPA_EN
 * Bit4 = CF_CNTA_EN
 * Bit3 = KWH_EN
 * Bit2 = RELY EN_A
 * Bit1 = RELY EN_B
 * Bit0 = RELY EN_C
 */
uint8_t pl7413_read_reg0x3803(void);
void pl7413_set_reg0x3803(uint8_t value);

/* read pl7413 vrms
 */
uint32_t pl7413_read_vrms(void);

/* read pl7413 irms
 * chA: 0x01
 * chB: 0x02
 * chC: 0x03
 */
uint32_t pl7413_read_irms(uint8_t);

/* read pl7413 power
 * chA: 0x01
 * chB: 0x02
 * chC: 0x03
 */
uint32_t pl7413_read_power(uint8_t ch);

/* read pl7413 whr(kwh)
 * chA: 0x01
 * chB: 0x02
 * chC: 0x03
 */
uint32_t pl7413_read_whr(uint8_t ch);

/* read pl7413 frequency */
uint32_t pl7413_read_freq(void);

/* initialize pl7413 */
void pl7413_init(void);

#endif /* __PL7413_H */
