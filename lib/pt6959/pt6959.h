#ifndef __PT6959_H
#define __PT6959_H


/*
 *   file  : pt6959.h
 *   Author: Bill
 *   Ver   : 1.0.0
 *   Date  : 2020/09/03
 *   Brief : pt6959 led drive
 */


#define HW_MODE							0
#define SW_MODE							1

#define PT6959_MODE						HW_MODE

#if (PT6959_MODE == HW_MODE)
#define PT6959_SPI						hspi2
#define PT6959_TIMEOUT_VALUE			0xFFFF

extern SPI_HandleTypeDef hspi2;

#else
#define PT6959_CLK(value) 				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, value)
#define PT6959_MOSI(value) 				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, value)
#define PT6959_MISO		 				HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)
#endif

#define PL6959_CS(value) 				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, value)


/* char to 7 segment
 * c value:
 * '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
 * '-', '.' , ''
 * 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'L', 'O', 'P', 'S', 'U'
 *
 * return segment byte
 */
uint8_t char2segment(char c);

/* digits: display RAM Address
 *
 * |SG1 SG4|SG5 SG8|SG9 SG12|SG13 SG14|
 * |  00HL |  00HU |  01HL  |   01HU  | DIG1
 * |  02HL |  02HU |  03HL  |   03HU  | DIG2
 * |  04HL |  04HU |  05HL  |   05HU  | DIG3
 * |  06HL |  06HU |  07HL  |   07HU  | DIG4
 * |  08HL |  08HU |  09HL  |   09HU  | DIG5
 * |  0AHL |  0AHU |  0BHL  |   0BHU  | DIG6
 * |  0CHL |  0CHU |  0DHL  |   0DHU  | DIG7
 *
 * segments:
 * DISP_A: 0x01
 * DISP_B: 0x02
 * DISP_C: 0x04
 * DISP_D: 0x08
 * DISP_E: 0x10
 * DISP_F: 0x20
 * DISP_G: 0x40
 * DISP_H: 0x80
 */
void pt6959_set_address(uint8_t digits, uint8_t segments);

/* set pt6959 dimmer
 * 0x80: dimmer off
 * 0x88: dimmer level = 1/16
 * 0x89: dimmer level = 2/16
 * 0x8A: dimmer level = 4/16
 * 0x8B: dimmer level = 10/16
 * 0x8C: dimmer level = 11/16
 * 0x8D: dimmer level = 12/16
 * 0x8E: dimmer level = 13/16
 * 0x8F: dimmer level = 14/16
 */
void pt6959_set_dimmer(uint8_t data);

/* initialize pt6959 */
void pt6959_init(void);


#endif /* __PT6959_H */
