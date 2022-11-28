/*******************************************************************************************************
  Programs for Arduino - Copyright of the author DevMG - 24/11/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This project is a POC for the remote trigger using sx1280. Firmware is based on
  Stuarts Project. It requires SX12XX-LoRa-master library to be installed.

  This branch is the remote controller, mainly a RF transmitter (but not only) that sends a command to the base. 
  It embeds, buttons, a buzzer and a RGB led. 

  The board is Arduino UNO + custom shield SX1280 with DLP-RFS1280.

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#define Program_Version "V0.0"


#include "led.h"
#include "serial_drv.h"
#include "rf_drv.h"
#include "app.h"
/*



*/


void setup() 
{

  vdLed_Init();       
  vdLed_FixBlue();
  vdSerialDrv_Init(Program_Version);
  vdRfDrv_Init();
  vdApp_Init();
  /*
  

  
  */
}

void loop() {
  vdApp_Task();

}

