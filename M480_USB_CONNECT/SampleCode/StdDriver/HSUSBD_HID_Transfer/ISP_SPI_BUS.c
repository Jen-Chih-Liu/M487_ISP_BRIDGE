#include <stdio.h>
#include "NuMicro.h"
#include "stdio.h"
#include "stdint.h"
#include "ISP_DRIVER.H"


ErrNo SPI_WRITE(io_handle_t handle, const void *buf, uint32 *len)
{
	uint8_t  *pTxData;
	uint32 txlen=*len;
	pTxData = (uint8_t *)buf;
SPI_ClearRxFIFO(SPI0);
	SPI_ClearTxFIFO(SPI0);
	for (int i = 0; i<txlen; i++)
	{
		/* Write to TX register */
		SPI_WRITE_TX(SPI0, pTxData[i]);
		CLK_SysTickDelay(300);
		/* Check SPI0 busy status */
		while (SPI_IS_BUSY(SPI0));
		while (SPI_GET_TX_FIFO_EMPTY_FLAG(SPI0)==0);
		CLK_SysTickDelay(300);
	}
	//CLK_SysTickDelay(500);
	return ENOERR;
}

ErrNo SPI_READ(io_handle_t handle, void *buf, uint32 *len)
{
	unsigned char *pRxData;
	uint32 rxlen = *len;
		CLK_SysTickDelay(5000);
	pRxData = (unsigned char *)buf;
  //CLEAR SPI TX AND RX FIFO	
SPI_ClearRxFIFO(SPI0);
	SPI_ClearTxFIFO(SPI0);
	for (int i = 0; i<rxlen; i++)
	{
		/* Write to TX register */
		SPI_WRITE_TX(SPI0, 0xff);
		CLK_SysTickDelay(300);
		while (SPI_IS_BUSY(SPI0));
		while (SPI_GET_TX_FIFO_EMPTY_FLAG(SPI0)==0);
		pRxData[i] = SPI_READ_RX(SPI0);
		CLK_SysTickDelay(300);
	}	
	CLK_SysTickDelay(5000);
	return ENOERR;
}

ErrNo SPI_Config(void *priv)
{


	/* Select PCLK0 as the clock source of SPI0 */
	CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL2_SPI0SEL_PCLK1, MODULE_NoMsk);


	/* Enable SPI0 peripheral clock */
	CLK_EnableModuleClock(SPI0_MODULE);

	/* Setup SPI0 multi-function pins */
	SYS->GPA_MFPL |= SYS_GPA_MFPL_PA0MFP_SPI0_MOSI | SYS_GPA_MFPL_PA1MFP_SPI0_MISO | SYS_GPA_MFPL_PA2MFP_SPI0_CLK | SYS_GPA_MFPL_PA3MFP_SPI0_SS;

	/* Enable SPI0 clock pin (PA2) schmitt trigger */
	PA->SMTEN |= GPIO_SMTEN_SMTEN2_Msk;
  /* Configure as a master, clock idle low, 8-bit transaction, drive output on falling clock edge and latch input on rising edge. */
    /* Set IP clock divider. SPI clock rate = 100kHz */
	SPI_Open(SPI0, SPI_MASTER, SPI_MODE_0, 8, 100000);
/* Enable the automatic hardware slave select function. Select the SS pin and configure as low-active. */
    SPI_EnableAutoSS(SPI0, SPI_SS, SPI_SS_ACTIVE_LOW);
	return ENOERR;
}

ErrNo SPI_Package(io_handle_t handle, void *buf, uint32 *len)
{
return ENOERR;

}



