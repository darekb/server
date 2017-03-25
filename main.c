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
#include "slSPI.h"

#define LED (1 << PB0)
#define LED_TOG PORTB ^= LED


void clearData();
void setupTimer();
void setupInt0();
void nrf24_Start();

//server
void sensor11start();
void getDataFromSensor();
void sensorSendDataViaUart();

uint8_t data[9];
uint8_t status;
struct MEASURE BME180measure = {0, 0, 0, 0, 11};
volatile uint8_t stage = 0;
volatile uint16_t counter = 0;
volatile uint16_t counter2 = 0;
char startStringSensor11[] = {'s', 't', 'a', 'r', 't', '-', 's', '1', '1'};

int main(void) {
    slUART_SimpleTransmitInit();
    slSPI_Init();
    slNRF24_IoInit();
    slNRF24_Init();
    setupTimer();
    setupInt0();
    sei();
    stage = 1;
    slUART_WriteStringNl("\nStart server");
    _delay_ms(5000);
    slNRF24_Reset();
    while (1) {
        switch (stage) {
            case 1:
                sensor11start();
                break;
            case 3:
                getDataFromSensor();
                break;
            case 4:
                sensorSendDataViaUart();
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
    TCCR0B |= (1 << CS02) | (1 << CS00);//prescaler 1024
    TIMSK0 |= (1 << TOIE0);//przerwanie przy przepłnieniu timera0
}

void setupInt0() {
   DDRD &= ~(1 << DDD2);     // Clear the PD2 pin
   // PD2 (PCINT0 pin) is now an input
   PORTD |= (1 << PORTD2);    // turn On the Pull-up
   // PD2 is now an input with pull-up enabled
   EICRA |=  (1<<ISC01);// INT0 falling edge    PD2
   EICRA  &=  ~(1<<ISC00);// INT0 falling edge PD2
   EIMSK |= (1 << INT0);     // Turns on INT0
}

//server

//stage1
void sensor11start() {
    counter = 0;
    slNRF24_Reset();
    slNRF24_FlushTx();
    slNRF24_FlushRx();
    slUART_WriteStringNl("Sensor11StartSending");
    slNRF24_TxPowerUp();
    slNRF24_TransmitPayload(&startStringSensor11, 9);
    clearData();
    stage = 0;//wait for interupt
}


//stage 3
void getDataFromSensor(){
    counter = 0;
    slNRF24_RxPowerUp();
    clearData();
    slNRF24_GetRegister(R_RX_PAYLOAD,data,9);
    stage = 4;
}

//stage 4
void sensorSendDataViaUart() {
    counter = 0;
    BME180measure = returnMEASUREFromBuffer(data);
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
    stage = 0;
    clearData();
}

ISR(TIMER0_OVF_vect) {
    //co 0.01632sek.
    if (stage == 0) {
        counter = counter + 1;
    } else {
        counter2 = counter2 + 1;
    }
    if (counter == 1840) {//30 sek Next mesurements
        counter = 0;
        counter2 = 0;
        stage = 1;
    }
    if (counter2 == 2840) {//46.348 sek Reset to sending
        counter = 0;
        counter2 = 0;
        stage = 1;
    }
}
ISR(INT0_vect){
    status = 0;
    slNRF24_GetRegister(STATUS, &status, 1);
    cli();
    if ((status & (1 << 6)) != 0) {
        //got data
        stage = 3;//getDataFromSensor
    }
    if ((status & (1 << 5)) != 0) {
        slNRF24_FlushTx();
        slNRF24_FlushRx();
        slNRF24_Reset();
        slNRF24_RxPowerUp();
        //sent data
        stage = 0;//wait for data from sensor
    }
    sei();
}
