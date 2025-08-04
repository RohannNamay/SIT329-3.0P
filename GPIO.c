#include "GPIO.h"
#include "Arduino.h"

bool Config_GPIO(char PortNum, char PinNum, uint8_t Mode) {
    if ((PortNum != 'A' && PortNum != 'B') || PinNum > 31) return false;
    
    PortGroup* port = (PortNum == 'A') ? &PORT->Group[0] : &PORT->Group[1]; // Select correct port group

    if (Mode == OUTPUT) {
        port->DIRSET.reg = (1 << PinNum); // Set direction to output
    } else if (Mode == INPUT) {
        port->DIRCLR.reg = (1 << PinNum); // Clear direction to input
        port->PINCFG[PinNum].bit.INEN = 1; // Enable input buffer
    } else {
        return false; 
    }
    return true;
}

bool Write_GPIO(char PortNum, char PinNum, bool State) {
    if ((PortNum != 'A' && PortNum != 'B') || PinNum > 31) return false;

    PortGroup* port = (PortNum == 'A') ? &PORT->Group[0] : &PORT->Group[1];

    if (State == HIGH) {
        port->OUTSET.reg = (1 << PinNum); // Set the output high by writing to OUTSET
    } else {
        port->OUTCLR.reg = (1 << PinNum); // Set the output low by writing to OUTCLR
    }
    return true;
}

bool Read_GPIO(char PortNum, char PinNum) {
    if ((PortNum != 'A' && PortNum != 'B') || PinNum > 31) return false;

    PortGroup* port = (PortNum == 'A') ? &PORT->Group[0] : &PORT->Group[1];
    return (port->IN.reg & (1 << PinNum)) != 0; // Check the input register and return the pin state
}
