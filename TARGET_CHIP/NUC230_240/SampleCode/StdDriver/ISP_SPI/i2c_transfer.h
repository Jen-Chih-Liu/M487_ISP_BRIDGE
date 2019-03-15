extern volatile uint32_t bI2CDataReady;
extern __align(4) uint8_t  rcv_buf[MAX_PKT_SIZE];
extern void I2C_SlaveRcvSendData(void);

