/* spi_demo.c
 * Gabriel Konar-Steenberg
 * 2022-11-14
 * Tests SPI communication from the MCU to the FPGA by sending the values of four digital input pins
 */

#include "STM32L432KC.h"
#define in_1 PB0
#define in_2 PB6
#define in_3 PB7
#define in_4 PA8

int get_num() {
    return digitalRead(in_1) +
          (digitalRead(in_2) << 1) +
          (digitalRead(in_3) << 2) +
          (digitalRead(in_4) << 3);
}

int spi_demo(void) {
    // Set up SPI (it initializes its own pins) and a timer
    initSPI(1, 0, 0);
    RCC->APB2ENR |= (RCC_APB2ENR_TIM15EN);
    initTIM(TIM15);

    // Set up the input pins
    gpioEnable(GPIO_PORT_A);
    gpioEnable(GPIO_PORT_B);
    //gpioEnable(GPIO_PORT_C);

    pinMode(in_1, GPIO_INPUT);
    pinMode(in_2, GPIO_INPUT);
    pinMode(in_3, GPIO_INPUT);
    pinMode(in_4, GPIO_INPUT);

    while(1) {
        // Read the input pins and send the value
        char num = (char) get_num();
        printf("%d\n", num);
        spiOn();
        spiSendReceive(num);
        spiOff();

        // Wait a human-scale amount of time, then do it again
        delay_millis(TIM15, 250);
    }
}

