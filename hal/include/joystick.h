#ifndef JOYSTICK_H
#define JOYSTICK_H


enum JoystickDirection  {up, right, down, left, pushed, none};

int readFromFileToScreen(char *path);

static enum JoystickDirection getDirection(void);