#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "pi_i2c.h"

const char * const channel_location[] = {"/dev/i2c-0", "/dev/i2c-1"};

bcm2835_i2c_peripheral bcm2835_i2c = {BCM2835_I2C_BASE};

int map_peripheral(bcm2835_i2c_peripheral *p);
void unmap_peripheral(bcm2835_i2c_peripheral *p);

// Function to wait for the I2C transaction to complete
void wait_i2c_done() {
        //Wait till done, let's use a timeout just in case
        int timeout = 50;
        while((!((I2C_S) & I2C_S_DONE)) && --timeout) {
            usleep(1000);
        }
        if(timeout == 0)
            fprintf(stderr, "wait_i2c_done() timeout. Something went wrong.\n");
}

void i2c_strobe_read() {
	I2C_S = I2C_CLEAR_STATUS; // Reset status bits (see #define)
	I2C_C = I2C_START_READ;    // Start Read after clearing FIFO (see #define)

	wait_i2c_done();
}

void i2c_strobe_write() {
	I2C_S = I2C_CLEAR_STATUS;
	I2C_C = I2C_START_WRITE;
	
	wait_i2c_done();
}

int bcm2835_i2c_file_open(int chan, int addr) {

	if (chan > 1 || chan < 0) {
		fprintf(stderr, "Invalid channel.");
		return -1;
	}

        int handle = open(channel_location[chan], O_RDWR);

        if (handle >= 0) {

        	// set as a slave device
        	if (ioctl(handle, I2C_SLAVE, addr) < 0) {
        		return -1;
        	}

        	char buffer[7];
        	buffer[0] = 0x00;
        	int readBytes = write(handle, buffer, 1);
        	if (readBytes != 1) {
        		return -1;
        	}
        }

        return handle;
}

void bcm2835_i2c_file_close(int file_handle) {
        close(file_handle);
}

int bcm2835_i2c_file_read(int file_handle, unsigned char * buf, int len) {
        return read(file_handle, buf, len);
}

int bcm2835_i2c_file_write(int file_handle, unsigned char * buf, int len) {
        return write(file_handle, buf, len);
}

int bcm2835_i2c_mem_init(void) {

	if(map_peripheral(&bcm2835_i2c) == -1) {
		fprintf(stderr, "Failed to map the physical BSC0 (I2C) registers into the virtual memory space.\n");
		return 0;
	}

	return 1;
}

/// Close the library, deallocating any allocated memory and closing /dev/mem
/// \return 1 if successful else 0
int bcm2835_i2c_mem_close(void) {

    unmap_peripheral(&bcm2835_i2c);
    return 1;
}

// private functions

// Exposes the physical address defined in the passed structure using mmap on /dev/mem
int map_peripheral(bcm2835_i2c_peripheral *p) {

	// Open /dev/mem
	if ((p->mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
		perror("Failed to open /dev/mem, try checking permissions.\n");
		return -1;
	}

	p->map = mmap(
			NULL,
			BCM2835_BLOCK_SIZE,
			PROT_READ|PROT_WRITE,
			MAP_SHARED,
			p->mem_fd,  // File descriptor to physical memory virtual file '/dev/mem'
			p->addr_p      // Address in physical map that we want this memory block to expose
   );

   if (p->map == MAP_FAILED) {
        perror("mmap");
        return -1;
   }

   p->addr = (volatile unsigned int *)p->map;

   return 0;
}

int bcm2835_i2c_mem_write(int addr, unsigned char *buf, int len) {

	int i;

	I2C_A = addr;

	while(len) {
		if( len > 16 ) {
			// send full 16 byte buffer
			I2C_DLEN = 16;
			for(i = 0; i < 16; i++) {
				I2C_FIFO = buf[i];
			}
			i2c_strobe_write();

			len -= 16;
			buf += 16;

		} else {

			// otherwise send the remaining bytes
			I2C_DLEN = len;
			for(i = 0; i < len; i++) {
				I2C_FIFO = buf[i];
			}

			i2c_strobe_write();
			len = 0;
		}
	}

	return 1;
}

int bcm2835_i2c_mem_read(int addr, unsigned char *buf, int len) {
	int i;

	I2C_A = addr;

	while(len) {
		if( len > 16 ) {
			// send full 16 byte buffer
			I2C_DLEN = 16;
			for(i = 0; i < 16; i++) {
				buf[i] = I2C_FIFO;
			}
			i2c_strobe_read();

			len -= 16;
			buf += 16;

		} else {

			// otherwise send the remaining bytes
			I2C_DLEN = len;
			for(i = 0; i < len; i++) {
				buf[i] = I2C_FIFO;
			}
			i2c_strobe_read();

			len = 0;
		}
	}

	return 1;
}

void unmap_peripheral(bcm2835_i2c_peripheral *p) {
	munmap(p->map, BCM2835_BLOCK_SIZE);
	close(p->mem_fd);
}

#ifdef TEST
int main(void) {
	unsigned char buffer[3] = {'L', '1', '1'};
	
	printf("Test!\n");
	
	bcm2835_i2c_mem_init();
	
	bcm2835_i2c_mem_write(0x2A, buffer, 3);

	usleep(2000000);

	buffer[2] = '0';
	bcm2835_i2c_mem_write(0x2A, buffer, 3);

	usleep(2000000);
	
	bcm2835_i2c_mem_close();

	return 0;
	
}
#endif

