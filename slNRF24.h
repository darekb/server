//
// Created by dariusz on 20.02.17.
//

#ifndef CMAKE_AVR_SLNRF24_H
#define CMAKE_AVR_SLNRF24_H

/* Bit Mnemonics */

#define RX_P_NO     1
#define TX_FULL     0

#define CE_PIN        PB1
#define CE_OUTPUT()   DDRB |= (1 << CE_PIN)
#define CE_HIGH()     PORTB |= (1 << CE_PIN)//enable RX
#define CE_LOW()      PORTB &= ~(1 << CE_PIN)//disable RX
#define CSN_PIN       PB2
#define CSN_OUTPUT()  DDRB |= (1 << CSN_PIN)
#define CSN_HIGH()    PORTB |= (1 << CSN_PIN)
#define CSN_LOW()     PORTB &= ~(1 << CSN_PIN)

#define rf24_max(a,b) (a>b?a:b)
#define rf24_min(a,b) (a<b?a:b)

#define NRF_SETTINGS (1 << MASK_TX_DS) | (1 << MASK_MAX_RT) | (1 << EN_CRC)
#define NRF_PAYLOAD_SIZE 4
#define NRF_ADDR_LEN 5

#define slNRF_CONFIG      0x00
#define slNRF_EN_AA       0x01
#define slNRF_RF_CH       0x05
#define slNRF_RF_SETUP    0x06
#define slNRF_RPD         0x09



// SPI Commands

#define R_REGISTER          0x00 // 000A AAAA
#define W_REGISTER          0x20 // 001A AAAA
#define R_RX_PAYLOAD        0x61 // 0110 0001
#define W_TX_PAYLOAD        0xA0 // 1010 0000
#define FLUSH_TX            0xE1 // 1110 0001
#define FLUSH_RX            0xE2 // 1110 0010
#define REUSE_TX_PL         0xE3 // 1110 0011
#define R_RX_PL_WID         0x60 // 0110 0000
#define W_ACK_PAYLOAD       0xA8 // 1010 1PPP
#define W_TX_PAYLOAD_NOACK  0xB0 // 1011 0000
#define NOP                 0xFF // 1111 1111


// Register Map

#define CONFIG      0x00
#define MASK_RX_DR  1 << 6
#define MASK_TX_DS  1 << 5
#define MASK_MAX_RT 1 << 4
#define EN_CRC      1 << 3
#define CRCO        1 << 2
#define PWR_UP      1 << 1
#define PRIM_RX     1 << 0

#define EN_AA       0x01
#define ENAA_P5     1 << 5
#define ENAA_P4     1 << 4
#define ENAA_P3     1 << 3
#define ENAA_P2     1 << 2
#define ENAA_P1     1 << 1
#define ENAA_P0     1 << 0

#define EN_RXADDR   0x02
#define ERX_P5      1 << 5
#define ERX_P4      1 << 4
#define ERX_P3      1 << 3
#define ERX_P2      1 << 2
#define ERX_P1      1 << 1
#define ERX_P0      1 << 0

#define SETUP_AW    0x03
#define AW          0
#define SETUP_AW_3  0x01 //3 bytes
#define SETUP_AW_4  0x10 //4 bytes
#define SETUP_AW_5  0x11 //5 bytes

#define SETUP_RETR  0x04
#define ARD         1 << 4
#define ARC         1 << 0

#define RF_CH       0x05

#define RF_SETUP    0x06
#define CONT_WAVE   1 << 7
#define RF_DR_LOW   1 << 5
#define PLL_LOCK    1 << 4
#define RF_DR_HIGH  1 << 3
#define RF_PWR      1 << 1

#define STATUS              0x07
#define RX_DR               1 << 6
#define TX_DS               1 << 5
#define MAX_RT              1 << 4
#define RX_P_NO_MASK        0x0E
#define STATUS_TX_FULL      1 << 0

#define OBSERVE_TX  0x08
#define PLOS_CNT    1 << 4
#define ARC_CNT     1 << 0

#define RPD         0x09

#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F

#define TX_ADDR     0x10

#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16

#define FIFO_STATUS         0x17
#define TX_REUSE            1 << 6
#define FIFO_TX_FULL        1 << 5
#define TX_EMPTY            1 << 4
#define RX_FULL             1 << 1
#define RX_EMPTY            1 << 0

#define DYNPD       0x1C
#define DPL_P5      1 << 5
#define DPL_P4      1 << 4
#define DPL_P3      1 << 3
#define DPL_P2      1 << 2
#define DPL_P1      1 << 1
#define DPL_P0      1 << 0

#define FEATURE     0x1D
#define EN_DPL      1 << 2
#define EN_ACK_PAY  1 << 1
#define EN_DYN_ACK  1 << 0

// #define RF24_1MBPS   1 << 0
// #define RF24_2MBPS   1 << 1
// #define RF24_250KBPS 1 << 2

//#define RF24_PA_MIN   1 << 0
//#define RF24_PA_LOW   1 << 1
//#define RF24_PA_HIGH  1 << 2
//#define RF24_PA_MAX   1 << 3
//#define RF24_PA_ERROR 1 << 4

#define PRIPSTR "%S"


/* P model bit Mnemonics */
#define RF_DR_LOW   1 << 5
#define RF_DR_HIGH  1 << 3
#define RF_PWR_LOW  1 << 1
#define RF_PWR_HIGH 1 << 2


void slNRF24_SetupIo();
void slNRF24_Config();
void slNRF24_PowerUp();
void slNRF24_PowerDown();
void slNRF24_Transmit();
void slNRF24_StopTransmit();
void slNRF24_PacketBegin();
void slNRF24_PacketEnd();
void slNRF24_Flush();
void slNRF24_ClearMaxrt();

#endif //CMAKE_AVR_SLNRF24_H
