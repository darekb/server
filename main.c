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
uint8_t pipe2[] = {0xF0, 0xF0, 0xF0, 0xF0, 0xD2};
uint8_t data[9];
uint8_t i = 0;
float t = 0;
struct MEASURE BME180measure = {0, 0, 0, 0, 11};
volatile uint8_t stage = 0;
volatile uint16_t counter = 0;
uint8_t t1[9] = {0x73, 0x74, 0x61, 0x72, 0x74, 0x2d, 0x73, 0x31, 0x31};

const char startStringSensor11[] = {'s','t','a','r','t','-','s','1','1'};

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

void setupTimer(){
    TCCR0B |= (1 << CS02) | (1 << CS00);//prescaler 1024
    TIMSK0 |= (1 << TOIE0);//przerwanie przy przepłnieniu timera0
    sei();
}

void nrf24_Start() {
    slNRF_Init();
    slNRF_OpenWritingPipe(pipe1, 9);
    slNRF_OpenReadingPipe(pipe2, 9, 1);
    slNRF_SetDataRate(RF24_2MBPS);
    slNRF_SetPALevel(RF24_PA_MAX);
    slNRF_SetChannel(100);
    slNRF_EnableDynamicPayloads();
    slNRF_EnableAckPayload();
    slNRF_SetRetries(0, 15);
    slNRF_AutoAck(1);
    slNRF_PowerUp();
    slNRF_StartListening();
}

//server

//stage1
void sensor11start() {
    slNRF_StopListening();
    slNRF_FlushTX();
    slNRF_FlushRX();
    if (!slNRF_Sent((uint8_t *) startStringSensor11, sizeof(startStringSensor11))) {
        slUART_WriteStringNl("Sensor11SendFail|z");
    } else {
        slUART_WriteStringNl("Sensor11SendOk|z");
    }
    slNRF_StartListening();
    clearData();
    stage = 2;
}

//stage 2
void waitForSensor11() {
    if (slNRF_Available()) {
        slNRF_Recive(data, sizeof(BME180measure));
        BME180measure = returnMEASUREFromBuffer(data);
        slNRF_FlushTX();
        slNRF_FlushRX();
        stage = 3;
    }
}

//stage 3
void sensor11sendViaUart() {
    slUART_LogDecWithSign(BME180measure.temperature);
    slUART_WriteString("|");
    slUART_LogDecWithSign(BME180measure.humidity);
    slUART_WriteString("|");
    slUART_LogDecWithSign(BME180measure.pressure);
    slUART_WriteString("|");
    slUART_LogDecWithSign(BME180measure.voltage);
    slUART_WriteString("|");
    slUART_LogDecWithSign(BME180measure.sensorId);
    slUART_WriteStringNl("|z");
    stage = 0;
}

ISR(TIMER0_OVF_vect) {
  //co 0.01632sek.
  counter = counter + 1;
  if (counter == 920) {//15.014400000000002 sek
    counter = 0;
    //if(stage == 0){
      stage = 1;
    //}
  }
}