// STM32L432KC_SPI.c
// Gabriel Konar-Steenberg
// gmka2019@mymail.pomona.edu
// 2022-10-13
// Driver for the SPI1 interface

#include "STM32L432KC_SPI.h"

/* Enables the SPI peripheral and intializes its clock speed (baud rate), polarity, and phase.
 *    -- br: (0b000 - 0b111). The SPI clk will be the master clock / 2^(BR+1).
 *    -- cpol: clock polarity (0: inactive state is logical 0, 1: inactive state is logical 1).
 *    -- cpha: clock phase (0: data captured on leading edge of clk and changed on next edge, 
 *          1: data changed on leading edge of clk and captured on next edge)
 * Refer to the datasheet for more low-level details. */ 
void initSPI(int br, int cpol, int cpha) {
  RCC->APB2ENR |= _VAL2FLD(RCC_APB2ENR_SPI1EN, 1);  // Enable clock to SPI1

  pinMode(NSS_PIN, GPIO_OUTPUT);  // Set proper pin modes
  pinMode(SCK_PIN, GPIO_ALT);
  pinMode(MISO_PIN, GPIO_ALT);
  pinMode(MOSI_PIN, GPIO_ALT);

  GPIOA->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL5, 5);  // Set proper alternate functions
  GPIOA->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL6, 5);
  GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL5, 5);

  SPI1->CR1 |= _VAL2FLD(SPI_CR1_BR, br);  // "Clock rate using baud rate divisor"
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_MSTR, 1);  // Set to master
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPOL, cpol);  // Configure clock polarity
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPHA, cpha);  // Configure clock phase
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_LSBFIRST, 0);  // MSB first
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_SSM, 0);  // Disable SSM

  SPI1->CR2 |= _VAL2FLD(SPI_CR2_DS, 0b0111);  // 8-bit data size
  SPI1->CR2 |= _VAL2FLD(SPI_CR2_FRXTH, 1);  // Generate event on 8 bits received
  SPI1->CR2 |= _VAL2FLD(SPI_CR2_SSOE, 1);  // SS output enable

  SPI1->CR1 |= _VAL2FLD(SPI_CR1_SPE, 1);  // Enable SPI!
}

// Pull the enable line high
void spiOn() {
  digitalWrite(NSS_PIN, PIO_HIGH);
}

// Pull the enable line low
void spiOff() {
  digitalWrite(NSS_PIN, PIO_LOW);
}


/* Transmits a character (1 byte) over SPI and returns the received character.
 *    -- send: the character to send over SPI
 *    -- return: the character received over SPI */
char spiSendReceive(char send) {
  while (!_FLD2VAL(SPI_SR_TXE, SPI1->SR));  // Wait while transmit buffer not empty
  *((volatile char *) &(SPI1->DR)) = send;  // Send one byte
  while (!_FLD2VAL(SPI_SR_RXNE, SPI1->SR));  // Wait while receive buffer yes empty
  char recv = *((volatile char *) &(SPI1->DR));  // Receive one byte
  return recv;
}
