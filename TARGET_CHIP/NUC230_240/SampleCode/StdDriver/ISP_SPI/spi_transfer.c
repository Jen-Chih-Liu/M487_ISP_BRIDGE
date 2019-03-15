#include <stdio.h>
#include "NUC230_240.h"
#include "ISP_USER.h"
extern __align(4) uint8_t response_buff[MAX_PKT_SIZE];
__align(4) uint8_t  rcv_buf[MAX_PKT_SIZE] = {0};

void SPI_SendData(void)
{
	uint32_t temp_count;
	for (temp_count = 0; temp_count<MAX_PKT_SIZE; temp_count++)
	{
		/* Write to TX register */
		SPI_WRITE_TX0(SPI0, response_buff[temp_count]);
		
		/* Trigger SPI data transfer */
		 SPI_TRIGGER(SPI0);
		/* Check SPI0 busy status */
		while (SPI_IS_BUSY(SPI0));
	}
}


void SPI_RcvData(void)
{
	uint32_t temp_count;
	for (temp_count = 0; temp_count<MAX_PKT_SIZE; temp_count++)
	{
		/* Write to TX register */
		SPI_WRITE_TX0(SPI0, 0xff);
		/* Trigger SPI data transfer */
		SPI_TRIGGER(SPI0);
		/* Check SPI0 busy status */
		while (SPI_IS_BUSY(SPI0));
		rcv_buf[temp_count]=SPI_READ_RX0(SPI0);		
	}
}

