#ifndef PII2C_H
#define PII2C_H

// Physical addresses for various peripheral register sets
// Base Physical Address of the BCM 2835 peripheral registers
#define BCM2835_PERI_BASE               0x20000000

// Base Physical Address of the I2C registers
#define BCM2835_I2C_BASE                (BCM2835_PERI_BASE + 0x205000)

#define I2C_I2CEN    (1 << 15)
#define I2C_INTR    (1 << 10)
#define I2C_INTT    (1 << 9)
#define I2C_INTD    (1 << 8)
#define I2C_ST    (1 << 7)
#define I2C_CLEAR    (1 << 4)
#define I2C_READ    1

#define I2C_START_READ    I2C_I2CEN|I2C_ST|I2C_CLEAR|I2C_READ
#define I2C_START_WRITE    I2C_I2CEN|I2C_ST

#define I2C_S_CLKT    (1 << 9)
#define I2C_S_ERR    (1 << 8)
#define I2C_S_RXF    (1 << 7)
#define I2C_S_TXE    (1 << 6)
#define I2C_S_RXD    (1 << 5)
#define I2C_S_TXD    (1 << 4)
#define I2C_S_RXR    (1 << 3)
#define I2C_S_TXW    (1 << 2)
#define I2C_S_DONE    (1 << 1)
#define I2C_S_TA    1

#define I2C_CLEAR_STATUS    I2C_S_CLKT|I2C_S_ERR|I2C_S_DONE

/// Size of memory page on RPi
#define BCM2835_PAGE_SIZE               (4*1024)
/// Size of memory block on RPi
#define BCM2835_BLOCK_SIZE              (4*1024)

#define I2C_C        *(bcm2835_i2c.addr + 0x00)
#define I2C_S        *(bcm2835_i2c.addr + 0x01)
#define I2C_DLEN     *(bcm2835_i2c.addr + 0x02)
#define I2C_A        *(bcm2835_i2c.addr + 0x03)
#define I2C_FIFO     *(bcm2835_i2c.addr + 0x04)

#ifndef MAP_FAILED
#define MAP_FAILED ((void *) -1)
#endif

typedef struct bcm2835_i2c_peripheral_tag {
	unsigned long addr_p;  	// physical address to map to memory file
	int mem_fd; 			// memory file descriptor
	void *map;  			// pointer to memory mapped pointer
	volatile unsigned int *addr; // address to memory map
} bcm2835_i2c_peripheral;

/// Base of the GPIO registers.
/// Available after bcm2835_init has been called
extern bcm2835_i2c_peripheral bcm2835_i2c;

// file methods
extern int bcm2835_i2c_file_open(int chan, int addr);
extern void bcm2835_i2c_file_close(int handle);
extern int bcm2835_i2c_file_write(int file_handle, unsigned char * buf, int len);
extern int bcm2835_i2c_file_read(int file_handle, unsigned char * buf, int len);

// memory methods
/// Initialise the library by opening /dev/mem and getting pointers to the
/// internal memory for BCM 2835 device registers. You must call this (successfully)
/// before calling any other
/// functions in this library (except bcm2835_set_debug).
/// If bcm2835_init() fails by returning 0,
/// calling any other function may result in crashes or other failures.
/// Prints messages to stderr in case of errors.
/// \return 1 if successful else 0
extern int bcm2835_i2c_mem_init(void);

/// Close the library, deallocating any allocated memory and closing /dev/mem
/// \return 1 if successful else 0
extern int bcm2835_i2c_mem_close(void);

extern int bcm2835_i2c_mem_write(int addr, unsigned char *buf, int len);
extern int bcm2835_i2c_mem_read(int addr, unsigned char *buf, int len);

#endif

