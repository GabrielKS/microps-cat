#include "STM32L432KC.h"
#include "I2C.h"
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_RCC.h"

// setup the I2C periperhal
void initI2C() {
    // enable in I2C1 in RCC
    RCC->APB1ENR1 |= (1<<RCC_APB1ENR1_I2C1EN);
    // disable digital filter for now
    I2C1->CR1 &= ~(0b1111<<I2C_CR1_DNF_Msk); // clear bits
    I2C1->CR1 |= _VAL2FLD(I2C_CR1_DNF, 0b0000);

    // 7 bit addressing mode
    I2C1->CR2 &= ~(1<<11);
    // recieve 2 bytes of data (roll, pg 35 of datasheet)
    I2C1->CR2 &= ~(0b11111111<<I2C_CR2_NBYTES_Msk); // clear bits
    I2C1->CR2 |= _VAL2FLD(I2C_CR2_NBYTES, 0b00000010);
    // stop generation

    // enable peripheral
    I2C1->CR1 |= (1<<0);
}
/*
1. Send the START bit (S).
2. Send the slave address (ADDR). Usually 7 bits.
3. Send the Read(R)-1 / Write(W)-0 bit.
4. Wait for/Send an acknowledge bit (A).
5. Send/Receive the data byte (8 bits) (DATA).
6. Expect/Send acknowledge bit (A).
7. Send the STOP bit (P).
*/
char I2CSendRecieve(char send) {

}