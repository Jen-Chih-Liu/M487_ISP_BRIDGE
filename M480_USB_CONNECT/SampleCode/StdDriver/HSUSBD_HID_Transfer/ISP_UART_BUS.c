#include "stdio.h"
#include "NuMicro.h"
#include "stdint.h"
#include "ISP_DRIVER.H"
#define uart_timeout   10
void SendChar_ToUART0(uint8_t u8InChar)
{
    while(UART_IS_TX_FULL(UART0)){};  /* Wait Tx is not full to transmit data */
          UART_WRITE(UART0, u8InChar);
	  while(UART_IS_TX_EMPTY(UART0)==0);  /* Wait Tx is not full to transmit data */
}


ErrNo ISP_UART_WRITE(io_handle_t handle, const void *buf, uint32 *len)
{
	uint8_t *pTxData;
	uint32 txlen=*len;
	pTxData = (uint8_t *)buf;
	for (int i = 0; i < txlen; i++)
	     SendChar_ToUART0(pTxData[i]);		
	return ENOERR;
}

ErrNo ISP_UART_READ(io_handle_t handle, void *buf, uint32 *len)
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
		 if(UART_IS_RX_READY(UART0))
		 {
		 pRxData[i]=UART_READ(UART0);
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



ErrNo UART_Config(void *priv)
{
		    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);


    /* Select UART clock source is HXT */
    CLK->CLKSEL1 = (CLK->CLKSEL1 & ~CLK_CLKSEL1_UART0SEL_Msk) | (0x0 << CLK_CLKSEL1_UART0SEL_Pos);

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk);
    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);
		    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
	    CLK_EnableModuleClock(TMR0_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HXT, CLK_CLKDIV0_UART0(1));
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0);
	  return ENOERR;
}

ErrNo ISP_UART_Package(io_handle_t handle, uint8_t *in_buf, uint32_t *in_len,uint8_t *out_buf, uint32_t *out_len)
{
	ISP_UART_WRITE(handle,out_buf,out_len);
	return ISP_UART_READ(handle,in_buf,in_len);
 

}




