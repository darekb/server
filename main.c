#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/wdt.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define showDebugDataMain 0

#include "main.h"
#include "slUart.h"
#include "slSPI.h"
#include "slNRF24.h"
#include "slBME180Measure.h"

#define LED (1 << PB0)
#define LED_TOG PORTB ^= LED


void clearData();
void setupTimer();
void nrf24_Start();

//server
void sensor11start();
void waitForSensor11();
void sensor11sendViaUart();


uint8_t pipe1[] = {0xF0, 0xF0, 0xF0, 0xF0, 0xE1};
uint8_t pipe2[] = {0xF0, 0xF0, 0xF0, 0xF0, 0x95};
uint8_t data[9];
//uint8_t i = 0;
float t = 0;
struct MEASURE BME180measure = {0, 0, 0, 0, 11};
volatile uint8_t stage = 0;
volatile uint8_t nextStage = 0;
volatile uint16_t counter = 0;
volatile uint16_t counter2 = 0;
uint8_t t1[9] = {0x73, 0x74, 0x61, 0x72, 0x74, 0x2d, 0x73, 0x31, 0x31};
char startStringSensor11[] = {'s', 't', 'a', 'r', 't', '-', 's', '1', '1'};

int main(void) {
    slUART_SimpleTransmitInit();
    setupTimer();
    nrf24_Start();
    stage = 1;
    while (1) {
        switch (stage) {
            case 1:
                sensor11start();
                break;
            case 2:
                waitForSensor11();
                break;
            case 3:
                sensor11sendViaUart();
                break;
        }
    }
    return 0;
}


void clearData() {
    for (uint8_t i = 0; i < 9; i++) {
        data[i] = 0;
    };
}

void setupTimer() {
    slUART_WriteStringNl("StartTimer");
    TCCR0B |= (1 << CS02) | (1 << CS00);//prescaler 1024
    TIMSK0 |= (1 << TOIE0);//przerwanie przy przepłnieniu timera0

    DDRD &= ~(1 << DDD2);     // Clear the PD2 pin
    // PD2 (PCINT0 pin) is now an input
    PORTD |= (1 << PORTD2);    // turn On the Pull-up
    // PD2 is now an input with pull-up enabled
    EICRA |= (1 << ISC00);    // set INT0 to trigger on ANY logic change
    EIMSK |= (1 << INT0);     // Turns on INT0

    sei();
}

void nrf24_Start() {
    slSPI_Init();
    slUART_WriteStringNl("nrf Start");
    slNRF24_SetupIo();
    slNRF24_Config();
    clearData();
}

//server

//stage1
void sensor11start() {
    counter = 0;
    nextStage = 0;
    slUART_WriteStringNl("Sensor11StartSending");
    slNRF24_PowerUp();
    _delay_ms(2000);
    slNRF24_PacketBegin();
    slSPI_TransferInt((uint8_t)startStringSensor11[0]);
    slSPI_TransferInt((uint8_t)startStringSensor11[1]);
    slSPI_TransferInt((uint8_t)startStringSensor11[3]);
    slSPI_TransferInt((uint8_t)startStringSensor11[4]);
    slSPI_TransferInt((uint8_t)startStringSensor11[6]);
    slSPI_TransferInt((uint8_t)startStringSensor11[7]);
    slSPI_TransferInt((uint8_t)startStringSensor11[8]);
    slNRF24_PacketEnd();
    slNRF24_Transmit();
    _delay_ms(5000);
    slNRF24_StopTransmit();
    slNRF24_Flush();
    slNRF24_ClearMaxrt();
    slNRF24_PowerDown();
    slUART_WriteStringNl("Sensor11SendOk");
    clearData();
    stage = nextStage;
}

//stage 2
void waitForSensor11() {
    nextStage = 3;
    counter = 0;
//    if (slNRF_Available()) {
//        //slUART_WriteStringNl("got data");
//        slNRF_Recive(data, sizeof(BME180measure));
//        BME180measure = returnMEASUREFromBuffer(data);
//        // _delay_ms(200);
//        stage = nextStage;
//    }
}

//stage 3
void sensor11sendViaUart() {
    nextStage = 0;
    counter = 0;
    slUART_LogDecWithSign(BME180measure.temperature);
    slUART_WriteString("|");
    slUART_LogDec(BME180measure.humidity);
    slUART_WriteString("|");
    slUART_LogDecWithSign(BME180measure.pressure);
    slUART_WriteString("|");
    slUART_LogDecWithSign(BME180measure.voltage);
    slUART_WriteString("|");
    slUART_LogDec(BME180measure.sensorId);
    slUART_WriteStringNl("");
    stage = nextStage;
    clearData();
}

ISR(TIMER0_OVF_vect) {
    //co 0.01632sek.
    if (stage == 0) {
        counter = counter + 1;
    } else {
        counter2 = counter2 + 1;
    }
    if (counter == 1840) {//30.028800000000004 sek
        counter = 0;
        counter2 = 0;
        stage = 1;
    }
    if (counter2 == 2840) {
        counter = 0;
        counter2 = 0;
        stage = nextStage;
    }
}


ISR (INT0_vect){
    slUART_WriteStringNl("0");
}