#include "joystick.h"

#define JSUP "/sys/class/gpio/gpio26/value" //filepath for press value of joystick
#define JSRT "/sys/class/gpio/gpio47/value" ////1 is not pressed 0 is pressed
#define JSDN "/sys/class/gpio/gpio46/value"
#define JSLFT "/sys/class/gpio/gpio65/value"
#define JSPB "/sys/class/gpio/gpio27/value"


int readFromFileToScreen(char *path)
{
    FILE *pFile = fopen(path, "r");
    if (pFile == NULL) {
        printf("ERROR: Unable to open file (%s) for read\n", path);
        exit(-1);
    }
    // Read string (line)
    const int MAX_LENGTH = 1024;
    char buff[MAX_LENGTH];
    fgets(buff, MAX_LENGTH, pFile);
    // Close
    fclose(pFile);
    return atoi(buff);
}

static enum JoystickDirection getDirection(void)
{
    if(readFromFileToScreen(JSUP) == 0){
        return up;
    } else if (readFromFileToScreen(JSRT) == 0){
        return right;
    }   else if (readFromFileToScreen(JSDN) == 0){
        return down;
    }   else if (readFromFileToScreen(JSLFT) == 0){
        return left;
    } else if (readFromFileToSCreen(JSPB) == 0){
        return pushed;
    } else{
        return none;
    } 
}