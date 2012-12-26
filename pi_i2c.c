#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "pi_i2c.h"

const char * const channel_location[] = {"/dev/i2c-0", "/dev/i2c-1"};

int i2c_connect(int chan, int address) {

        int handle = open("/dev/i2c-0", O_RDWR);
        if (handle >= 0) {
                if (ioctl(handle, I2C_SLAVE, address) < 0) {
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

void i2c_close(int handle) {
        close(handle);
}

int i2c_read(int handle, unsigned char * pbuf, int len) {
        return read(handle, pbuf, len);
}

int i2c_write(int handle, unsigned char * pbuf, int len) {
        return write(handle, pbuf, len);
}
