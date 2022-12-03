// STM32L432KC_SPI.h
// Gabriel Konar-Steenberg
// gmka2019@mymail.pomona.edu
// 2022-10-13
// Driver for the SPI1 interface

#ifndef STM32L4_SPI_H
#define STM32L4_SPI_H

#include <stdint.h>
#include <stm32l432xx.h>
#include "STM32L432KC.h"

// Pin choices
#define NSS_PIN PA11
#define SCK_PIN PB3
#define MISO_PIN PB4
#define MOSI_PIN PB5


///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

/* Enables the SPI peripheral and intializes its clock speed (baud rate), polarity, and phase.
 *    -- br: (0b000 - 0b111). The SPI clk will be the master clock / 2^(BR+1).
 *    -- cpol: clock polarity (0: inactive state is logical 0, 1: inactive state is logical 1).
 *    -- cpha: clock phase (0: data captured on leading edge of clk and changed on next edge, 
 *          1: data changed on leading edge of clk and captured on next edge)
 * Refer to the datasheet for more low-level details. */ 
void initSPI(int br, int cpol, int cpha);

// Pull the enable line high
void spiOn();

// Pull the enable line low
void spiOff();

/* Transmits a character (1 byte) over SPI and returns the received character.
 *    -- send: the character to send over SPI
 *    -- return: the character received over SPI */
char spiSendReceive(char send);

#endif
