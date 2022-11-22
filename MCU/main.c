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


/////////////////////////////////////////////////////////////////
// Solution Functions
/////////////////////////////////////////////////////////////////
void imuInit(){
  //// set up the USART with the correct baud rate
  //int baudRate =  115200;
  //USART_TypeDef * imu = initUSART(USART1_ID, baudRate);
  //sendChar(imu, 0xAA); // start byte (see pg 94 of datasheet)
  //sendChar(imu, 0x00); // write command
  //sendChar(imu, 0x3D); // OPR_MODE address
  //sendChar(imu, 0x01); // length 1 byte
  //sendChar(imu, 0x08); // write to put in IMU mode
  //char rehead = readChar(imu);
  //char restatus = readChar(imu);
}
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
   // set up operation mode
  sendChar(imu, 0xAA); // start byte (see pg 94 of datasheet)
  sendChar(imu, 0x00); // write command
  sendChar(imu, 0x3D); // OPR_MODE address
  sendChar(imu, 0x01); // length 1 byte
  sendChar(imu, 0x11); // write to put in IMU mode
  char rehead = readChar(imu);
  char restatus = readChar(imu);

  while(1) {
  // define the commands and addresses of registers
  char start = 0xAA;
  char read = 0x01;
  char rollMSB_address = 0x1D; // see pg 53 of BNO55 datasheet
  char rollLSB_address = 0x1C;
  char send_length = 0x01; // hypothesis: 2 bytes (LSB then MSB) but for now just MSB
  // make a command string to send
  //char * commandArray[5];
  //commandArray[0] = start;
  //commandArray[1] = read;
  //commandArray[2] = rollMSB_address;
  //commandArray[3] =  rollLSB_address;
  //commandArray[4] = send_length;

  // some dummy code to play around with for USART
 
  // sendString(imu, commandArray);

  

  // send command to read data
  sendChar(imu, start); // start byte (see pg 94 of datasheet)
  sendChar(imu, read); // read
  sendChar(imu, rollMSB_address);
  sendChar(imu, send_length);

  char response = readChar(imu);
  char response_length = readChar(imu);
  char rollMSB =  readChar(imu);
 
  printf("%x\n",rehead);
  printf("%x\n",restatus);
  printf("%x\n",rollMSB);

  
  for(volatile int i = 0; i < 20000; i++);
  }
}