#line 1 "..\\ISP_DRIVER.C"
#line 1 "..\\ISP_DRIVER.H"

typedef void * io_handle_t;
typedef unsigned int   uint32;
typedef unsigned char   uint8_t;
typedef signed int ErrNo;

typedef struct {
	ErrNo (*write)(io_handle_t handle,
	const void *buf,
	uint32 *len);
	ErrNo (*read)(io_handle_t handle,
		void *buf,
		uint32 *len);		
} devio_table_t;

typedef struct {
	const	char        *name;										
	const	devio_table_t *handlers;                              
	ErrNo	(*init)(void *);										                                 

}devtab_entry_t;







#line 45 "..\\ISP_DRIVER.H"
extern ErrNo  io_open(const char *dev_name, io_handle_t *io_handle);
extern ErrNo io_write(io_handle_t handle, const void *buf, uint32 *len);
extern ErrNo io_read(io_handle_t handle, void *buf, uint32 *len);
extern int str_compare(const char *n1, const char *n2);
#line 2 "..\\ISP_DRIVER.C"

extern ErrNo ISP_UART_WRITE(io_handle_t handle, const void *buf, uint32 *len);
extern ErrNo ISP_UART_READ(io_handle_t handle, void *buf, uint32 *len);
extern ErrNo UART_Config(void *priv);





const devio_table_t UART_Driver = { ISP_UART_WRITE, ISP_UART_READ, };

extern ErrNo CAN_WRITE(io_handle_t handle, const void *buf, uint32 *len);
extern ErrNo CAN_READ(io_handle_t handle, void *buf, uint32 *len);
extern ErrNo CAN_Config(void *priv);





const devio_table_t CAN_Driver = { CAN_WRITE, CAN_READ, };

extern ErrNo SPI_WRITE(io_handle_t handle, const void *buf, uint32 *len);
extern ErrNo SPI_READ(io_handle_t handle, void *buf, uint32 *len);
extern ErrNo SPI_Config(void *priv);





const devio_table_t SPI_Driver = { SPI_WRITE, SPI_READ, };

extern ErrNo I2C_WRITE(io_handle_t handle, const void *buf, uint32 *len);
extern ErrNo I2C_READ(io_handle_t handle, void *buf, uint32 *len);
extern ErrNo I2C_Config(void *priv);





const devio_table_t I2C_Driver = { I2C_WRITE, I2C_READ, };


extern ErrNo RS485_WRITE(io_handle_t handle, const void *buf, uint32 *len);
extern ErrNo RS485_READ(io_handle_t handle, void *buf, uint32 *len);
extern ErrNo RS485_Config(void *priv);
extern ErrNo RS485_Package(io_handle_t handle, void *buf, uint32 *len);




const devio_table_t RS485_Driver = { RS485_WRITE, RS485_READ, };

devtab_entry_t DevTab[] = {
		 "UART0", &UART_Driver, UART_Config,
	   "CAN0", &CAN_Driver, CAN_Config,
	   "SPI0", &SPI_Driver, SPI_Config,
	   "I2C0", &I2C_Driver, I2C_Config,
	   "RS4850",&RS485_Driver, RS485_Config,
};

static int io_compare(const char *n1, const char *n2, const char **ptr)
{
	while (*n1 && *n2) {
		if (*n1++ != *n2++) {
			return 0;
		}
	}
	if (*n1) {
		
		if (*(n2 - 1) == '/') {
			*ptr = n1;
			return 1;
		}
	}
	if (*n1 || *n2) {
		return 0;
	}
	*ptr = n1;
	return 1;
}

ErrNo  io_open(const char *dev_name, io_handle_t *io_handle)
{
		devtab_entry_t *p_devtab_entry;
		uint32 devtab_size, i;
		const char  *name_ptr;
		ErrNo re;

		if (dev_name == 0 || io_handle == 0)
		{
			return -22;
		}

		p_devtab_entry = (devtab_entry_t *)DevTab;

		devtab_size = sizeof(DevTab) / sizeof(devtab_entry_t);

		for (i = 0; i < devtab_size; i++)
		{
			if (io_compare(dev_name, p_devtab_entry->name, &name_ptr))
			{
				if (p_devtab_entry->init)
				{
					re = p_devtab_entry->init((void *)p_devtab_entry);
					if (re != 0)
						return re;

					*io_handle = (io_handle_t *)p_devtab_entry; 
					return re;
				}
			}
			p_devtab_entry++;
		}

		return -2;
}

ErrNo io_write(io_handle_t handle, const void *buf, uint32 *len)
{
	devtab_entry_t *t = (devtab_entry_t *)handle;

	if (handle == 0 || buf == 0 || len == 0)
	{
		return -22;
	}
	
	if (!t->handlers->write) {
		return -202;
	}
	
	
	if (0 != len && 0 == *len) {
		return 0;
	}
	return t->handlers->write(handle, buf, len);
}





ErrNo io_read(io_handle_t handle, void *buf, uint32 *len)
{
	devtab_entry_t *t = (devtab_entry_t *)handle;

	if (handle == 0 || buf == 0 || len == 0)
	{
		return -22;
	}

	
	if (!t->handlers->read) {
		return -202;
	}
	
	
	if (0 != len && 0 == *len) {
		return 0;
	}
	return t->handlers->read(handle, buf, len);
}
int str_compare(const char *n1, const char *n2)
{
	while (*n1 && *n2) {
		if (*n1++ != *n2++) {
			return 0;
		}
	}
	if (*n1) {
		
		if (*(n2 - 1) == '/') {
			return 1;
		}
	}
	if (*n1 || *n2) {
		return 0;
	}
	return 1;
}


