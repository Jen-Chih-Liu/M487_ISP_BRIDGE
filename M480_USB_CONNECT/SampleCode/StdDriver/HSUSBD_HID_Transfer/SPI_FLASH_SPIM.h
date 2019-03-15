extern void initial_spi(void);
#define Flash_Read_Operation(address,size, buff) SPIM_IO_Read((address), USE_4_BYTES_MODE, (size), (buff), OPCODE_FAST_READ, 1, 1, 1, 1);
