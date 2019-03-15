#include <stdio.h>
#include "NUC230_240.h"
#include "ISP_USER.h"
volatile uint32_t bI2CDataReady;
extern __align(4) uint8_t response_buff[MAX_PKT_SIZE];
#define WaitSI_Timeout() while(I2C_GetIntFlag(I2C0) == 0)
__align(4) uint8_t  rcv_buf[MAX_PKT_SIZE] = {0};
void I2C_SlaveRcvSendData(void)
{
	int i;
	
	do{
		/* I2C0 as slave set AA*/
    I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI_AA);
		while( I2C_GetIntFlag(I2C0) == 0);
	}while((I2C0->I2CSTATUS != 0xA8) && (I2C0->I2CSTATUS != 0xB0)
			&& (I2C0->I2CSTATUS != 0x60) && (I2C0->I2CSTATUS != 0x68));//wait SLA+R/SLA+W again
	
	if((I2C0->I2CSTATUS == 0xA8) || (I2C0->I2CSTATUS == 0xB0))/* SLA+R has been received and ACK has been returned */
	{
		//printf("start sent\n");
				
		i = 0;
		do
		{
			I2C0->I2CDAT = response_buff[i];
			I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI_AA);
			//while( I2C0->CON.SI == 0);
			WaitSI_Timeout();
			
			////Timeout or NACK received, break and don't resent
			if((I2C_GetIntFlag(I2C0) == 0) || (I2C0->I2CSTATUS != 0xB8))
				break;
			i++;
		}while(i<MAX_PKT_SIZE);
	}
	else if((I2C0->I2CSTATUS == 0x60) || (I2C0->I2CSTATUS == 0x68))/* SLA+W has been received and ACK has been returned */
	{
		//printf("start recv\n");
		bI2CDataReady = TRUE;

		i = 0;
		do
		{
			I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI_AA);
			WaitSI_Timeout();
			//printf("i %d\n", i);
			
				if((I2C_GetIntFlag(I2C0) == 0) || (I2C0->I2CSTATUS != 0x80))
			{
				bI2CDataReady = FALSE;
				break;
			}
			rcv_buf[i] = I2C0->I2CDAT;
			i++;
		}while(i<MAX_PKT_SIZE);
	}
	//wait stop signal
	I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI_AA);
	WaitSI_Timeout();
}



