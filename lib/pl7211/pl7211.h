#ifndef __PL7211_H
#define __PL7211_H


/*
 *   file  : pl7211.h
 *   Author: Bill
 *   Ver   : 1.0.0
 *   Date  : 2020/09/02
 *   Brief : pl7211 meter ic drive
 */

#define HW_MODE							0
#define SW_MODE							1

#define PL7211_MODE						HW_MODE

#if (PL7211_MODE == HW_MODE)
#define PL7211_SPI						hspi2
#define PL7211_TIMEOUT_VALUE			0xFFFF

extern SPI_HandleTypeDef hspi2;

#else
#define PL7211_CLK(value) 				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, value)
#define PL7211_MOSI(value) 				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, value)
#define PL7211_MISO		 				HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)
#endif

#define PL7211_RST(value) 				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, value)
#define PL7211_CS(value) 				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, value)


/* reg 3803
 * Bit15 = NULL
 * Bit14 = OWP_EN
 * Bit13 = HANDSHK_EN
 * Bit12 = AC_Lose_EN
 * Bit11 = CLEAR_FLAG
 * Bit10 = INST_IA_EN
 * Bit9 = Leakage_INST_EN
 * Bit8 = Leakage_EN
 * Bit7 = CF_CNTB_EN
 * Bit6 = NOLOAD_EN
 * Bit5 = OCPA_EN
 * Bit4 = CF_CNTA_EN
 * Bit3 = KWH_EN
 * Bit2 = RELY_ON_EN
 * Bit1 = NULL
 * Bit0 = AVM_EN
 */
uint8_t pl7211_read_reg0x3803(void);
void pl7211_set_reg0x3803(uint8_t value);

/* read pl7211 vrms */
uint32_t pl7211_read_vrms(void);

/* read pl7211 irms */
uint32_t pl7211_read_irms(void);

/* read pl7211 power */
uint32_t pl7211_read_power(void);

/* read pl7211 whr(kwh) */
uint32_t pl7211_read_whr(void);

/* read pl7211 frequency */
uint32_t pl7211_read_freq(void);

/* initialize pl7211 */
void pl7211_init(void);

#endif /* __PL7211_H */
