#include "stm32f0xx_hal.h"
#include "stdio.h"
#include "pl7413.h"

#define PL7413_DEBUG				0

#define	pl7413_delay_ms(delay)		HAL_Delay(delay)


#if(PL7413_MODE == SW_MODE)
#define PL7413_delay_us(cycle)   	{for(uint32_t i=0; i<cycle; i++){__NOP();};}


static void PL7413_send_bytes(uint8_t *bytes, uint8_t size)
{
    uint8_t mask;

    for(int i=0; i<size; i++){
        mask = 0x80;
    	while(mask!=0)
    	{
    		PL7413_CLK(0);
        	if((bytes[i]&mask)!=0)
        	{
        		PL7413_MOSI(1);
        	}
        	else
        	{
        		PL7413_MOSI(0);
        	}
        	PL7413_delay_us(0x10);
        	PL7413_CLK(1);
        	mask>>=1;
        	PL7413_delay_us(0x15);
    	}
    }
}

static void pl7413_read_bytes(uint8_t *bytes, uint8_t size)
{
	uint8_t mask;
	uint8_t data;

    for(int i=0; i<size; i++){
        mask=0x80;
        data=0;
    	PL7413_CLK(1);
        PL7413_delay_us(0x10);

        while(mask!=0)
        {
        	PL7413_CLK(0);
            PL7413_delay_us(0x10);
        	PL7413_CLK(1);

            if(PL7413_MISO == 1)
            {
            	data |= mask;
            }
            PL7413_delay_us(0x10);
            mask>>=1;
        }

        bytes[i]=data;
    }
}
#endif

uint8_t pl7413_read_reg0x3803(void)
{
	uint8_t tx_buf[4]={0x31, 0x38, 0x03, 0xFF};
	uint8_t rx_buf[4];

	uint8_t rst;

	PL7413_CS(0);
#if (PL7413_MODE == HW_MODE)
	HAL_SPI_TransmitReceive(&PL7143_SPI, tx_buf, rx_buf, 4, PL7413_TIMEOUT_VALUE);
#else
	PL7413_send_bytes(tx_buf, 3);
	pl7413_read_bytes(rx_buf, 1);

#endif
	PL7413_CS(1);

	rst=rx_buf[3];

#if (PL7413_DEBUG == 1)
	printf("pl7413 read reg0x3803 %x\r\n", rst);
#endif
    return rst;
}

void pl7413_set_reg0x3803(uint8_t value)
{
	uint8_t tx_buf[4]={0x41, 0x38, 0x03, 0xFF};
	tx_buf[3] = value;
	pl7413_read_reg0x3803();
	PL7413_CS(0);
#if (PL7413_MODE == HW_MODE)
	HAL_SPI_Transmit(&PL7143_SPI, tx_buf, 4, PL7413_TIMEOUT_VALUE);
#else
	PL7413_send_bytes(tx_buf, 4);
#endif
	PL7413_CS(1);

#if (PL7413_DEBUG == 1)
	printf("pl7413 set reg0x3803 %x\r\n", value);
#endif
}

/* Vrms register address : 0x3078~0x307D */
uint32_t pl7413_read_vrms(void)
{
	uint8_t tx_buf[9]={0x36, 0x30, 0x78, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t rx_buf[9];

	uint64_t vrms_buf;
	uint32_t vrms;

	PL7413_CS(0);
#if (PL7413_MODE == HW_MODE)
	HAL_SPI_TransmitReceive(&PL7143_SPI, tx_buf, rx_buf, 9, PL7413_TIMEOUT_VALUE);
#else
	PL7413_send_bytes(tx_buf, 3);
	pl7413_read_bytes(rx_buf, 6);
#endif
	PL7413_CS(1);

#if (PL7413_MODE == HW_MODE)
	vrms_buf=((uint64_t)rx_buf[8]<<40)+((uint64_t)rx_buf[7]<<32)+(rx_buf[6]<<24)+(rx_buf[5]<<16)+(rx_buf[4]<<8)+rx_buf[3];
#else
	vrms_buf=((uint64_t)rx_buf[5]<<40)+((uint64_t)rx_buf[4]<<32)+(rx_buf[3]<<24)+(rx_buf[2]<<16)+(rx_buf[1]<<8)+rx_buf[0];
#endif

	vrms=(vrms_buf/16777216.0)*100;

#if (PL7413_DEBUG == 1)
	printf("pl7413 vrms %ld\r\n", vrms);
#endif

    return vrms;
}

/* Irms(A) register address : 0x3084~0x3089
 * Irms(B) register address : 0x308A~0x308F
 * Irms(C) register address : 0x30AE~0x30B3
 */
uint32_t pl7413_read_irms(uint8_t ch)
{
	uint8_t tx_buf[9]={0x36, 0x30, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t rx_buf[9];

	uint64_t irms_buf;
	uint32_t irms;

	if(ch == 0x01){
		tx_buf[2] = 0x84;
	}else if(ch == 0x02){
		tx_buf[2] = 0x8A;
	}else if(ch == 0x03){
		tx_buf[2] = 0xAE;
	}

	PL7413_CS(0);
#if (PL7413_MODE == HW_MODE)
	HAL_SPI_TransmitReceive(&PL7143_SPI, tx_buf, rx_buf, 9, PL7413_TIMEOUT_VALUE);
#else
	PL7413_send_bytes(tx_buf, 3);
	pl7413_read_bytes(rx_buf, 6);
#endif
	PL7413_CS(1);

#if (PL7413_MODE == HW_MODE)
	irms_buf=((uint64_t)rx_buf[8]<<40)+((uint64_t)rx_buf[7]<<32)+(rx_buf[6]<<24)+(rx_buf[5]<<16)+(rx_buf[4]<<8)+rx_buf[3];
#else
	irms_buf=((uint64_t)rx_buf[5]<<40)+((uint64_t)rx_buf[4]<<32)+(rx_buf[3]<<24)+(rx_buf[2]<<16)+(rx_buf[4]<<1)+rx_buf[0];
#endif

	irms=(irms_buf/1073741824.0)*100;

#if (PL7413_DEBUG == 1)
	printf("pl7413 irms %ld\r\n", irms);
#endif

    return irms;
}

/* ActivePower(WA) register address :0x3090~0x3095
 * ActivePower(WB) register address :0x3096~0x309B
 * ActivePower(WC) register address :0x30B4~0x30B9
 */
uint32_t pl7413_read_power(uint8_t ch)
{
	uint8_t tx_buf[9]={0x36, 0x30, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t rx_buf[9];

	uint64_t power_buf;
	uint32_t power;

	if(ch == 0x01){
		tx_buf[2] = 0x90;
	}else if(ch == 0x02){
		tx_buf[2] = 0x96;
	}else if(ch == 0x03){
		tx_buf[2] = 0xB4;
	}

	PL7413_CS(0);
#if (PL7413_MODE == HW_MODE)
	HAL_SPI_TransmitReceive(&PL7143_SPI, tx_buf, rx_buf, 9, PL7413_TIMEOUT_VALUE);
#else
	PL7413_send_bytes(tx_buf, 3);
	pl7413_read_bytes(rx_buf, 6);
#endif
	PL7413_CS(1);

#if (PL7413_MODE == HW_MODE)
	power_buf=((uint64_t)rx_buf[8]<<40)+((uint64_t)rx_buf[7]<<32)+(rx_buf[6]<<24)+(rx_buf[5]<<16)+(rx_buf[4]<<8)+rx_buf[3];
#else
	power_buf=((uint64_t)rx_buf[5]<<40)+((uint64_t)rx_buf[4]<<32)+(rx_buf[3]<<24)+(rx_buf[2]<<16)+(rx_buf[1]<<8)+rx_buf[0];
#endif

	power=(power_buf/16777216.0)*100;

#if (PL7413_DEBUG == 1)
	printf("pl7413 power %ld\r\n", power);
#endif

    return power;
}

/* CF_Count(kWA) register address :0x312C~0x3131
 * CF_Count(kWB) register address :0x3132~0x3137
 * CF_Count(kWC) register address :0x3150~0x3155
 */
uint32_t pl7413_read_whr(uint8_t ch)
{
	uint8_t tx_buf[9]={0x36, 0x31, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t rx_buf[9];

	uint64_t whr_buf;
	uint32_t whr;

	if(ch == 0x01){
		tx_buf[2] = 0x2C;
	}else if(ch == 0x02){
		tx_buf[2] = 0x32;
	}else if(ch == 0x03){
		tx_buf[2] = 0x50;
	}

	PL7413_CS(0);
#if (PL7413_MODE == HW_MODE)
	HAL_SPI_TransmitReceive(&PL7143_SPI, tx_buf, rx_buf, 9, PL7413_TIMEOUT_VALUE);
#else
	PL7413_send_bytes(tx_buf, 3);
	pl7413_read_bytes(rx_buf, 6);
#endif
	PL7413_CS(1);

#if (PL7413_MODE == HW_MODE)
	whr_buf=((uint64_t)rx_buf[8]<<40)+((uint64_t)rx_buf[7]<<32)+(rx_buf[6]<<24)+(rx_buf[5]<<16)+(rx_buf[4]<<8)+rx_buf[3];
#else
	whr_buf=((uint64_t)rx_buf[5]<<40)+((uint64_t)rx_buf[4]<<32)+(rx_buf[3]<<24)+(rx_buf[2]<<16)+(rx_buf[1]<<8)+rx_buf[0];
#endif

	whr=(whr_buf*0.3125)*100;

#if (PL7413_DEBUG == 1)
	printf("pl7413 whr %ld\r\n", whr);
#endif

    return whr;
}

/* ZccCnt register address : 0x3018~0x301D
 * ZccStart register address : 0x301E~0x3023
 * ZccStop register address : 0x3024~0x3029
 * SampleCnt mapping address : 0x3809~0x380A
 */
uint32_t pl7413_read_freq(void)
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
	PL7413_CS(0);
#if (PL7413_MODE == HW_MODE)
	HAL_SPI_TransmitReceive(&PL7143_SPI, tx_buf, rx_buf, 9, PL7413_TIMEOUT_VALUE);
#else
	PL7413_send_bytes(tx_buf, 3);
	pl7413_read_bytes(rx_buf, 6);
#endif
	PL7413_CS(1);

#if (PL7413_MODE == HW_MODE)
	cnt=(rx_buf[4]<<8)+rx_buf[3];
#else
	cnt=(rx_buf[1]<<8)+rx_buf[0];
#endif

	/* start calculation */
	tx_buf[0]=0x36;
	tx_buf[1]=0x30;
	tx_buf[2]=0x1E;
	PL7413_CS(0);
#if (PL7413_MODE == HW_MODE)
	HAL_SPI_TransmitReceive(&PL7143_SPI, tx_buf, rx_buf, 9, PL7413_TIMEOUT_VALUE);
#else
	PL7413_send_bytes(tx_buf, 3);
	pl7413_read_bytes(rx_buf, 6);
#endif
	PL7413_CS(1);

#if (PL7413_MODE == HW_MODE)
	start=((uint64_t)rx_buf[8]<<40)+((uint64_t)rx_buf[7]<<32)+(rx_buf[6]<<24)+(rx_buf[5]<<16)+(rx_buf[4]<<8)+rx_buf[3];
	//start=rx_buf[3];		//debug
#else
	start=((uint64_t)rx_buf[5]<<40)+((uint64_t)rx_buf[4]<<32)+(rx_buf[3]<<24)+(rx_buf[2]<<16)+(rx_buf[1]<<8)+rx_buf[0];
	//start=rx_buf[0];		//debug
#endif

	/* stop calculation */
	tx_buf[0]=0x36;
	tx_buf[1]=0x30;
	tx_buf[2]=0x24;
	PL7413_CS(0);
#if (PL7413_MODE == HW_MODE)
	HAL_SPI_TransmitReceive(&PL7143_SPI, tx_buf, rx_buf, 9, PL7413_TIMEOUT_VALUE);
#else
	PL7413_send_bytes(tx_buf, 3);
	pl7413_read_bytes(rx_buf, 6);
#endif
	PL7413_CS(1);

#if (PL7413_MODE == HW_MODE)
	stop=((uint64_t)rx_buf[8]<<40)+((uint64_t)rx_buf[7]<<32)+(rx_buf[6]<<24)+(rx_buf[5]<<16)+(rx_buf[4]<<8)+rx_buf[3];
#else
	stop=((uint64_t)rx_buf[5]<<40)+((uint64_t)rx_buf[4]<<32)+(rx_buf[3]<<24)+(rx_buf[2]<<16)+(rx_buf[1]<<8)+rx_buf[0];
#endif

	/* samplecnt calculation */
	tx_buf[0]=0x32;
	tx_buf[1]=0x38;
	tx_buf[2]=0x09;
	PL7413_CS(0);
#if (PL7413_MODE == HW_MODE)
	HAL_SPI_TransmitReceive(&PL7143_SPI, tx_buf, rx_buf, 5, PL7413_TIMEOUT_VALUE);
#else
	PL7413_send_bytes(tx_buf, 3);
	pl7413_read_bytes(rx_buf, 2);
#endif
	PL7413_CS(1);

#if (PL7413_MODE == HW_MODE)
	samplecnt=(rx_buf[4]<<8)+rx_buf[3];
#else
	samplecnt=(rx_buf[1]<<8)+rx_buf[0];
#endif



	/* Frequency value = {( (ZccCnt-1) / 2 ) / ( ( ZccStop - ZccStart) / SampleCnt) } */
	rst1=(((cnt-1)*100)/200);
	rst2=(stop-start)*1;
	freq=(((rst1*samplecnt)*1000)/rst2)/10;
//120 5 3879 3902
#if (PL7413_DEBUG == 1)
	printf("pl7413 freq %ld\r\n", freq);
#endif

    return freq;
}

void pl7413_init(void)
{
#if (PL7413_DEBUG == 1)
	printf("pl7413 init\r\n");
#endif

	PL7413_CS(0);

	PL7413_RST(1);
	PL7413_RST(0);

	pl7413_delay_ms(1);
	PL7413_RST(1);
	pl7413_delay_ms(1);

	PL7413_CS(1);
}
