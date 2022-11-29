#ifndef __PINS_H__
#define __PINS_H__

#define UNUSED1 0
#define UNUSED2 1
#define UNUSED5 6




// SPIClassSAMD(SERCOM *p_sercom, uint8_t uc_pinMISO, uint8_t uc_pinSCK, uint8_t uc_pinMOSI, SercomSpiTXPad PadTx, SercomRXPad PadRx)

#define SPI2_MOSI 2 // SERCOM2.2        SPI_[PAD_2]_SCK_x
#define SPI2_MISO 4 // SERCOM2.0_ALT    SERCOM_RX_PAD_0 ALTERNATE
#define SPI2_SCK  5 // SERCOM2.3        SPI_PAD_x_[SCK_3]

#define RF_NSS 10
#define RF_BUSY 7
#define RF_NRESET 8
#define RF_DIO1 1
#define LED1 13 // Shared with CHB_DIR - not a problem.

#define PIN_CHA_DIR   12
#define PIN_CHA_PWM    3 
#define PIN_CHA_BRAKE  9 
#define PIN_CHA_CURR   PIN_A0
#define PIN_CHB_DIR   13
#define PIN_CHB_PWM   11
#define PIN_CHB_BRAKE  9
#define PIN_CHB_CURR   PIN_A1

#endif // #ifndef __PINS_H__