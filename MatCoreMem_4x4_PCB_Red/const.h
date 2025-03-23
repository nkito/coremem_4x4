#pragma once

#define DELAY_AFTER_READ  0
#define DELAY_IN_CALIB    (DELAY_AFTER_READ)

const int LCD_ADDRESS     = 0x27;


const int BIT_CORE_R0R    = 7;
const int BIT_CORE_R0L    = 6;
const int BIT_CORE_R1R    = 5;
const int BIT_CORE_R1L    = 4;
const int BIT_CORE_R2R    = 3;
const int BIT_CORE_R2L    = 2;
const int BIT_CORE_R3R    = 1;
const int BIT_CORE_R3L    = 0;

const int BIT_CORE_C0N    =  9;
const int BIT_CORE_C0S    =  8;
const int BIT_CORE_C1N    = 11;
const int BIT_CORE_C1S    = 10;
const int BIT_CORE_C2N    = 13;
const int BIT_CORE_C2S    = 12;
const int BIT_CORE_C3N    = 15;
const int BIT_CORE_C3S    = 14;

const int PIN_CORE_SER    = 7;
const int PIN_CORE_nOE    = 6;
const int PIN_CORE_RCLK   = 5;
const int PIN_CORE_SRCLK  = 4;

const int PIN_CORE_SENSEA = 10;
const int PIN_CORE_SENSEB = 11;

const int PIN_LED_INFO    =  8;
const int PIN_LED_ERR     =  9;
const int PIN_LED_BOARD   = 13;

const int PIN_SW0 = 12;
const int PIN_SW1 = A0;
const int PIN_SW2 = A1;
const int PIN_SW3 = A2;

const int APIN_TP = 3;

