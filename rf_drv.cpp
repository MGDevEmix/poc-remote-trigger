#include "rf_drv.h"

#include <SPI.h>                                               //the SX128X device is SPI based so load the SPI library                                         
#include "SX128XLT.h"                                          //include the appropriate library  
#include "Settings.h"                                          //include the settings file, frequencies, LoRa settings etc 
#include "pins.h"

uint8_t au8Packet[3];

void tx_packet_is_OK();
void tx_packet_is_Error();
void rx_packet_is_Error();
void rx_packet_is_OK();
void printElapsedTime();
void vdSendRawPacket(uint8_t* pu8Buf, uint8_t u8Len);
uint8_t u8RecvRawPacket(uint32_t u32Timeout_ms, uint8_t* pu8Packet, uint8_t u8MaxLen);

bool bDeviceOperational;

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

E_RF_PROFILE eSavedRfProfile;

void vdRfDrv_Init(E_RF_PROFILE eRfProfile)
{
  eSavedRfProfile = eRfProfile;
  SPI.begin();

  //SPI beginTranscation is normally part of library routines, but if it is disabled in library
  //a single instance is needed here, so uncomment the program line below
  //SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

  //setup hardware pins used by device, then check if device is found

  if (LT.begin(RF_NSS, RF_NRESET, RF_BUSY, RF_DIO1, RF_DIO2, RF_DIO3, RF_RX_EN, RF_TX_EN, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found!"));
    bDeviceOperational = true;
  }
  else
  {
    Serial.println(F("LoRa Device NOT found..."));
    bDeviceOperational = false;
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
}

bool bRfDrv_IsOperational(void)
{
  return bDeviceOperational;
}

void vdRfDrv_SendPacket(uint8_t u8Data)
{
  au8Packet[0] = U8_PKTVAL_BASE; // From.
  au8Packet[1] = U8_PKTVAL_CTRL; // To.
  au8Packet[2] = u8Data;
  vdSendRawPacket(au8Packet, 3);
}

bool bRfDrv_RecvPacketBlocking(uint32_t u32Timeout_ms, uint8_t* pu8Data)
{
  static uint8_t au8RxBuf[3];
  static bool bRet;
  bRet = false;
  if(3 == u8RecvRawPacket(u32Timeout_ms, au8RxBuf, sizeof(au8RxBuf))) 
  {
    if( (U8_PKTVAL_CTRL == au8RxBuf[0]) && // From
        (U8_PKTVAL_BASE == au8RxBuf[1]) )  // To
    {
      *pu8Data = au8RxBuf[2];
      bRet = true;
    }
  }
  return bRet;
}

bool bRfDrv_RecvStatusReqBlocking(uint32_t u32Timeout_ms)
{
  static uint8_t au8RxBuf[3];
  static bool bRet;
  bRet = false;
  Serial.print("bRfDrv_RecvStatusReqBlocking:");
  Serial.println(u32Timeout_ms);
  if(3 == u8RecvRawPacket(u32Timeout_ms, au8RxBuf, sizeof(au8RxBuf))) 
  {
    if( (U8_PKTVAL_CTRL == au8RxBuf[0]) && // From
        (U8_PKTVAL_BASE == au8RxBuf[1]) && // To
        (U8_PKTVAL_GET_STATUS == au8RxBuf[2]) )  
    {
      bRet = true;
    }
  }
  return bRet;
}

// ==================== PRIVATE =============================

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

void printElapsedTime()
{
  float seconds;
  seconds = millis() / 1000;
  Serial.print(seconds, 0);
  Serial.print(F("s"));
}

void vdPrintHexPacket(uint8_t* pu8Buf, uint8_t u8Len) {
  Serial.print("\n[");
  for(uint8_t i = 0; i < u8Len; i++) {
    Serial.print(pu8Buf[i], HEX);
    if(i < u8Len - 1) {
      Serial.print(' ');
    }
  }
  Serial.print(']');
}



void vdSendRawPacket(uint8_t* pu8Buf, uint8_t u8Len)
{
  Serial.print(TXpower);                                       //print the transmit power defined
  Serial.print(F("dBm "));
  Serial.print(F("Packet> "));
  Serial.flush();

  vdPrintHexPacket(pu8Buf, u8Len);

  digitalWrite(LED1, HIGH);
  startmS =  millis();                                         //start transmit timer
  if (LT.transmit(pu8Buf, u8Len, 10000, TXpower, WAIT_TX))   //will return packet length sent if OK, otherwise 0 if transmit, timeout 10 seconds
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

uint8_t u8RecvRawPacket(uint32_t u32Timeout_ms, uint8_t* pu8Packet, uint8_t u8MaxLen)
{
  uint8_t u8RetLen = 0;
  memset(pu8Packet, 0xFF, u8MaxLen);

  RXPacketL = LT.receive(RXBUFFER, RXBUFFER_SIZE, u32Timeout_ms, WAIT_RX); //wait for a packet to arrive with 60seconds (60000mS) timeout

  if(u8MaxLen >= RXPacketL) {
    u8RetLen = RXPacketL;
    memcpy(pu8Packet, RXBUFFER, u8RetLen);
  }
  
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
    vdPrintHexPacket(RXBUFFER, RXPacketL);
  }

  if (BUZZER > 0)
  {
    digitalWrite(BUZZER, LOW);                    //buzzer off
  }

  digitalWrite(LED1, LOW);                        //LED off

  Serial.println();

  return u8RetLen;
}
