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

#include <SPI.h>                                               //the SX128X device is SPI based so load the SPI library                                         
#include <SX128XLT.h>                                          //include the appropriate library  
#include "Settings.h"                                          //include the setiings file, frequencies, LoRa settings etc  
#include "ArduinoLowPower.h"
#include <Adafruit_DotStar.h>
#include "timeout.h"

#define BUT_ARMD  12
#define BUT_TRIG 11
#define mBUT_IsArmdPressed() (digitalRead(BUT_ARMD) == 0)
#define mBUT_IsTrigPressed() (digitalRead(BUT_TRIG) == 0)

SX128XLT LT;                                                   //create a library class instance called LT

// Tx
uint8_t TXPacketL;
uint32_t TXPacketCount, startmS, endmS;
uint8_t buff[] = "Hello World 1234567890";

// Rx
uint32_t RXpacketCount;
uint32_t errors;
uint8_t RXBUFFER[RXBUFFER_SIZE];                 //create the buffer that received packets are copied into
uint8_t RXPacketL;                               //stores length of packet received
int16_t  PacketRSSI;                             //stores RSSI of received packet
int8_t  PacketSNR;                               //stores signal to noise ratio of received packet

Adafruit_DotStar rgbLed(1, 41, 40, DOTSTAR_BGR);

void setup() {
  pinMode(LED1, OUTPUT);                                   //setup pin as output for indicator LED
  led_Flash(2, 125);                                       //two quick LED flashes to indicate program start

  rgbLed.begin(); // Initialize pins for output
  rgbLed.setBrightness(10);
  rgbLed.setPixelColor(0, 255, 0, 0); // Red
  rgbLed.show();  // Turn all LEDs off ASAP

  // Set BUTs as INPUT_PULLUP to avoid spurious wakeup
  pinMode(BUT_ARMD, INPUT_PULLUP);
  pinMode(BUT_TRIG, INPUT_PULLUP);
  // Attach a wakeup interrupt on pin 8, calling repetitionsIncrease when the device is woken up
  LowPower.attachInterruptWakeup(BUT_ARMD, irqBut1WakeUp, FALLING);

  // Set BUT2 & BUT3 as input with PU.

  Serial.begin(9600);
  vdTimeoutSet(2000);
  while ((!Serial) && (!bTimeoutExpired()));
  Serial.println();
  Serial.print(F(__TIME__));
  Serial.print(F(" "));
  Serial.println(F(__DATE__));
  Serial.println(F(Program_Version));
  Serial.println();
  Serial.println(F("Controller Starting"));

  if (BUZZER > 0)
  {
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, HIGH);
    delay(50);
    digitalWrite(BUZZER, LOW);
  }

  SPI.begin();

  //SPI beginTranscation is normally part of library routines, but if it is disabled in library
  //a single instance is needed here, so uncomment the program line below
  //SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  //setup hardware pins used by device, then check if device is found

  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, DIO2, DIO3, RX_EN, TX_EN, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
    led_Flash(2, 125);                                   //two further quick LED flashes to indicate device found
    delay(1000);
  }
  else
  {
    Serial.println(F("No device responding"));
    while (1)
    {
      led_Flash(50, 50);                                 //long fast speed LED flash indicates device error
    }
  }

  //The function call list below shows the complete setup for the LoRa device using the information defined in the
  //Settings.h file.
  //The 'Setup LoRa device' list below can be replaced with a single function call;
  //LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate);

  //***************************************************************************************************
  //Setup LoRa device
  //***************************************************************************************************
  LT.setMode(MODE_STDBY_RC);
  LT.setRegulatorMode(USE_LDO);
  LT.setPacketType(PACKET_TYPE_LORA);
  LT.setRfFrequency(Frequency, Offset);
  LT.setBufferBaseAddress(0, 0);
  LT.setModulationParams(SpreadingFactor, Bandwidth, CodeRate);
  LT.setPacketParams(12, LORA_PACKET_VARIABLE_LENGTH, 255, LORA_CRC_ON, LORA_IQ_NORMAL, 0, 0);
  LT.setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);
  LT.setHighSensitivity();
  //LT.setLowPowerRX();
  //***************************************************************************************************

  Serial.println();
  LT.printModemSettings();                               //reads and prints the configured LoRa settings, useful check
  Serial.println();
  LT.printOperatingSettings();                           //reads and prints the configured operating settings, useful check
  Serial.println();
  Serial.println();
  LT.printRegisters(0x900, 0x9FF);                       //print contents of device registers
  Serial.println();
  Serial.println();

  Serial.print(F("Controller ready"));
  Serial.println();
}

void loop() {
  while(1)
  {
  if(mBUT_IsArmdPressed()) 
  {
    rgbLed.setPixelColor(0, 0, 0, 255); 
  }
  else if(mBUT_IsTrigPressed())
  {
    rgbLed.setPixelColor(0,255,0,0);
  }
  else
  {
    rgbLed.setPixelColor(0,0,0,0);
  }
    
  rgbLed.show(); // Blue.
  }
  

  rgbLed.setPixelColor(0, 0, 0, 255); rgbLed.show(); // Blue.
  

  Serial.println("Going to sleep");
  delay(2000);

  rgbLed.setPixelColor(0, 0, 0, 0);
  rgbLed.show(); // Off.

  // Triggers an infinite sleep (the device will be woken up only by the registered wakeup sources)
  // The power consumption of the chip will drop consistently
  Serial.end();  
  USBDevice.detach();  
  LowPower.sleep();
  USBDevice.attach(); 
  Serial.begin(9600);
  vdTimeoutSet(2000);
  while((!Serial) && (!bTimeoutExpired()));
  Serial.println("hello");
  // Wakeup from BUT1.

  rgbLed.setPixelColor(0, 255, 165, 0);
  rgbLed.show(); // Orange.

//  setup();

  // Send packet.
  tx_send_packet();

  // Listen to response.
  rx_recv_packet(); // blocking.

  //delay(2000); // Wait at least 2s between packets (~debouncing for button).
  delay(10000);
}

void tx_send_packet()
{
    Serial.print(TXpower);                                       //print the transmit power defined
    Serial.print(F("dBm "));
    Serial.print(F("Packet> "));
    Serial.flush();

    TXPacketL = sizeof(buff);                                    //set TXPacketL to length of array
    buff[TXPacketL - 1] = '*';                                   //replace null character at buffer end so its visible on reciver

    LT.printASCIIPacket(buff, TXPacketL);                        //print the buffer (the sent packet) as ASCII

    digitalWrite(LED1, HIGH);
    startmS =  millis();                                         //start transmit timer
    if (LT.transmit(buff, TXPacketL, 10000, TXpower, WAIT_TX))   //will return packet length sent if OK, otherwise 0 if transmit, timeout 10 seconds
    {
      endmS = millis();                                          //packet sent, note end time
      TXPacketCount++;
      tx_packet_is_OK();
    }
    else
    {
      tx_packet_is_Error();                                 //transmit packet returned 0, there was an error
    }

    digitalWrite(LED1, LOW);
    Serial.println();
}

void tx_packet_is_OK()
{
  //if here packet has been sent OK
  uint16_t localCRC;

  Serial.print(F("  BytesSent,"));
  Serial.print(TXPacketL);                             //print transmitted packet length
  localCRC = LT.CRCCCITT(buff, TXPacketL, 0xFFFF);
  Serial.print(F("  CRC,"));
  Serial.print(localCRC, HEX);                              //print CRC of sent packet
  Serial.print(F("  TransmitTime,"));
  Serial.print(endmS - startmS);                       //print transmit time of packet
  Serial.print(F("mS"));
  Serial.print(F("  PacketsSent,"));
  Serial.print(TXPacketCount);                         //print total of packets sent OK
}

void tx_packet_is_Error()
{
  //if here there was an error transmitting packet
  uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();                  //read the the interrupt register
  Serial.print(F(" SendError,"));
  Serial.print(F("Length,"));
  Serial.print(TXPacketL);                         //print transmitted packet length
  Serial.print(F(",IRQreg,"));
  Serial.print(IRQStatus, HEX);                    //print IRQ status
  LT.printIrqStatus();                             //prints the text of which IRQs set
}

void rx_recv_packet()
{
  RXPacketL = LT.receive(RXBUFFER, RXBUFFER_SIZE, 60000, WAIT_RX); //wait for a packet to arrive with 60seconds (60000mS) timeout

  digitalWrite(LED1, HIGH);                      //something has happened

  if (BUZZER > 0)                                //turn buzzer on
  {
    digitalWrite(BUZZER, HIGH);
  }

  PacketRSSI = LT.readPacketRSSI();              //read the recived RSSI value
  PacketSNR = LT.readPacketSNR();                //read the received SNR value

  if (RXPacketL == 0)                            //if the LT.receive() function detects an error, RXpacketL == 0
  {
    rx_packet_is_Error();
  }
  else
  {
    rx_packet_is_OK();
  }

  if (BUZZER > 0)
  {
    digitalWrite(BUZZER, LOW);                    //buzzer off
  }

  digitalWrite(LED1, LOW);                        //LED off

  Serial.println();
}

void rx_packet_is_OK()
{
  uint16_t IRQStatus, localCRC;

  IRQStatus = LT.readIrqStatus();                  //read the LoRa device IRQ status register

  RXpacketCount++;

  printElapsedTime();                              //print elapsed time to Serial Monitor
  Serial.print(F("  "));
  LT.printASCIIPacket(RXBUFFER, RXPacketL);        //print the packet as ASCII characters

  localCRC = LT.CRCCCITT(RXBUFFER, RXPacketL, 0xFFFF);  //calculate the CRC, this is the external CRC calculation of the RXBUFFER
  Serial.print(F(",CRC,"));                        //contents, not the LoRa device internal CRC
  Serial.print(localCRC, HEX);
  Serial.print(F(",RSSI,"));
  Serial.print(PacketRSSI);
  Serial.print(F("dBm,SNR,"));
  Serial.print(PacketSNR);
  Serial.print(F("dB,Length,"));
  Serial.print(RXPacketL);
  Serial.print(F(",Packets,"));
  Serial.print(RXpacketCount);
  Serial.print(F(",Errors,"));
  Serial.print(errors);
  Serial.print(F(",IRQreg,"));
  Serial.print(IRQStatus, HEX);
}


void rx_packet_is_Error()
{
  uint16_t IRQStatus;
  IRQStatus = LT.readIrqStatus();                   //read the LoRa device IRQ status register

  printElapsedTime();                               //print elapsed time to Serial Monitor

  if (IRQStatus & IRQ_RX_TIMEOUT)                   //check for an RX timeout
  {
    Serial.print(F(" RXTimeout"));
  }
  else
  {
    errors++;
    Serial.print(F(" PacketError"));
    Serial.print(F(",RSSI,"));
    Serial.print(PacketRSSI);
    Serial.print(F("dBm,SNR,"));
    Serial.print(PacketSNR);
    Serial.print(F("dB,Length,"));
    Serial.print(LT.readRXPacketL());               //get the real packet length
    Serial.print(F(",Packets,"));
    Serial.print(RXpacketCount);
    Serial.print(F(",Errors,"));
    Serial.print(errors);
    Serial.print(F(",IRQreg,"));
    Serial.print(IRQStatus, HEX);
    LT.printIrqStatus();                            //print the names of the IRQ registers set
  }

  delay(250);                                       //gives a longer buzzer and LED flash for error

}

void led_Flash(uint16_t flashes, uint16_t delaymS)
{
  uint16_t index;
  for (index = 1; index <= flashes; index++)
  {
    digitalWrite(LED1, HIGH);
    delay(delaymS);
    digitalWrite(LED1, LOW);
    delay(delaymS);
  }
}

void printElapsedTime()
{
  float seconds;
  seconds = millis() / 1000;
  Serial.print(seconds, 0);
  Serial.print(F("s"));
}

void irqBut1WakeUp() {
  // This function will be called once on device wakeup
  // You can do some little operations here (like changing variables which will be used in the loop)
  // Remember to avoid calling delay() and long running functions since this functions executes in interrupt context
}
