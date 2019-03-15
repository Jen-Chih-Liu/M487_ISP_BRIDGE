#include "StdAfx.h"
#include "ISP_COMMAND.h"
#include "HID.h"
#include <time.h>
#include <io.h> 
#include <fcntl.h>
#define connect_sleep_time 50
#define dbg_printf printf
//#define dbg_printf(...) 
//#define package_printf printf
#define package_printf(...) 
#define USB_VID 0x0416
#define USB_PID 0x5020
//#define Time_Out_Value 1000
//#define Time_Out_Value 5000 //uart
#define Time_Out_Value 5000
#define Package_Size 64
CHidCmd pUSB;
DWORD Length;
unsigned char buffer[Package_Size]={0};
unsigned int PacketNumber;
unsigned int Address,Size;


ISP_STATE ISP_COMMAND::OPEN_USBPORT(void)
{
	if(!pUSB.OpenDevice(USB_VID, USB_PID))
	{
		printf("connect false\n\r");
		return RES_CONNECT_FALSE;
	}
	else 
		USB_OPEN_FLAG=1;
		return RES_CONNECT;
}


ISP_STATE ISP_COMMAND::CLOSE_USBPORT(void)
{
if(USB_OPEN_FLAG==1)
	pUSB.CloseDevice();
	return RES_DISCONNECT;
}


ISP_STATE ISP_COMMAND::READFW_VERSION(unsigned int *FW_VERSION)
{
	clock_t start_time, end_time;
	float total_time = 0;

	unsigned char cmd[Package_Size] = {0xa6,0,0,0,
		(PacketNumber&0xff),((PacketNumber>>8)&0xff),((PacketNumber>>16)&0xff),((PacketNumber>>24)&0xff)};
	pUSB.WriteFile((unsigned char *)&cmd, sizeof(cmd), &Length, 2000);
	Sleep(connect_sleep_time);
	start_time = clock(); /* mircosecond */  
	while(1)
	{
		pUSB.ReadFile(buffer, Package_Size, &Length, 2000);
		package_printf("package: 0x%x\n\r",buffer[4]);
		if ((buffer[4] | ((buffer[5] << 8) & 0xff00) | ((buffer[6] << 16) & 0xff0000) | ((buffer[7] << 24) & 0xff000000)) == (PacketNumber + 1))
			break;
		else
			return RES_PACKAGE_ERROR;
		end_time = clock(); 
		/* CLOCKS_PER_SEC is defined at time.h */ 
		if((end_time - start_time)>Time_Out_Value)
			return RES_TIME_OUT;

	}
	PacketNumber+=2;
	*FW_VERSION=(buffer[8] | ((buffer[9] << 8) & 0xff00) | ((buffer[10] << 16) & 0xff0000) | ((buffer[11] << 24) & 0xff000000));
	return RES_PASS;
}



void ISP_COMMAND::RUN_TO_APROM(void)
{
	unsigned char cmd[Package_Size] = {0xab,0,0,0,
		(PacketNumber&0xff),((PacketNumber>>8)&0xff),((PacketNumber>>16)&0xff),((PacketNumber>>24)&0xff)};
	pUSB.WriteFile((unsigned char *)&cmd, sizeof(cmd), &Length, 2000);					
	PacketNumber+=2;
}



ISP_STATE ISP_COMMAND::READ_PID(unsigned int *PID)
{
	clock_t start_time, end_time;
	float total_time = 0; 
	unsigned char cmd[Package_Size] = {0xB1,0,0,0,
		(PacketNumber&0xff),((PacketNumber>>8)&0xff),((PacketNumber>>16)&0xff),((PacketNumber>>24)&0xff)};	                    
	pUSB.WriteFile((unsigned char *)&cmd, sizeof(cmd), &Length, 2000);					
	start_time = clock(); /* mircosecond */ 
	Sleep(connect_sleep_time);
	while(1)
	{
		pUSB.ReadFile(buffer, Package_Size, &Length, 2000);
		package_printf("package: 0x%x\n\r",buffer[4]);
		if((buffer[4]|((buffer[5]<<8)&0xff00)|((buffer[6]<<16)&0xff0000)|((buffer[7]<<24)&0xff000000))==(PacketNumber+1))
			break;
		else
			return RES_PACKAGE_ERROR;
		end_time = clock(); 
		/* CLOCKS_PER_SEC is defined at time.h */ 
		if((end_time - start_time)>Time_Out_Value)
			return RES_TIME_OUT;
	}
	PacketNumber+=2;
	//return (buffer[8]|((buffer[9]<<8)&0xff00)|((buffer[10]<<16)&0xff0000)|((buffer[11]<<24)&0xff000000));
	unsigned int temp_PDID = *PID;
	*PID = buffer[8] | ((buffer[9] << 8) & 0xff00) | ((buffer[10] << 16) & 0xff0000) | ((buffer[11] << 24) & 0xff000000);
    return RES_PASS;
}


ISP_STATE ISP_COMMAND::USB_TO_UART_AUTO_DETECT(void)
{
	unsigned char cmd[Package_Size] = { 0xae, 0, 0, 0,
		(PacketNumber & 0xff), ((PacketNumber >> 8) & 0xff), ((PacketNumber >> 16) & 0xff), ((PacketNumber >> 24) & 0xff) };
	while (1)
	{
		pUSB.WriteFile((unsigned char *)&cmd, sizeof(cmd), &Length, 2000);
		pUSB.ReadFile(buffer, Package_Size, &Length, 2000);
		dbg_printf("package: 0x%x\n\r", buffer[4]);
		if ((buffer[4] | ((buffer[5] << 8) & 0xff00) | ((buffer[6] << 16) & 0xff0000) | ((buffer[7] << 24) & 0xff000000)) == (PacketNumber + 1))
			break;
	}
	PacketNumber += 2;
	return RES_PASS;
}



ISP_STATE ISP_COMMAND::USB_SET_INTERFACE_UART(void)
{
	unsigned char cmd[Package_Size] = { 0xba, 0, 0, 0,0x01,0,0,0};

		pUSB.WriteFile((unsigned char *)&cmd, sizeof(cmd), &Length, 2000);
		pUSB.ReadFile(buffer, Package_Size, &Length, 2000);		
		return RES_PASS;
}


ISP_STATE ISP_COMMAND::USB_SET_INTERFACE_I2C(void)
{
	unsigned char cmd[Package_Size] = { 0xba, 0, 0, 0, 0x04, 0, 0, 0 };

	pUSB.WriteFile((unsigned char *)&cmd, sizeof(cmd), &Length, 2000);
	pUSB.ReadFile(buffer, Package_Size, &Length, 2000);
	return RES_PASS;
}


ISP_STATE ISP_COMMAND::USB_SET_INTERFACE_RS485(void)
{
	unsigned char cmd[Package_Size] = { 0xba, 0, 0, 0, 0x02, 0, 0, 0 };

	pUSB.WriteFile((unsigned char *)&cmd, sizeof(cmd), &Length, 2000);
	pUSB.ReadFile(buffer, Package_Size, &Length, 2000);
	return RES_PASS;
}

ISP_STATE ISP_COMMAND::USB_SET_INTERFACE_SPI(void)
{
	unsigned char cmd[Package_Size] = { 0xba, 0, 0, 0, 0x05, 0, 0, 0 };

	pUSB.WriteFile((unsigned char *)&cmd, sizeof(cmd), &Length, 2000);
	pUSB.ReadFile(buffer, Package_Size, &Length, 2000);
	return RES_PASS;
}


ISP_STATE ISP_COMMAND::USB_SET_INTERFACE_CAN(void)
{
	unsigned char cmd[Package_Size] = { 0xba, 0, 0, 0, 0x03, 0, 0, 0 };

	pUSB.WriteFile((unsigned char *)&cmd, sizeof(cmd), &Length, 2000);
	pUSB.ReadFile(buffer, Package_Size, &Length, 2000);
	return RES_PASS;
}
ISP_STATE ISP_COMMAND::SN_PACKAGE(void)
{
	clock_t start_time, end_time;
	float total_time = 0;

	unsigned char cmd1[Package_Size] = {0xa4,0,0,0,
		(PacketNumber&0xff),((PacketNumber>>8)&0xff),((PacketNumber>>16)&0xff),((PacketNumber>>24)&0xff),
		(PacketNumber&0xff),((PacketNumber>>8)&0xff),((PacketNumber>>16)&0xff),((PacketNumber>>24)&0xff)};
	pUSB.WriteFile((unsigned char *)&cmd1, sizeof(cmd1), &Length, 2000);		
	start_time = clock(); /* mircosecond */ 
	while(1)
	{
		pUSB.ReadFile(buffer, Package_Size, &Length, 2000);
		package_printf("package: 0x%x\n\r",buffer[4]);
		if ((buffer[4] | ((buffer[5] << 8) & 0xff00) | ((buffer[6] << 16) & 0xff0000) | ((buffer[7] << 24) & 0xff000000)) == (PacketNumber + 1))
			break;
		else
			return RES_PACKAGE_ERROR;
		end_time = clock(); 
		/* CLOCKS_PER_SEC is defined at time.h */ 
		if((end_time - start_time)>Time_Out_Value)
			return RES_TIME_OUT;
	}
	PacketNumber+=2;
	return RES_PASS;
}

ISP_STATE ISP_COMMAND::READ_CONFIG(unsigned int *config)
{
	clock_t start_time, end_time;
	float total_time = 0;
	unsigned char cmd[Package_Size] = {0xa2,0, 0, 0, 
		(PacketNumber&0xff),((PacketNumber>>8)&0xff),((PacketNumber>>16)&0xff),((PacketNumber>>24)&0xff)};
	pUSB.WriteFile((unsigned char *)&cmd, sizeof(cmd), &Length, 2000);
	Sleep(connect_sleep_time);
	start_time = clock(); /* mircosecond */ 
	while(1)
	{
		pUSB.ReadFile(buffer, Package_Size, &Length, 2000);
		package_printf("package: 0x%x\n\r",buffer[4]);
		if ((buffer[4] | ((buffer[5] << 8) & 0xff00) | ((buffer[6] << 16) & 0xff0000) | ((buffer[7] << 24) & 0xff000000)) == (PacketNumber + 1))
			break;
		else
			return RES_PACKAGE_ERROR;
		end_time = clock(); 
		/* CLOCKS_PER_SEC is defined at time.h */ 
		if((end_time - start_time)>Time_Out_Value)
		{
			return RES_TIME_OUT;
		}
	}
	config[0] = buffer[8] | ((buffer[9] << 8) & 0xff00) | ((buffer[10] << 16) & 0xff0000) | ((buffer[11] << 24) & 0xff000000);
	config[1] =buffer[12] | ((buffer[13] << 8) & 0xff00) | ((buffer[14] << 16) & 0xff0000) | ((buffer[15] << 24) & 0xff000000);

	PacketNumber+=2;
	return RES_PASS;
}



ISP_STATE ISP_COMMAND::File_Open_APROM(_TCHAR* temp)
{
	FILE *fp;
	file_size=0;
	if((fp=fopen(temp,"rb"))==NULL)
	{
		dbg_printf("APROM FILE OPEN FALSE\n\r");
		return RES_FILE_NO_FOUND;
	}	
	if(fp!=NULL)
	{
		while(!feof(fp)) {	
			fread(&W_APROM_BUFFER[file_size], sizeof(char), 1, fp);                
			file_size++;	
		}
	}

	file_size=file_size-1;	
	fclose(fp);

	if (file_size > (APROM_SIZE * 1024))
	{
		dbg_printf("APROM FILE over size\n\r");
		return RES_FILE_SIZE_OVER;
	}
	return RES_FILE_LOAD;
}

ISP_STATE ISP_COMMAND::UPDATE_APROM(void)
{
	clock_t start_time, end_time;
	float total_time = 0;
	unsigned int count=0; 
	unsigned char cmd[Package_Size] = {0xa0,0, 0, 0, 
		(PacketNumber&0xff),((PacketNumber>>8)&0xff),((PacketNumber>>16)&0xff),((PacketNumber>>24)&0xff),
		(Address&0xff),((Address>>8)&0xff),((Address>>16)&0xff),((Address>>24)&0xff),
		(file_size&0xff),((file_size>>8)&0xff),((file_size>>16)&0xff),((file_size>>24)&0xff),
	};

	unsigned char cmd1[Package_Size] = {0,0, 0, 0, 
		(PacketNumber&0xff),((PacketNumber>>8)&0xff),((PacketNumber>>16)&0xff),((PacketNumber>>24)&0xff)
	};
	printf("                                                        ");
	printf("\r program progrss 0%%                                  ");
	//first package

	while(1)
	{
		cmd[count+16]=W_APROM_BUFFER[count];
		count++;
		if(count+16==Package_Size||count>file_size) 
		{		
			dbg_printf("send first\n\r");

			pUSB.WriteFile((unsigned char *)&cmd, sizeof(cmd), &Length, 2000);					
			Sleep(2000);
			start_time = clock(); /* mircosecond */  
			while(1)
			{
				pUSB.ReadFile(buffer, Package_Size, &Length, 2000);
				package_printf("package: 0x%x\n\r",buffer[4]);
				if((buffer[4]|((buffer[5]<<8)&0xff00)|((buffer[6]<<16)&0xff0000)|((buffer[7]<<24)&0xff000000))==(PacketNumber+1))
					break;				
			}
			break;
		}
	}
	dbg_printf("\n\r");
	///////////////////////////////////
	int j=0;


	while(1)
	{
		cmd1[j+8]=W_APROM_BUFFER[count];
		count++;
		j++;

		if(count>file_size) 
		{		
			PacketNumber=PacketNumber+2;
			cmd1[4]=(PacketNumber&0xff);
			cmd1[5]=(PacketNumber>>8)&0xff;
			cmd1[6]=(PacketNumber>>16)&0xff;
			cmd1[7]=(PacketNumber>>24)&0xff;
			dbg_printf("send late\n\r");
			pUSB.WriteFile((unsigned char *)&cmd1, sizeof(cmd1), &Length, 2000);
			Sleep(connect_sleep_time);
			start_time = clock(); /* mircosecond */  
			while(1)
			{
				pUSB.ReadFile(buffer, Package_Size, &Length, 2000);
				package_printf("package: 0x%x\n\r",buffer[4]);
				if((buffer[4]|((buffer[5]<<8)&0xff00)|((buffer[6]<<16)&0xff0000)|((buffer[7]<<24)&0xff000000))==(PacketNumber+1))
					break;				
			}
			PacketNumber=PacketNumber+2;
			break;
		}

		if(j+8==Package_Size)
		{
			//printf("\r                                                           ");
			printf("\r program progrss: %f %%",((float)count/(float)file_size)*100);
			PacketNumber=PacketNumber+2;
			cmd1[4]=(PacketNumber&0xff);
			cmd1[5]=(PacketNumber>>8)&0xff;
			cmd1[6]=(PacketNumber>>16)&0xff;
			cmd1[7]=(PacketNumber>>24)&0xff;
			//dbg_printf("send\n\r");
			pUSB.WriteFile((unsigned char *)&cmd1, sizeof(cmd1), &Length, 2000);
			Sleep(connect_sleep_time);
			start_time = clock(); /* mircosecond */  
			while(1)
			{
				pUSB.ReadFile(buffer, Package_Size, &Length, 2000);
				package_printf("package: 0x%x\n\r",buffer[4]);
				if((buffer[4]|((buffer[5]<<8)&0xff00)|((buffer[6]<<16)&0xff0000)|((buffer[7]<<24)&0xff000000))==(PacketNumber+1))
					break;
			}
			j=0;
		}
		//dbg_printf("\n\r");
	}
	printf("\r                                ");
	printf("\r program progrss: 100%% \n\r");
	return RES_PASS;
}





ISP_COMMAND::ISP_COMMAND(void)
{
	COM_OPEN_FLAG=0;;
	USB_OPEN_FLAG=0;
	Address=0;
	PacketNumber=1;
	APROM_SIZE=0;
	LDROM_SIZE=0;
	DATAFLASH_SIZE=0;
}


ISP_COMMAND::~ISP_COMMAND(void)
{

}
