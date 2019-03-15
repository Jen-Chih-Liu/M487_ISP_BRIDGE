#include <stdio.h>
#include "NUC230_240.h"
#include "ISP_USER.h"
#include <string.h>
#include "can_transfer.h"

extern __align(4) uint8_t response_buff[MAX_PKT_SIZE];
__align(4) uint8_t  rcv_buf[MAX_PKT_SIZE] = {0};


/* Declare a CAN message structure */
STR_CANMSG_T rrMsg;

void CAN_RecvData(CAN_T *tCAN)
{	
	unsigned int int_count;
	
  int_count=1;
		while(tCAN->IIDR ==0 ){};                  /* Wait IDR is changed */
	  CLK_SysTickDelay(2000);
    while(1) {
        CAN_Receive(tCAN, tCAN->IIDR - 1, &rrMsg); /* Get the message */
			  memcpy(&rcv_buf[(int_count-1)*8], &rrMsg.Data[0], 8);
			  int_count++;
			  if(int_count>8)
					break;
    }

}	

void CAN_SendData(CAN_T *tCAN)
{
	unsigned char i;
    STR_CANMSG_T tMsg;
		tMsg.FrameType = CAN_DATA_FRAME;
    tMsg.IdType   = CAN_STD_ID;
	  tMsg.DLC      = 8;
	  tMsg.Id       =CAN_ID_TX; 
	for(i=0;i<8;i++)
	{    
		memcpy(&tMsg.Data[0], &response_buff[i<<3], 8);
		CAN_Transmit(tCAN, MSG(i), &tMsg);
		CLK_SysTickDelay(200);
		CAN_CLR_INT_PENDING_BIT(tCAN,i);
  }

}
