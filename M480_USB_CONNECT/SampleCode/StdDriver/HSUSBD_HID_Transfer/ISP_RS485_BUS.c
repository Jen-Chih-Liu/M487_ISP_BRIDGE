#include "stdio.h"
#include "NuMicro.h"
#include "stdint.h"
#include "ISP_DRIVER.H"
#define uart_timeout   10

#define RS458_TX_ENEABLE PH2=1
#define RS458_TX_DISABLE PH2=0
void SendChar_ToRS485(uint8_t u8InChar)
{
    
    while(UART_IS_TX_FULL(UART5)){};  /* Wait Tx is not full to transmit data */
          UART_WRITE(UART5, u8InChar);
	  while(UART_IS_TX_EMPTY(UART5)==0);  /* Wait Tx is not full to transmit data */
	  
}


ErrNo RS485_WRITE(io_handle_t handle, const void *buf, uint32 *len)
{
	uint8_t *pTxData;
	uint32 txlen=*len;
	pTxData = (uint8_t *)buf;
	RS458_TX_ENEABLE;
	for (int i = 0; i < txlen; i++)
	     SendChar_ToRS485(pTxData[i]);		
    RS458_TX_DISABLE;		 
	return ENOERR;
}

ErrNo RS485_READ(io_handle_t handle, void *buf, uint32 *len)
{
	
	uint8_t *pRxData;
	uint32 rxlen = *len;
	pRxData = (uint8_t *)buf;
	uint32_t time_count=0;
	TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1);
    TIMER_Start(TIMER0);
	for (int i = 0; i < rxlen; i++)
   {
		 while(1)
		 {
		 if(UART_IS_RX_READY(UART5))
		 {
		 pRxData[i]=UART_READ(UART5);
			 break;
		 }
		 
		if(TIMER_GetIntFlag(TIMER0))
			 {
				
		/* Clear Timer0 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER0);
				time_count++;
				if(time_count>=uart_timeout) 
				{
					TIMER_Stop(TIMER0);
					return ETO; //THIS IS TIMEOUT ERROR
		   } 
		 
	 }
 }
		 }

TIMER_Stop(TIMER0);
   
	return ENOERR;
}



ErrNo RS485_Config(void *priv)
{
		    /* Enable UART module clock */
    CLK_EnableModuleClock(UART5_MODULE);


    /* Select UART clock source is HXT */
    CLK_SetModuleClock(UART5_MODULE, CLK_CLKSEL3_UART5SEL_HXT, 0);

    /* Set GPB multi-function pins for UART5 RXD and TXD */
    SYS->GPH_MFPL &= ~(SYS_GPH_MFPL_PH0MFP_Msk | SYS_GPH_MFPL_PH1MFP_Msk);
    SYS->GPH_MFPL |= (SYS_GPH_MFPL_PH0MFP_UART5_TXD) | (SYS_GPH_MFPL_PH1MFP_UART5_RXD);
     
		    /* Configure UART5 and set UART5 baud rate */
    UART_Open(UART5, 115200);
	    CLK_EnableModuleClock(TMR0_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(UART5_MODULE, CLK_CLKSEL3_UART5SEL_HXT, CLK_CLKDIV0_UART0(1));
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0);
	  
			    /* Configure PB.2 as Output mode for RS485 transfer*/
    GPIO_SetMode(PH, BIT2, GPIO_MODE_OUTPUT);
		PH2=0;//RS485 RECIVER.
		return ENOERR;
}

ErrNo RS485_Package(io_handle_t handle, void *buf, uint32 *len)
{
return ENOERR;

}




