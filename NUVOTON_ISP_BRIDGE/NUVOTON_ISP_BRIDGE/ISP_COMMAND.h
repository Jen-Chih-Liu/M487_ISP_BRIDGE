#pragma once
#include "afxwin.h"
#include "afxcmn.h"
typedef enum {
	RES_FALSE = 0,		
	RES_PASS,		    
	RES_FILE_NO_FOUND,
    RES_PROGRAM_FALSE,
	RES_CONNECT,
	RES_CONNECT_FALSE,
	RES_DISCONNECT,
	RES_FILE_LOAD,
	RES_FILE_SIZE_OVER,
	RES_TIME_OUT,
	RES_PACKAGE_ERROR
} ISP_STATE;

class ISP_COMMAND
{


private:
	HANDLE	m_hCom;
	DCB		m_dcb;
	DWORD iBytesWritten;
	DWORD iBytesRead;
	COMMTIMEOUTS m_CommTimeouts;
		//发送报告用的OVERLAPPED。
	OVERLAPPED WriteOverlapped;
	//接收报告用的OVERLAPPED。
	OVERLAPPED ReadOverlapped;
public:
	unsigned char W_APROM_BUFFER[128*1024];
    unsigned int file_size;
	unsigned int USB_OPEN_FLAG;
	unsigned int COM_OPEN_FLAG;
	char P_NUMBER[255];
    unsigned int APROM_SIZE;
    unsigned int LDROM_SIZE;
    unsigned int DATAFLASH_SIZE;
public:
	void RUN_TO_APROM(void);

	ISP_STATE READ_CONFIG(unsigned int *config);

	ISP_STATE READFW_VERSION(unsigned int *FW_VERSION);


    ISP_STATE OPEN_USBPORT(void);

	ISP_STATE READ_PID(void);

	
	ISP_STATE SN_PACKAGE(void);

	ISP_STATE READ_PID(unsigned int *PID);
	

	ISP_STATE CLOSE_USBPORT(void);	

	ISP_STATE File_Open_APROM(_TCHAR* temp);
	
	ISP_STATE UPDATE_APROM(void);
	
	ISP_STATE USB_SET_INTERFACE_UART(void);
	ISP_STATE USB_SET_INTERFACE_RS485(void);
	ISP_STATE USB_SET_INTERFACE_CAN(void);
	ISP_STATE USB_SET_INTERFACE_SPI(void);
	ISP_STATE USB_SET_INTERFACE_I2C(void);
	ISP_STATE USB_TO_UART_AUTO_DETECT(void);
	ISP_COMMAND(void);
	~ISP_COMMAND(void);
};

