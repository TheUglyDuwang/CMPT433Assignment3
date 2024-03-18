#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define BUS1 "/dev/i2c-1"
#define I2C_ADDRESS 0x1C
#define WHO_AM_I 0x2A
#define XMSB 0x01
#define XLSB 0x02
#define YMSB 0x03
#define YLSB 0x04
#define ZMSB 0x05
#define ZLSB 0x06

static int i2cFile;

void AccelStart();
void AccelStop();
int *getAccel();
