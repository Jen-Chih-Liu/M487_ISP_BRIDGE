#include "stdio.h"
#include "stdint.h"
#include "ISP_DRIVER.H"
#include "NuMicro.h"
#define Device_Addr0  0x36

ErrNo I2C_WRITE(io_handle_t handle, const void *buf, uint32 *len)
{
	unsigned char *pTxData;
	uint32 txlen=*len;
	pTxData = (unsigned char *)buf;

	I2C_SET_CONTROL_REG(I2C0, I2C_CTL_STA);
	I2C_WAIT_READY(I2C0);    

	I2C_SetData(I2C0, Device_Addr0 << 1);
	I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
	I2C_WAIT_READY(I2C0);  
	if (I2C_GetStatus(I2C0) != 0x18)
	{
		I2C_SET_CONTROL_REG(I2C0, I2C_CTL_STO| I2C_CTL_SI );
		return EI2CSLW;
	}
	for (int i = 0; i<txlen; i++)
	{
		I2C_SetData(I2C0, pTxData[i]);
		I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
		I2C_WAIT_READY(I2C0);
	}
	I2C_SET_CONTROL_REG(I2C0, I2C_CTL_STO |I2C_CTL_SI);
	while ((I2C0->CTL0 & I2C_CTL0_STO_Msk) == I2C_CTL0_STO_Msk);
	return ENOERR;
}

ErrNo I2C_READ(io_handle_t handle, void *buf, uint32 *len)
{
	unsigned char *pRxData;
	uint32 rxlen = *len;
	pRxData = (unsigned char *)buf;

	I2C_SET_CONTROL_REG(I2C0, I2C_CTL_STA);
	I2C_WAIT_READY(I2C0);

	I2C_SetData(I2C0, (Device_Addr0 << 1) | 0x01);
	I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
	I2C_WAIT_READY(I2C0);
	if (I2C_GetStatus(I2C0) != 0x40)
	{
		I2C_SET_CONTROL_REG(I2C0, I2C_CTL_STO | I2C_CTL_SI);
		return EI2CSLR;
	}
	for (int i = 0; i<rxlen; i++)
	{

		I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI | I2C_CTL_AA);
		I2C_WAIT_READY(I2C0);
		pRxData[i] = I2C_GetData(I2C0);
	}
	I2C_SET_CONTROL_REG(I2C0, I2C_CTL_STO | I2C_CTL_SI);
	while ((I2C0->CTL0 & I2C_CTL0_STO_Msk) == I2C_CTL0_STO_Msk);
	return ENOERR;
}

ErrNo I2C_Config(void *priv)
{
	/* Enable I2C0 clock */
	CLK_EnableModuleClock(I2C0_MODULE);
	/* Set I2C0 multi-function pins */
	SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA4MFP_Msk | SYS_GPA_MFPL_PA5MFP_Msk)) |
		(SYS_GPA_MFPL_PA4MFP_I2C0_SDA | SYS_GPA_MFPL_PA5MFP_I2C0_SCL);

	/* I2C clock pin enable schmitt trigger */
	PA->SMTEN |= GPIO_SMTEN_SMTEN5_Msk;
	/* Open I2C0 and set clock to 100k */
	I2C_Open(I2C0, 100000);
		return 0;
}

ErrNo I2C_Package(io_handle_t handle, void *buf, uint32 *len)
{
return ENOERR;

}




