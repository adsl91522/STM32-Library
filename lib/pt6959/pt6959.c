#include "stm32f0xx_hal.h"
#include "stdio.h"
#include "pt6959.h"


#define PT6959_DEBUG			0

#define	PT6959_Delay(delay)		HAL_Delay(delay)

#define DISP_A 					0x01
#define DISP_B 					0x02
#define DISP_C 					0x04
#define DISP_D 					0x08
#define DISP_E 					0x10
#define DISP_F 					0x20
#define DISP_G 					0x40
#define DISP_H 					0x80

struct PT6959{
	uint8_t CMD1;
	uint8_t CMD2;
	uint8_t CMD3;
	uint8_t CMD4;
}pt6959;

uint8_t pt6961_addr_buf[14];


#if(PT6959_MODE == SW_MODE)
static void pt6961_send_byte(uint8_t byte)
{
#if (PT6959_DEBUG == 1)
	printf("pt6959 send byte %02X\r\n", byte);
#endif
    for(int i=0; i<8; i++)
    {
        if(byte&0x01)
        {
    		PT6959_MOSI(1);
        }
        else
        {
    		PT6959_MOSI(0);
        }
        byte>>=0x01;
		PT6959_CLK(0);
		PT6959_CLK(1);
    }
}
#endif

uint8_t char2segment(char c)
{
    unsigned char data = 0;
    switch(c)
    {
    case '0':
        data = DISP_A | DISP_B | DISP_C | DISP_D | DISP_E | DISP_F;
        break;
    case '1':
        data = DISP_B | DISP_C;
        break;
    case '2':
        data = DISP_A | DISP_B | DISP_G | DISP_E | DISP_D;
        break;
    case '3':
        data = DISP_A | DISP_B | DISP_C | DISP_D | DISP_G;
        break;
    case '4':
        data = DISP_F | DISP_G | DISP_B | DISP_C;
        break;
    case '5':
        data = DISP_A | DISP_F | DISP_G | DISP_C | DISP_D;
        break;
    case '6':
        data = DISP_A | DISP_F | DISP_E | DISP_D | DISP_C | DISP_G;
        break;
    case '7':
        data = DISP_A | DISP_B | DISP_C;
        break;
    case '8':
        data = DISP_A | DISP_B | DISP_C | DISP_D | DISP_E | DISP_F | DISP_G;
        break;
    case '9':
        data = DISP_A | DISP_B | DISP_C | DISP_D | DISP_F | DISP_G;
        break;
    case '-':
        data = DISP_G;
        break;
    case 'A':
    case 'a':
        data = DISP_A | DISP_B | DISP_C | DISP_F | DISP_E | DISP_G;
        break;
    case 'B':
    case 'b':
        data = DISP_A | DISP_B | DISP_C | DISP_D | DISP_E | DISP_F | DISP_G;
        break;
    case 'C':
    case 'c':
        data = DISP_A | DISP_F | DISP_E | DISP_D;
        break;
    case 'D':
    case 'd':
        data = DISP_B | DISP_C | DISP_D | DISP_E | DISP_G;
        break;
    case 'e':
    case 'E':
        data = DISP_A | DISP_G | DISP_D | DISP_F | DISP_E;
        break;
    case 'f':
    case 'F':
        data = DISP_A | DISP_G | DISP_F | DISP_E;
        break;

    case 'G':
    case 'g':
        data = DISP_A | DISP_B | DISP_C | DISP_D | DISP_F | DISP_G;
        break;

    case 'H':
    case 'h':
         data = DISP_B | DISP_C | DISP_E | DISP_F | DISP_G;
         break;

    case 'I':
    case 'i':
        data = DISP_E;
        break;

    case 'l':
    case 'L':
        data = DISP_E | DISP_F | DISP_D;
        break;

    case 'O':
    case 'o':
        data = DISP_E | DISP_G | DISP_C | DISP_D;
        break;

    case 'P':
    case 'p':
        data = DISP_A | DISP_B | DISP_F | DISP_E | DISP_G;
        break;

    case 'R':
    case 'r':
        data = DISP_E | DISP_G;
        break;

    case 'S':
    case 's':
        data =  DISP_D | DISP_C | DISP_G | DISP_F | DISP_A;
        break;

    case 'U':
    case 'u':
        data =  DISP_B | DISP_C | DISP_D | DISP_E | DISP_F;
        break;

    case '.':
    case ',':
        data = DISP_E;
        break;
    case ' ':
    default:
        data = 0x00; // empty space
    }
    return data;
}

/* pt6961 update sequence: CMD2 -> CMD3 -> CMD1 -> CMD4
 *
 * command 1:DISPLAY MODE SETTING COMMANDS
 *  MSB						     LSB
 * | 0 | 0 | - | - | - | - | b1 | b0 |
 *   (b1)(b0) = Display Mode Settings:
 *   	00: 4 digits, 14 segments
 *   	01: 5 digits, 13 segments
 *   	10: 6 digits, 12 segments
 *   	11: 7 digits, 11 segments
 * ========================================
 * command 2:DATA SETTING COMMANDS
 *  MSB						        LSB
 * | 0 | 1 | - | - | b3 | b2 | b1 | b0 |
 *   (b1)(b0) = Data Write Mode Setting:
 *   	00: Write Data to Display Mode
 *   (b2) = Address Increment Mode Settings (Display Mode)
 *   	0 = Increment Address after Data has been Written
 *   	1 = Fixed Address
 *   (b3) = Mode Settings:
 *   	0 = Normal Operation Mode
 *   	1 = Test Mode
 * ========================================
 * command 3:ADDRESS SETTING COMMANDS
 *  MSB						        LSB
 * | 1 | 1 | - | - | b3 | b2 | b1 | b0 |
 *   (b3)(b2)(b1)(b0) = Address: 00H to 0DH
 * ========================================
 * command 4:DISPLAY CONTROL COMMANDS
 *  MSB						        LSB
 * | 1 | 0 | - | - | b3 | b2 | b1 | b0 |
 *   (b2)(b1)(b0) = Dimming Quantity Settings:
 *   	000: Pulse width = 1/16
 *		001: Pulse width = 2/16
 *		010: Pulse width = 4/16
 *		011: Pulse width = 10/16
 *		100: Pulse width – 11/16
 *		101: Pulse width = 12/16
 *		110: Pulse width = 13/16
 *		111: Pulse width = 14/16
 *	 (b3) = Display Settings:
 *	 	0: Display Off
 *	 	1: Display On
 * ========================================
 */
static void pt6961_update(void)
{
#if (PT6959_DEBUG == 1)
	printf("pt6959 update\r\n");
#endif

	PL6959_CS(0);
#if (PT6959_MODE == HW_MODE)
	HAL_SPI_Transmit(&PT6959_SPI, &pt6959.CMD2, 1, PT6959_TIMEOUT_VALUE);
#else
	pt6961_send_byte(pt6959.CMD2);
#endif
	PL6959_CS(1);

	PL6959_CS(0);
#if (PT6959_MODE == HW_MODE)
	HAL_SPI_Transmit(&PT6959_SPI, &pt6959.CMD3, 1, PT6959_TIMEOUT_VALUE);
	HAL_SPI_Transmit(&PT6959_SPI, pt6961_addr_buf, 14, PT6959_TIMEOUT_VALUE);
#else
	pt6961_send_byte(pt6959.CMD3);
	for(int i=0; i<14; i++)
	{
		pt6961_send_byte(pt6961_addr_buf[i]);
	}
#endif
	PL6959_CS(1);

	PL6959_CS(0);
#if (PT6959_MODE == HW_MODE)
	HAL_SPI_Transmit(&PT6959_SPI, &pt6959.CMD1, 1, PT6959_TIMEOUT_VALUE);
#else
	pt6961_send_byte(pt6959.CMD1);
#endif
	PL6959_CS(1);

	PL6959_CS(0);
#if (PT6959_MODE == HW_MODE)
	HAL_SPI_Transmit(&PT6959_SPI, &pt6959.CMD4, 1, PT6959_TIMEOUT_VALUE);
#else
	pt6961_send_byte(pt6959.CMD4);
#endif
	PL6959_CS(1);
}


void pt6959_set_address(uint8_t digits, uint8_t segments)
{

#if (PT6959_DEBUG == 1)
	printf("pt6959 set address, digits %02x, segments %02x\r\n", digits, segments);
#endif

	pt6961_addr_buf[digits]=segments;

	pt6961_update();
}

void pt6959_set_dimmer(uint8_t data)
{

#if (PT6959_DEBUG == 1)
	printf("pt6959 set dimmer level %02x\r\n", data);
#endif

	pt6959.CMD4=data;

	pt6961_update();
}

void pt6959_init(void)
{

#if (PT6959_DEBUG == 1)
	printf("pt6959 init\r\n");
#endif

#if (PT6959_MODE == SW_MODE)
	PT6959_CLK(1);
	PL6959_CS(1);
	PT6959_MOSI(0);
#endif

	PT6959_Delay(200);

	pt6959.CMD1=0x03;
	pt6959.CMD2=0x40;
	pt6959.CMD3=0xC0;
	pt6959.CMD4=0x8F;

	pt6961_update();
}
