extern __align(4) uint8_t  rcv_buf[MAX_PKT_SIZE];
extern void CAN_RecvData(CAN_T *tCAN);
extern void CAN_SendData(CAN_T *tCAN);
#define CAN_ID_RX		0x102
#define CAN_ID_TX		0x100
