//
// Created by dariusz on 20.02.17.
//

#define showDebugDataNRF24 0


#include <avr/io.h>
#include "slNRF24.h"
#include "slSPI.h"

#if showDebugDataNRF24
#include "slUart.h"
#endif


void slNRF24_SetupIo() {
    CE_OUTPUT();
    CSN_OUTPUT();
    CE_LOW();
    CSN_HIGH();
}

void slNRF24_Config() {
    CSN_LOW(); slSPI_TransferInt(FLUSH_TX); CSN_HIGH();
    CSN_LOW(); slSPI_TransferInt(W_REGISTER | STATUS); slSPI_TransferInt(MAX_RT); CSN_HIGH();//?
    CSN_LOW(); slSPI_TransferInt(W_REGISTER | RF_CH); slSPI_TransferInt(77); CSN_HIGH();//set channel
    CSN_LOW(); slSPI_TransferInt(W_REGISTER | RF_SETUP); slSPI_TransferInt(0x26); CSN_HIGH();//250Kbps and 0dBm strength of signal
    CSN_LOW(); slSPI_TransferInt(W_REGISTER | SETUP_RETR); slSPI_TransferInt(0x13); CSN_HIGH();//wait 500uS up to 3 retransmits
    CSN_LOW(); slSPI_TransferInt(W_REGISTER | EN_RXADDR); slSPI_TransferInt(ERX_P0); CSN_HIGH();//set reading pipe
    CSN_LOW(); slSPI_TransferInt(W_REGISTER | EN_AA); slSPI_TransferInt(ENAA_P0); CSN_HIGH();//enable ‘Auto Acknowledgment’
    CSN_LOW(); slSPI_TransferInt(W_REGISTER | SETUP_AW); slSPI_TransferInt(3); CSN_HIGH();//Setup of Address Widths
    CSN_LOW(); slSPI_TransferInt(W_REGISTER | TX_ADDR); slSPI_TransferInt(0xF0); slSPI_TransferInt(0xF0); slSPI_TransferInt(0xF0); slSPI_TransferInt(0xF0); slSPI_TransferInt(0xE1); CSN_HIGH();
    CSN_LOW(); slSPI_TransferInt(W_REGISTER | RX_ADDR_P0); slSPI_TransferInt(0xF0); slSPI_TransferInt(0xF0); slSPI_TransferInt(0xF0); slSPI_TransferInt(0xF0); slSPI_TransferInt(0xE1); CSN_HIGH();
}


void slNRF24_PowerUp(){
    //CSN_LOW(); slSPI_TransferInt(W_REGISTER | CONFIG); slSPI_TransferInt(1 << PWR_UP | 1 << EN_CRC | 1 << CRCO | 1 << MASK_RX_DR); CSN_HIGH();
    CSN_LOW(); slSPI_TransferInt(W_REGISTER | CONFIG); slSPI_TransferInt(1 << PWR_UP); CSN_HIGH();
}

void slNRF24_PowerDown(){
    CE_LOW();
    //CSN_LOW(); slSPI_TransferInt(W_REGISTER | CONFIG); slSPI_TransferInt(1 << EN_CRC | 1 << CRCO | 1 << MASK_RX_DR); CSN_HIGH();
    CSN_LOW(); slSPI_TransferInt(W_REGISTER | CONFIG); slSPI_TransferInt(1 << MASK_RX_DR); CSN_HIGH();
}

void slNRF24_Transmit(){
    CE_HIGH();
}

void slNRF24_StopTransmit(){
    CE_LOW();
}

void slNRF24_PacketBegin(){
    CSN_LOW();
    slSPI_TransferInt(W_TX_PAYLOAD);
}

void slNRF24_PacketEnd(){
    CSN_HIGH();
}

void slNRF24_Flush(){
    CSN_LOW(); slSPI_TransferInt(FLUSH_TX); CSN_HIGH();
}

void slNRF24_ClearMaxrt(){
    CSN_LOW(); slSPI_TransferInt(W_REGISTER | STATUS); slSPI_TransferInt(1 << MAX_RT | 1 << TX_DS); CSN_HIGH();
}
