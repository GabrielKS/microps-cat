// STM32L432KC_RCC.c
// Source code for RCC functions

#include "STM32L432KC_RCC.h"

//set clock to default 4MHz
void configureClock() {
  // turn on MSI clock (default 4MHz) and enable ready flag
  RCC->CR |=(0b11<<1);
  // set clock to MSI
  RCC->CFGR &= ~(0b1111<<3);
   //divide by 1
   RCC->CFGR &= ~(0b111<<30);
  // clock output MSI
  RCC->CFGR |= (0b0010<<27);
  RCC->APB1ENR1 |=(1<<4);  //enable TIM6
  RCC->APB2ENR |=(1<<17); //enable TIM16
  RCC->AHB2ENR |=(1<<0); //enable clk for GPIO A
  RCC->AHB2ENR |=(1<<1); // enable clk for GPIO B

}
