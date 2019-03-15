#include <stdio.h>
#include "NuMicro.h"
#include "stdint.h"
#include "ISP_DRIVER.H"
#include <string.h>
extern int32_t CAN_SetRxMsgObj(CAN_T  *tCAN, uint8_t u8MsgObj, uint8_t u8idType, uint32_t u32id, uint8_t u8singleOrFifoLast);
#define CAN_ID_ISP_RX 0X100
#define CAN_ID_ISP_TX 0X102
ErrNo CAN_WRITE(io_handle_t handle, const void *buf, uint32 *len)
{
	uint8_t *pTxData;
	//uint32 txlen=*len;
uint8_t i;
	pTxData = (uint8_t *)buf;
	
	STR_CANMSG_T tMsg;
	CAN_T *tCAN;
	tCAN = (CAN_T *)CAN0;

	for (i = 0; i<8; i++)
	{
		tMsg.FrameType = CAN_DATA_FRAME;
	  tMsg.IdType = CAN_STD_ID;
	  tMsg.DLC = 8;
	  tMsg.Id = CAN_ID_ISP_TX;
		memcpy(&tMsg.Data[0], &pTxData[i << 3], 8);
		CAN_Transmit(tCAN, MSG(i), &tMsg);
		CLK_SysTickDelay(200);
		CAN_CLR_INT_PENDING_BIT(tCAN,i);
	}
	return ENOERR;
}

ErrNo CAN_READ(io_handle_t handle, void *buf, uint32 *len)
{
	uint8_t *pRxData;
	//uint32 rxlen = *len;

	pRxData = (uint8_t *)buf;
	uint8_t int_count;
	STR_CANMSG_T rrMsg;
			CAN_T *tCAN;
	tCAN = (CAN_T *)CAN0;

	int_count = 1;
  while (tCAN->IIDR == 0){}; 
	  CLK_SysTickDelay(2000);
	while (1) {
		                 /* Wait IDR is changed */
												  //printf("IDR = %x\n", tCAN->IIDR);
		CAN_Receive(tCAN, tCAN->IIDR - 1, &rrMsg); /* Get the message */
												   //CAN_ShowMsg(&rrMsg);                     /* Show the message object */
		memcpy(&pRxData[(int_count - 1) * 8], &rrMsg.Data[0], 8);
		int_count++;
		if (int_count>8)
			break;
	}
	return ENOERR;
}

ErrNo CAN_Config(void *priv)
{
			CAN_T *tCAN;
	tCAN = (CAN_T *)CAN0;

	/* Enable CAN module clock */
	CLK_EnableModuleClock(CAN0_MODULE);
	    /* Set PA multi-function pins for CAN0 RXD(PA.4) and TXD(PA.5) */
    SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA4MFP_Msk | SYS_GPA_MFPL_PA5MFP_Msk)) |
                    (SYS_GPA_MFPL_PA4MFP_CAN0_RXD | SYS_GPA_MFPL_PA5MFP_CAN0_TXD);
	CAN_Open(tCAN, 1000000, CAN_NORMAL_MODE);
		for(int i=8;i<15;i++)
	CAN_SetRxMsgObj(tCAN, MSG(i), CAN_STD_ID, CAN_ID_ISP_RX, FALSE);
	CAN_SetRxMsg(tCAN, MSG(15), CAN_STD_ID, CAN_ID_ISP_RX); //FOR LAST CAN PACKAGE
	//control high speed
    /* Set CAN transceiver to high speed mode */
    GPIO_SetMode(PA, (BIT2 | BIT3), GPIO_MODE_OUTPUT);
    PA2 = 0;
    PA3 = 0;
	return ENOERR;
}
ErrNo CAN_Package(io_handle_t handle, void *buf, uint32 *len)
{
return ENOERR;
}

