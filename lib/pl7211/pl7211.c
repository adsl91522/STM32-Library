#include "stm32f0xx_hal.h"
#include "stdio.h"
#include "pl7211.h"


#define PL7211_DEBUG				0

#define	pl7211_delay_ms(delay)		HAL_Delay(delay)


#if(PL7211_MODE == SW_MODE)
#define PL7211_delay_us(cycle)   	{for(uint32_t i=0; i<cycle; i++){__NOP();};}


static void PL7211_send_bytes(uint8_t *bytes, uint8_t size)
{
    uint8_t mask;

    for(int i=0; i<size; i++){
        mask = 0x80;
    	while(mask!=0)
    	{
    		PL7211_CLK(0);
        	if((bytes[i]&mask)!=0)
        	{
        		PL7211_MOSI(1);
        	}
        	else
        	{
        		PL7211_MOSI(0);
        	}
        	PL7211_delay_us(0x10);
        	PL7211_CLK(1);
        	mask>>=1;
        	PL7211_delay_us(0x15);
    	}
    }
}

static void pl7211_read_bytes(uint8_t *bytes, uint8_t size)
{
	uint8_t mask;
	uint8_t data;

    for(int i=0; i<size; i++){
        mask=0x80;
        data=0;
    	PL7211_CLK(1);
        PL7211_delay_us(0x10);

        while(mask!=0)
        {
        	PL7211_CLK(0);
            PL7211_delay_us(0x10);
        	PL7211_CLK(1);

            if(PL7211_MISO == 1)
            {
            	data |= mask;
            }
            PL7211_delay_us(0x10);
            mask>>=1;
        }

        bytes[i]=data;
    }
}
#endif

uint8_t pl7211_read_reg0x3803(void)
{
	uint8_t tx_buf[4]={0x31, 0x38, 0x03, 0xFF};
	uint8_t rx_buf[4];

	uint8_t rst;

	PL7211_CS(0);
#if (PL7211_MODE == HW_MODE)
	HAL_SPI_TransmitReceive(&PL7211_SPI, tx_buf, rx_buf, 4, PL7211_TIMEOUT_VALUE);
#else
	PL7211_send_bytes(tx_buf, 3);
	pl7211_read_bytes(rx_buf, 1);

#endif
	PL7211_CS(1);

	rst=rx_buf[3];

#if (PL7211_DEBUG == 1)
	printf("pl7211 read reg0x3803 %x\r\n", rst);
#endif
    return rst;
}

void pl7211_set_reg0x3803(uint8_t value)
{
	uint8_t tx_buf[4]={0x41, 0x38, 0x03, 0xFF};
	tx_buf[3] = value;
	pl7211_read_reg0x3803();
	PL7211_CS(0);
#if (PL7211_MODE == HW_MODE)
	HAL_SPI_Transmit(&PL7211_SPI, tx_buf, 4, PL7211_TIMEOUT_VALUE);
#else
	PL7211_send_bytes(tx_buf, 4);
#endif
	PL7211_CS(1);

#if (PL7211_DEBUG == 1)
	printf("pl7211 set reg0x3803 %x\r\n", value);
#endif
}

/* Vrms register address : 0x3078~0x307D */
uint32_t pl7211_read_vrms(void)
{
	uint8_t tx_buf[9]={0x36, 0x30, 0x78, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t rx_buf[9];

	uint64_t vrms_buf;
	uint32_t vrms;

	PL7211_CS(0);
#if (PL7211_MODE == HW_MODE)
	HAL_SPI_TransmitReceive(&PL7211_SPI, tx_buf, rx_buf, 9, PL7211_TIMEOUT_VALUE);
#else
	PL7211_send_bytes(tx_buf, 3);
	pl7211_read_bytes(rx_buf, 6);
#endif
	PL7211_CS(1);

#if (PL7211_MODE == HW_MODE)
	vrms_buf=((uint64_t)rx_buf[8]<<40)+((uint64_t)rx_buf[7]<<32)+(rx_buf[6]<<24)+(rx_buf[5]<<16)+(rx_buf[4]<<8)+rx_buf[3];
#else
	vrms_buf=((uint64_t)rx_buf[5]<<40)+((uint64_t)rx_buf[4]<<32)+(rx_buf[3]<<24)+(rx_buf[2]<<16)+(rx_buf[1]<<8)+rx_buf[0];
#endif

	vrms=(vrms_buf/16777216.0)*100;

#if (PL7211_DEBUG == 1)
	printf("pl7211 vrms %ld\r\n", vrms);
#endif

    return vrms;
}

/* Irms register address : 0x3084~0x3089
 */
uint32_t pl7211_read_irms(void)
{
	uint8_t tx_buf[9]={0x36, 0x30, 0x84, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t rx_buf[9];

	uint64_t irms_buf;
	uint32_t irms;

	PL7211_CS(0);
#if (PL7211_MODE == HW_MODE)
	HAL_SPI_TransmitReceive(&PL7211_SPI, tx_buf, rx_buf, 9, PL7211_TIMEOUT_VALUE);
#else
	PL7211_send_bytes(tx_buf, 3);
	pl7211_read_bytes(rx_buf, 6);
#endif
	PL7211_CS(1);

#if (PL7211_MODE == HW_MODE)
	irms_buf=((uint64_t)rx_buf[8]<<40)+((uint64_t)rx_buf[7]<<32)+(rx_buf[6]<<24)+(rx_buf[5]<<16)+(rx_buf[4]<<8)+rx_buf[3];
#else
	irms_buf=((uint64_t)rx_buf[5]<<40)+((uint64_t)rx_buf[4]<<32)+(rx_buf[3]<<24)+(rx_buf[2]<<16)+(rx_buf[4]<<1)+rx_buf[0];
#endif

	irms=(irms_buf/1073741824.0)*100;

#if (PL7211_DEBUG == 1)
	printf("pl7211 irms %ld\r\n", irms);
#endif

    return irms;
}

/* ActivePower(W) register address :0x3090~0x3095
 */
uint32_t pl7211_read_power(void)
{
	uint8_t tx_buf[9]={0x36, 0x30, 0x90, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t rx_buf[9];

	uint64_t power_buf;
	uint32_t power;

	PL7211_CS(0);
#if (PL7211_MODE == HW_MODE)
	HAL_SPI_TransmitReceive(&PL7211_SPI, tx_buf, rx_buf, 9, PL7211_TIMEOUT_VALUE);
#else
	PL7211_send_bytes(tx_buf, 3);
	pl7211_read_bytes(rx_buf, 6);
#endif
	PL7211_CS(1);

#if (PL7211_MODE == HW_MODE)
	power_buf=((uint64_t)rx_buf[8]<<40)+((uint64_t)rx_buf[7]<<32)+(rx_buf[6]<<24)+(rx_buf[5]<<16)+(rx_buf[4]<<8)+rx_buf[3];
#else
	power_buf=((uint64_t)rx_buf[5]<<40)+((uint64_t)rx_buf[4]<<32)+(rx_buf[3]<<24)+(rx_buf[2]<<16)+(rx_buf[1]<<8)+rx_buf[0];
#endif

	power=(power_buf/16777216.0)*100;

#if (PL7211_DEBUG == 1)
	printf("pl7211 power %ld\r\n", power);
#endif

    return power;
}

/* CF_Count(kWH) register address :0x312C~0x3131
 */
uint32_t pl7211_read_whr(void)
{
	uint8_t tx_buf[9]={0x36, 0x31, 0x2C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t rx_buf[9];

	uint64_t whr_buf;
	uint32_t whr;

	PL7211_CS(0);
#if (PL7211_MODE == HW_MODE)
	HAL_SPI_TransmitReceive(&PL7211_SPI, tx_buf, rx_buf, 9, PL7211_TIMEOUT_VALUE);
#else
	PL7211_send_bytes(tx_buf, 3);
	pl7211_read_bytes(rx_buf, 6);
#endif
	PL7211_CS(1);

#if (PL7211_MODE == HW_MODE)
	whr_buf=((uint64_t)rx_buf[8]<<40)+((uint64_t)rx_buf[7]<<32)+(rx_buf[6]<<24)+(rx_buf[5]<<16)+(rx_buf[4]<<8)+rx_buf[3];
#else
	whr_buf=((uint64_t)rx_buf[5]<<40)+((uint64_t)rx_buf[4]<<32)+(rx_buf[3]<<24)+(rx_buf[2]<<16)+(rx_buf[1]<<8)+rx_buf[0];
#endif

	whr=(whr_buf*0.3125)*100;

#if (PL7211_DEBUG == 1)
	printf("pl7211 whr %ld\r\n", whr);
#endif

    return whr;
}

/* ZccCnt register address : 0x3018~0x301D
 * ZccStart register address : 0x301E~0x3023
 * ZccStop register address : 0x3024~0x3029
 * SampleCnt mapping address : 0x3809~0x380A
 */
uint32_t pl7211_read_freq(void)
{
	uint8_t tx_buf[9]={0x36, 0x30, 0x18, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t rx_buf[9];

	uint32_t cnt;
	uint32_t start;
	uint32_t stop;
	uint32_t samplecnt;
	uint32_t freq;

    uint32_t rst1, rst2;

	/* cnt calculation */
	PL7211_CS(0);
#if (PL7211_MODE == HW_MODE)
	HAL_SPI_TransmitReceive(&PL7211_SPI, tx_buf, rx_buf, 9, PL7211_TIMEOUT_VALUE);
#else
	PL7211_send_bytes(tx_buf, 3);
	pl7211_read_bytes(rx_buf, 6);
#endif
	PL7211_CS(1);

#if (PL7211_MODE == HW_MODE)
	cnt=(rx_buf[4]<<8)+rx_buf[3];
#else
	cnt=(rx_buf[1]<<8)+rx_buf[0];
#endif

	/* start calculation */
	tx_buf[0]=0x36;
	tx_buf[1]=0x30;
	tx_buf[2]=0x1E;
	PL7211_CS(0);
#if (PL7211_MODE == HW_MODE)
	HAL_SPI_TransmitReceive(&PL7211_SPI, tx_buf, rx_buf, 9, PL7211_TIMEOUT_VALUE);
#else
	PL7211_send_bytes(tx_buf, 3);
	pl7211_read_bytes(rx_buf, 6);
#endif
	PL7211_CS(1);

#if (PL7211_MODE == HW_MODE)
	start=((uint64_t)rx_buf[8]<<40)+((uint64_t)rx_buf[7]<<32)+(rx_buf[6]<<24)+(rx_buf[5]<<16)+(rx_buf[4]<<8)+rx_buf[3];
#else
	start=((uint64_t)rx_buf[5]<<40)+((uint64_t)rx_buf[4]<<32)+(rx_buf[3]<<24)+(rx_buf[2]<<16)+(rx_buf[1]<<8)+rx_buf[0];
#endif

	/* stop calculation */
	tx_buf[0]=0x36;
	tx_buf[1]=0x30;
	tx_buf[2]=0x24;
	PL7211_CS(0);
#if (PL7211_MODE == HW_MODE)
	HAL_SPI_TransmitReceive(&PL7211_SPI, tx_buf, rx_buf, 9, PL7211_TIMEOUT_VALUE);
#else
	PL7211_send_bytes(tx_buf, 3);
	pl7211_read_bytes(rx_buf, 6);
#endif
	PL7211_CS(1);

#if (PL7211_MODE == HW_MODE)
	stop=((uint64_t)rx_buf[8]<<40)+((uint64_t)rx_buf[7]<<32)+(rx_buf[6]<<24)+(rx_buf[5]<<16)+(rx_buf[4]<<8)+rx_buf[3];
#else
	stop=((uint64_t)rx_buf[5]<<40)+((uint64_t)rx_buf[4]<<32)+(rx_buf[3]<<24)+(rx_buf[2]<<16)+(rx_buf[1]<<8)+rx_buf[0];
#endif

	/* samplecnt calculation */
	tx_buf[0]=0x32;
	tx_buf[1]=0x38;
	tx_buf[2]=0x09;
	PL7211_CS(0);
#if (PL7211_MODE == HW_MODE)
	HAL_SPI_TransmitReceive(&PL7211_SPI, tx_buf, rx_buf, 5, PL7211_TIMEOUT_VALUE);
#else
	PL7211_send_bytes(tx_buf, 3);
	pl7211_read_bytes(rx_buf, 2);
#endif
	PL7211_CS(1);

#if (PL7211_MODE == HW_MODE)
	samplecnt=(rx_buf[4]<<8)+rx_buf[3];
#else
	samplecnt=(rx_buf[1]<<8)+rx_buf[0];
#endif

	/* Frequency value = {( (ZccCnt-1) / 2 ) / ( ( ZccStop - ZccStart) / SampleCnt) } */
	rst1=(((cnt-1)*100)/200);
	rst2=(stop-start)*1;
	freq=(((rst1*samplecnt)*1000)/rst2)/10;
//120 5 3879 3902
#if (PL7211_DEBUG == 1)
	printf("pl7211 freq %ld\r\n", freq);
#endif

    return freq;
}

void pl7211_init(void)
{
#if (PL7211_DEBUG == 1)
	printf("pl7211 init\r\n");
#endif

	PL7211_CS(0);

	PL7211_RST(1);
	PL7211_RST(0);

	pl7211_delay_ms(1);
	PL7211_RST(1);
	pl7211_delay_ms(1);

	PL7211_CS(1);
}
