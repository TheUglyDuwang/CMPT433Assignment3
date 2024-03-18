#include "accel.h"

static int initI2cBus(char* bus, int address)
{
int i2cFileDesc = open(bus, O_RDWR);
int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
if (result < 0) {
perror("I2C: Unable to set I2C device to slave address.");
exit(1);
}
return i2cFileDesc;
}




void AccelStart() {
    i2cFile = initI2cBus( BUS1, I2C_ADDRESS);
	char ar[2];
	ar[0] = WHO_AM_I;
	ar[1] = 0x01;
	write(i2cFile, ar, 2);
}

void AccelStop() {
   	char ar[2];
	ar[0] = 0x2A;
	ar[1] = 0x00;
	write(i2cFile, ar, 2);
    close(i2cFile);
}

int *getAccel() {
    char ar[7];
	static int array[3];

	write(i2cFile, 0x0, 1);
	if(read(i2cFile, ar, 7) != 7){
		printf("Error getting accel\n");
	}else{
		int16_t x = (ar[1] << 8) | ar[2];
		int16_t y = (ar[3] << 8) | ar[4];
		int16_t z = (ar[5] << 8) | ar[6];

		array[0] = x / 10000;
		array[1] = y / 10000;
		array[2] = z / 10000;
	}
    return array;
}
