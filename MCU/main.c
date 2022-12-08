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
#include "STM32L432KC.h"

/////////////////////////////////////////////////////////////////
// Provided Constants and Functions
/////////////////////////////////////////////////////////////////



//determines whether a given character sequence is in a char array request, returning 1 if present, -1 if not present
int inString(char request[], char des[]) {
	if (strstr(request, des) != NULL) {return 1;}
	return -1;
}

// print floats in debug mode
void print_float(float f)
{
  int before_point = (int)(f);
  int after_point = (int)((f - before_point) * 1000);
  after_point =  abs(after_point);

  printf("%d.%d\n", before_point, after_point);
}

// function to put imu in proper mode
volatile char initIMU(USART_TypeDef * imu, int mode) {
  // define the commands and addresses of registers
  char start = 0xAA; // see pg 94 of datasheet
  char write = 0x00;
  char opr_address = 0x3D;
  char imu_mode = 0x01; // see page 21 of BNO055 datasheet
  char ndof_mode = 0x0C;
  char gyro_only = 0x03;
  // do the actual mode
  if (mode == 0) {
       // set up operation mode
      sendChar(imu, start); 
      sendChar(imu, write); 
      sendChar(imu, opr_address); 
      sendChar(imu, 0x01);
      sendChar(imu, imu_mode); 
      readChar(imu);
     }
  else {
      sendChar(imu, start); 
      sendChar(imu, write); 
      sendChar(imu, opr_address); 
      sendChar(imu, 0x01);
      sendChar(imu, ndof_mode); 
      readChar(imu);
  }
    
  volatile char opr_mode_status = readChar(imu);
  return opr_mode_status;
}
// function to read roll 
/*void axisRemap(USART_TypeDef * imu) {
    // define the commands and addresses of registers
  char start = 0xAA; // see pg 94 of datasheet
  char write = 0x00;
  char read = 0x01;
  char axis_address = 0x24; /// pg 24 of datasheet
  
// eventually send rr 10 00 01
    sendChar(imu, start); 
    sendChar(imu, write); 
    sendChar(imu, axis_address); 
    sendChar(imu, 0x01);
    sendChar(imu, 0b00100001);
    readChar(imu);
    readChar(imu);
}*/
volatile int16_t readRoll(USART_TypeDef * imu, volatile int iter, volatile int16_t totalRoll) {
   // define the commands and addresses of registers
  char start = 0xAA; // see pg 94 of datasheet
  char read = 0x01;
  char rollMSB_address = 0x1f; // see pg 53 of BNO55 datasheet--> change to pitch
  char rollLSB_address = 0x1e;
  char send_length = 0x01; // hypothesis: 2 bytes (LSB then MSB) but for now just MSB
  
  volatile char byte1MSB, byte1LSB; //= readChar(imu);
  volatile char length_or_statusMSB, length_or_statusLSB;//= readChar(imu);
    

  volatile int16_t rollMSB;
  volatile unsigned char rollLSB;

 
   // read the roll MSB
  sendChar(imu, start); // start byte (see pg 94 of datasheet)
  sendChar(imu, read); // read
  sendChar(imu, rollMSB_address);
  sendChar(imu, send_length);
  byte1MSB = readChar(imu);
  length_or_statusMSB = readChar(imu);

  if (byte1MSB == 0xBB) {
    rollMSB = readChar(imu);
  }

  // read roll LSB
  sendChar(imu, start); // start byte (see pg 94 of datasheet)
  sendChar(imu, read); // read
  sendChar(imu, rollLSB_address);
  sendChar(imu, send_length);
  byte1LSB = readChar(imu);
  length_or_statusLSB = readChar(imu);

    if (byte1LSB == 0xBB) {
    rollLSB = readChar(imu);
  }

  /*if(byte1 == 0xBB)  {
    rollMSB =  readChar(imu);
    rollLSB =  readChar(imu);
    totalRoll = (rollMSB<<8)+rollLSB;
    totalRoll = totalRoll;
  }*/
   int16_t temp = (rollMSB<<8)+rollLSB;
   if (abs(temp-totalRoll)>0xA0&& iter>2);
   else totalRoll = (rollMSB<<8)+rollLSB;

    printf("%s", "rollMSB: ");
    printf("%x\n", rollMSB);
    printf("%s", "rollLSB: ");
    printf("%x\n", rollLSB);
    printf("%s", "byte1 LSB: ");
    printf("%x\n", byte1LSB);
    printf("%s", "message LSB: ");
    printf("%x\n", length_or_statusLSB);
    printf("%s", "byte1 MSB: ");
    printf("%x\n", byte1MSB);
    printf("%s", "message MSB: ");
    printf("%x\n", length_or_statusMSB);


  return totalRoll;
}

  
// function to read accel in the z axis
volatile int16_t readAccel(USART_TypeDef * imu) {
   // define the commands and addresses of registers
  char start = 0xAA; // see pg 94 of datasheet
  char read = 0x01;
  char accelMSB_adress = 0x0D; // see pg 52 of BNO55 datasheet, linear accel Z
  char accelLSB_address = 0x0C;
  char send_length = 0x02; // hypothesis: 2 bytes (LSB then MSB) but for now just MSB

  // read the accel
  sendChar(imu, start); // start byte (see pg 94 of datasheet)
  sendChar(imu, read); // read
  sendChar(imu, accelMSB_adress);
  sendChar(imu, send_length);

  volatile char byte1 = readChar(imu);
  volatile char length_or_status = readChar(imu);
  volatile signed char accelMSB, accelLSB;
  volatile int16_t totalAccel;
 

  if(byte1 == 0xBB)  {
    accelMSB =  readChar(imu);
    accelLSB =  readChar(imu);
    totalAccel = (accelMSB<<8)+accelLSB;
  }
  return totalAccel;
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
  pinMode(PB6, GPIO_OUTPUT);
  

  // initialize SPI and timer
  initSPI(1, 0, 0);

  RCC->APB2ENR |= (RCC_APB2ENR_TIM15EN);
  initTIM(TIM15);

 
  // set up the USART with the correct baud rate
  int baudRate =  115200;
  USART_TypeDef * imu = initUSART(USART1_ID, baudRate);

 
  
  volatile int iter = 0;
  volatile int16_t totalRoll;
  volatile char opr_mode_status = initIMU(imu, 1);
  //axisRemap(imu);
  volatile int led_count = 0;
  //start of loop
  while(1) {

    volatile int16_t totalRoll =  readRoll(imu, iter, totalRoll);
    volatile int16_t totalAccel = readAccel(imu);
  
    printf("%s", "iteration: ");
    printf("%d\n", iter);
 
  
    // convert int to rotations and degrees
     float roll_rotations =  totalRoll/5760.0;
     float roll_degrees = totalRoll/16.0;
     // convert accel to m/s^2
     float accelZ = totalAccel/100.0;

     // figure out number to send through SPI
    float moment_robot = 2; // calculate the moment of inertia later
    float moment_wheel = 1; // ^^
    // hardcode ratio later
    float ratio = ((1+moment_robot)/moment_wheel);
    float motor_spin = roll_rotations*ratio;
    uint8_t encoder_conversion = 80; // place holder number, could be 80 
  
    int16_t sendSPI =  motor_spin*encoder_conversion;//can probably hard code

    char sendSPI_MSB = sendSPI>>8;
    char sendSPI_LSB = sendSPI;

    spiOn();
    spiSendReceive(sendSPI_MSB);
    spiOff();

    spiOn();
    spiSendReceive(sendSPI_LSB);
    spiOff();
    volatile char nib = 0;
    nib |= ((((sendSPI_MSB)&(0b11)) << 2) |((sendSPI_LSB)&(0b11)));
     
    printf("%s", "totalRoll: ");
    printf("%d\n", totalRoll);
    printf("%s", "degrees roll: ");
    print_float(roll_degrees);
    printf("%s", "acceleration (m/s^2): ");
    print_float(accelZ);
    //printf("%s", "rotations: ");
    //printf("%d\n", roll_rotations);
    //print_float(roll_rotations);
 

    //printf("%s", "send SPI: ");
    //printf("%x\n", sendSPI);

    //printf("%x\n", sendSPI_MSB);
    //printf("%x\n", sendSPI_LSB);
    //printf("%s", "display: ");
    //printf("%x\n", nib);


 
    iter++;
   

    //// test code for driving LED high when dropped
    //if (led_count>100 && accelZ>5) {
    //  digitalWrite(PB6, 0); // turn off led after some delay
    //  led_count = 0;
    // } 

    if(accelZ<5.0) digitalWrite(PA12,1);
    else digitalWrite(PA12,0);

    led_count++;

  
    for(volatile int i = 0; i < 5000; i++);

 
  }
}