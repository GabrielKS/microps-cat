/* spi_demo.c
 * Gabriel Konar-Steenberg
 * 2022-11-14
 * Tests SPI communication from the MCU to the FPGA by sending the values of four digital input pins
 */

#include "STM32L432KC.h"


char spi_demo(char num) {
    // Set up the input pins
    gpioEnable(GPIO_PORT_A);
    gpioEnable(GPIO_PORT_B);
    //gpioEnable(GPIO_PORT_C);

    // Set up SPI (it initializes its own pins) and a timer
    initSPI(1, 0, 0);
    RCC->APB2ENR |= (RCC_APB2ENR_TIM15EN);
    initTIM(TIM15);
    
    // send number to FPGA and print number
    printf("%x\n", num);
    spiOn();
    spiSendReceive(num);
    spiOff();


}

