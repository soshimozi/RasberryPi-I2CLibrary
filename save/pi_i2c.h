typedef unsigned int Handle;
typedef unsigned char byte;

typedef enum {
        Channel0 = 0,
        Channel1 = 1
} Channel;

int i2c_connect(int chan, int address);
void i2c_close(int handle);
int i2c_write(int handle, unsigned char * pbuf, int len);
int i2c_read(int handle, unsigned char * pbuf, int len);
