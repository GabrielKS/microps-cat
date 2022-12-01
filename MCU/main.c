/*
File: Lab_6_JHB.c
Author: Josh Brake
Email: jbrake@hmc.edu
Date: 9/14/19
*/
 // http://192.168.4.1/.

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"

/////////////////////////////////////////////////////////////////
// Provided Constants and Functions
/////////////////////////////////////////////////////////////////



//determines whether a given character sequence is in a char array request, returning 1 if present, -1 if not present
int inString(char request[], char des[]) {
	if (strstr(request, des) != NULL) {return 1;}
	return -1;
}

void print_float(float f)
{
  int before_point = (int)(f);
  int after_point = (int)((f - before_point) * 1000);
  after_point =  abs(after_point);

  printf("%d.%d\n", before_point, after_point);
}

/////////////////////////////////////////////////////////////////
// Solution Functions
/////////////////////////////////////////////////////////////////


int main(void) {
  configureFlash();
  configureClock();

  gpioEnable(GPIO_PORT_A);
  gpioEnable(GPIO_PORT_B);
  gpioEnable(GPIO_PORT_C);

  pinMode(PB3, GPIO_OUTPUT);
  
  RCC->APB2ENR |= (RCC_APB2ENR_TIM15EN);
  initTIM(TIM15);
  // set up the USART with the correct baud rate
  int baudRate =  115200;
  USART_TypeDef * imu = initUSART(USART1_ID, baudRate);
 // define the commands and addresses of registers
  char start = 0xAA; // see pg 94 of datasheet
  char write = 0x00;
  char opr_address = 0x3D;
  char read = 0x01;
  char imu_mode = 0x01; // see page 21 of BNO055 datasheet
  char ndof_mode = 0x0C;
  char rollMSB_address = 0x1D; // see pg 53 of BNO55 datasheet
  char rollLSB_address = 0x1C;
  char send_length = 0x02; // hypothesis: 2 bytes (LSB then MSB) but for now just MSB
  
 
  
   volatile int iter = 0;
   // set up operation mode
  sendChar(imu, start); 
  sendChar(imu, write); 
  sendChar(imu, opr_address); 
  sendChar(imu, 0x01);
  sendChar(imu, ndof_mode); 
  readChar(imu);
  volatile char opr_mode_status = readChar(imu);

  for(volatile int i = 0; i < 20000; i++);

  while(1) {
  
  // send command to read data
  sendChar(imu, start); // start byte (see pg 94 of datasheet)
  sendChar(imu, read); // read
  sendChar(imu, rollMSB_address);
  sendChar(imu, send_length);

  volatile char byte1 = readChar(imu);
  volatile char length_or_status = readChar(imu);
  volatile signed char rollMSB, rollLSB;
  volatile int16_t totalRoll;
 
  if(byte1 == 0xBB)  {
    rollMSB =  readChar(imu);
    rollLSB =  readChar(imu);
    totalRoll = (rollMSB<<8)+rollLSB;
  }
  
  printf("%s", "iteration: ");
  printf("%d\n", iter);

  printf("%s", "opr_status: ");
  printf("%x\n", opr_mode_status);
  //if(byte1 == 0xBB) {
  //  printf("read success!\n");
  //  printf("%s", "length of byte or status: ");
  //  printf("%x\n", length_or_status);
  //}
  
  // convert int to rotations and degrees
   float roll_rotations =  totalRoll/5760.0;
   float roll_degrees = totalRoll/16.0;
  
  printf("%s", "length/status: ");
  printf("%x\n", length_or_status);
  printf("%s", "should be bb: ");
  printf("%x\n",byte1);
  printf("%s", "rollMSB: ");
  printf("%d\n", rollMSB);
  printf("%s", "rollLSB: ");
  printf("%d\n", rollLSB);
  printf("%s", "total: ");
  printf("%d\n", totalRoll);
  printf("%s", "degrees roll: ");
  print_float(roll_degrees);
  printf("%s", "rotations: ");
  print_float(roll_rotations);
 
  iter++;
  
  for(volatile int i = 0; i < 200000; i++);

  //// figure out number to send through SPI
  //float moment_robot = 29; // calculate the moment of inertia later
  //float moment_wheel = 1; // ^^
  //// hardcode motor_spin later
  //float motor_spin = roll_rotations*((1+moment_robot)/moment_wheel);
  //uint8_t encoder_conversion = 20; // place holder number, could be 80 
  
  //float sendSPI =  motor_spin*encoder_conversion;//can probably hard code

  }
}