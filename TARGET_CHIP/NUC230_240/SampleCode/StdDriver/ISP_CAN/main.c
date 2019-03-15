/******************************************************************************
 * @file     main.c
 * @brief
 *           Transfer data between USB device and PC through USB HID interface.
 *           A windows tool is also included in this sample code to connect with USB device.
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "ISP_USER.h"
#include "can_transfer.h"

#define GPIO_DETECT 0

#define PLLCON_SETTING  CLK_PLLCON_72MHz_HIRC
#define PLL_CLOCK       71884880
/*--------------------------------------------------------------------------*/
void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable Internal RC 22.1184MHz clock */
    CLK->PWRCON |= CLK_PWRCON_OSC22M_EN_Msk;

    /* Waiting for Internal RC clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_OSC22M_STB_Msk));

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK->CLKSEL0 &= ~CLK_CLKSEL0_HCLK_S_Msk;
    CLK->CLKSEL0 |= CLK_CLKSEL0_HCLK_S_HIRC;
    CLK->CLKDIV &= ~CLK_CLKDIV_HCLK_N_Msk;
    CLK->CLKDIV |= CLK_CLKDIV_HCLK(1);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK->PLLCON = PLLCON_SETTING;
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_PLL_STB_Msk));
    CLK->CLKSEL0 &= (~CLK_CLKSEL0_HCLK_S_Msk);
    CLK->CLKSEL0 |= CLK_CLKSEL0_HCLK_S_PLL;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    //SystemCoreClockUpdate();
    //PllClock        = PLL_CLOCK;            // PLL
    //SystemCoreClock = PLL_CLOCK / 1;        // HCLK
    CyclesPerUs     = PLL_CLOCK / 1000000;  // For SYS_SysTickDelay()

    /* Enable module clock */

    CLK->AHBCLK |= CLK_AHBCLK_ISP_EN_Msk;	// (1ul << 2)
		

}
extern int32_t CAN_SetRxMsgObj(CAN_T  *tCAN, uint8_t u8MsgObj, uint8_t u8idType, uint32_t u32id, uint8_t u8singleOrFifoLast);   
/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
	int i;
	 CAN_T *tCAN;
    tCAN = (CAN_T *) CAN0;
    /* Unlock write-protected registers */
    SYS_UnlockReg();

    /* Init system and multi-funcition I/O */
    SYS_Init();

    FMC->ISPCON |= FMC_ISPCON_ISPEN_Msk;	// (1ul << 0)

    g_apromSize = GetApromSize();
    GetDataFlashInfo(&g_dataFlashAddr , &g_dataFlashSize);
		    FMC_ENABLE_AP_UPDATE();


    CLK_EnableModuleClock(CAN0_MODULE);

    /* Set PD multi-function pins for CANTX0, CANRX0 */
    SYS->GPD_MFP &= ~(SYS_GPD_MFP_PD6_Msk | SYS_GPD_MFP_PD7_Msk);
    SYS->GPD_MFP = SYS_GPD_MFP_PD6_CAN0_RXD | SYS_GPD_MFP_PD7_CAN0_TXD;
	
	//for can phy initial
    GPIO_SetMode(PE, BIT2, GPIO_PMD_OUTPUT);
    GPIO_SetMode(PE, BIT3, GPIO_PMD_OUTPUT);
		/* Set RS to low to control CAN transceiver for Nuvoton board */
		/* CAN0 */
		PE2 = 0;
		PE3 = 0;
		//initial can
   CAN_Open(tCAN, 1000000, CAN_NORMAL_MODE);
		for(i=8;i<15;i++)
	CAN_SetRxMsgObj(tCAN, MSG(i), CAN_STD_ID, CAN_ID_RX, FALSE);
	CAN_SetRxMsg(tCAN, MSG(15), CAN_STD_ID, CAN_ID_RX);
	
    while(1) {
			#if GPIO_DETECT
    if(PB15!= 0)
	break;
#endif
			
			
       CAN_RecvData(tCAN);
    	
    		ParseCmd(rcv_buf, MAX_PKT_SIZE);

			CAN_SendData(tCAN);
    }

//_APROM:
    outpw(&SYS->RSTSRC, 3);//clear bit
    outpw(&FMC->ISPCON, inpw(&FMC->ISPCON) & 0xFFFFFFFC);
    outpw(&SCB->AIRCR, (V6M_AIRCR_VECTKEY_DATA | V6M_AIRCR_SYSRESETREQ));

    /* Trap the CPU */
    while(1);
}


/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/

