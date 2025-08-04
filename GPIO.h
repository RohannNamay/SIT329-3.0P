#ifndef GPIO_H
#define GPIO_H

#include <stdbool.h>
#include <stdint.h>
#include "sam.h"

// Function Prototypes
bool Config_GPIO(char PortNum, char PinNum, uint8_t Mode);
bool Write_GPIO(char PortNum, char PinNum, bool State);
bool Read_GPIO(char PortNum, char PinNum);

#endif 