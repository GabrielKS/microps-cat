// STM32L432KC_SPI.h
// TODO: <YOUR NAME>
// TODO: <YOUR EMAIL>
// TODO: <DATE>
// TODO: <SHORT DESCRIPTION OF WHAT THIS FILE DOES>

#ifndef I2C
#define I2C

#include <stdint.h>
#include <stm32l432xx.h>
#include <core_cm4.h>

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

// Enables the I2C peripheral and sets stuff up
void initI2C();

// sends a byte of information (address) to the IMU and recieves a byte of information
// back
char I2CSendReceive(char send);

#endif