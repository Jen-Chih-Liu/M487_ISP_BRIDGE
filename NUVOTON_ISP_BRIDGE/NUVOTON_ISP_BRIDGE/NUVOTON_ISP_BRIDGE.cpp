// NUVOTON_ISP_BRIDGE.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ISP_COMMAND.h"
#include <time.h>
int _tmain(int argc, _TCHAR* argv[])
{
	clock_t start_time, end_time;
	float total_time = 0;
	start_time = clock(); /* mircosecond */

	ISP_COMMAND *ISP = new ISP_COMMAND();
	if (ISP->OPEN_USBPORT() != RES_CONNECT)
	{
		printf("USB NO FOUND\n\r");
		goto EXIT;
	}
	//ISP->USB_SET_INTERFACE_UART(); 
	//ISP->USB_SET_INTERFACE_I2C(); 
	ISP->USB_SET_INTERFACE_CAN(); 
	//ISP->USB_SET_INTERFACE_SPI(); 
	//ISP->USB_SET_INTERFACE_RS485(); 

    ISP->USB_TO_UART_AUTO_DETECT(); //THIS IS FOR AUTO DETECT

	//START ;
	if (ISP->SN_PACKAGE() != RES_PASS)
	{
		printf("SN PACKAGE ERROR\n\r");
		goto EXIT;
	}
	//CHECK PID
	unsigned int fwversion;
	if (ISP->READFW_VERSION(&fwversion) != RES_PASS)
	{
		printf("READFW_VERSION ERROR\n\r");
		goto EXIT;
	}
	printf("FW version: 0x%x \n\r", fwversion);

	//CHECK PID
	unsigned int PID;
	if (ISP->READ_PID(&PID) != RES_PASS)
	{
		printf("CHIP PID FOUND\n\r");
		goto EXIT;
	}
	printf("PID version: 0x%x \n\r", PID);
	//READ CONFIG
	printf("config \n\r");
	unsigned int config[2];
	if (ISP->READ_CONFIG(config) != RES_PASS)
	{
		printf("CHIP NO FOUND\n\r");
		goto EXIT;
	}
		printf("config0: 0x%x\n\r", config[0]);
		printf("config1: 0x%x\n\r", config[1]);
	
	

	if (ISP->File_Open_APROM(argv[1]) == RES_FILE_NO_FOUND)
	{
		printf("FILE NO FOUND\n\r");
		goto EXIT;
	}

	printf("File name: %s\n\r", argv[1]);
	printf("File size: %d\n\r", ISP->file_size);

	//test updata aprom
	ISP->UPDATE_APROM();

	//reboot mcu to aprom
	ISP->RUN_TO_APROM();

EXIT:
	//close usb port
	ISP->CLOSE_USBPORT();

	delete ISP;
	end_time = clock();
	/* CLOCKS_PER_SEC is defined at time.h */
	total_time = (float)(end_time - start_time) / CLOCKS_PER_SEC;

	printf("Time : %f sec \n", total_time);

}

