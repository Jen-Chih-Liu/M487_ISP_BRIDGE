/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * @brief
 *           Transmit and receive data from PC terminal through RS232 interface.
 *
 * @copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include "stdio.h"
#include "NuMicro.h"
#include "ISP_CMD.h"
#include "ISP_DRIVER.H"
#include "SPI_FLASH_SPIM.H"
#define PLL_CLOCK   192000000


io_handle_t  DEV_handle = NULL;
extern struct sISP_COMMAND ISP_COMMAND;

void SYS_Init(void)
{

    /* Set XT1_OUT(PF.2) and XT1_IN(PF.3) to input mode */
    PF->MODE &= ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);

    /* Enable HXT clock (external XTAL 12MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);

    /* Wait for HXT clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(192000000);
    CLK->PCLKDIV = (CLK_PCLKDIV_PCLK0DIV2 | CLK_PCLKDIV_PCLK1DIV2); // PCLK divider set 2  
}


#if 1
void check_error(char d, char *l,ErrNo ret_in)
{
if(ret_in)
{
printf("%d\n\r",d);
printf("%s\n\r",l);
printf("this is error code %d\n\r",ret_in);
while(1);
}
}
#else
void check_error(char d, char *l,ErrNo ret_in)
{

}
#endif
volatile uint32_t g_au32TMRINTCount;
void TMR1_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        /* Clear Timer0 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER1);

        g_au32TMRINTCount++;
    }
}
void timer_1_init(void)
{
    CLK_EnableModuleClock(TMR1_MODULE);

    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0);
	
	/* Open Timer1 in periodic mode, enable interrupt and 2 interrupt ticks per second */
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 2);
    TIMER_EnableInt(TIMER1);
	  NVIC_EnableIRQ(TMR1_IRQn);
		 TIMER_Start(TIMER1);
}
    

uint32_t fwversion,flash_boot;
uint32_t devid, config[2];
int32_t main(void)
{
	 ErrNo ret;
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();
   GPIO_SetMode(PB, BIT5, GPIO_MODE_OUTPUT);

	//ret=io_open(UART_NAME_STRING, &DEV_handle);
	//ret=io_open(I2C_NAME_STRING, &DEV_handle);
	//ret=io_open(SPI_NAME_STRING, &DEV_handle);
	//ret=io_open(RS485_NAME_STRING, &DEV_handle);
	ret=io_open(CAN_NAME_STRING, &DEV_handle);
	initial_spi();//for SPIM initial
	// devtab_entry_t *t = (devtab_entry_t *)DEV_handle;
	//printf("interface type:%s\n\r",t->name);
	 check_error(__LINE__, __FILE__, ret);
	 init_ISP_command();
	 printf("this is test start\n\r");
//timer_1_init();
  // Auto_Detect_Connect(&ISP_COMMAND);
	// ret=SyncPackno(&ISP_COMMAND);	 
	// check_error(__LINE__, __FILE__, ret);


	ret=FWVersion(&ISP_COMMAND,&fwversion);
	check_error(__LINE__, __FILE__, ret);
	printf("fw version:0x%x\n\r",fwversion);
	ret=GetDeviceID(&ISP_COMMAND,&devid);
	check_error(__LINE__, __FILE__, ret);
	printf("device id:0x%x\n\r",devid);
	ret=GetConfig(&ISP_COMMAND,config);
	check_error(__LINE__, __FILE__, ret);
	printf("config0: 0x%x\n\r", config[0]);
	printf("config1: 0x%x\n\r", config[1]);
	ret=GetFlashMode(&ISP_COMMAND,&flash_boot);
	check_error(__LINE__, __FILE__, ret);
	printf("device id:0x%x\n\r",devid);
	if (flash_boot != LDROM_MODE)
	{
		printf("boot in APROM\n\r");
	}
	else
	{
		printf("boot in LDROM\n\r");
	}
	
	ret=Updated_Target_Flash(&ISP_COMMAND,0,512);
	check_error(__LINE__, __FILE__, ret);
	
	//RunAPROM(&ISP_COMMAND);//software rest to aprom
	printf("time spend: %f\n\r",(float)g_au32TMRINTCount*(float)0.5);
	
	printf("this is stop\n\r");
	
  while(1);
}
