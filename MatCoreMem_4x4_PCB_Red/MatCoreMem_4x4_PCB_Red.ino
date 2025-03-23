#include <LiquidCrystal_PCF8574.h>
#include "const.h"

#define SDA_PIN 2
#define SCL_PIN 3
#include <SoftWire.h>
SoftWire wire(SDA_PIN, SCL_PIN);

char swTxBuffer[16];
char swRxBuffer[16];

void setLED(int val){
/*
  digitalWrite(PIN_LED_B0, (val & 1) ? HIGH : LOW);
  digitalWrite(PIN_LED_B1, (val & 2) ? HIGH : LOW);
  digitalWrite(PIN_LED_B2, (val & 4) ? HIGH : LOW);
  digitalWrite(PIN_LED_B3, (val & 8) ? HIGH : LOW);
*/
}

uint8_t getSW(void){
  uint8_t val = 0;

  if( ! digitalRead(PIN_SW0) ) val |= 1;
  if( ! digitalRead(PIN_SW1) ) val |= 2;
  if( ! digitalRead(PIN_SW2) ) val |= 4;
  if( ! digitalRead(PIN_SW3) ) val |= 8;

  return val;
}

uint16_t getTemp(void){
    float temp = (analogRead(APIN_TP) * 5000.0 / (4*1024.0) - 400)/(19.5);
    Serial.print("Temperature: ");
    Serial.println(temp);
    return ((uint16_t)(temp*100));
}


#define MAX_NWAIT   23
//#define CALIB_MODE   1

// param_nnop[row][col]
uint8_t param_nnop[4][4] = { 
  {  MAX_NWAIT,  MAX_NWAIT,  MAX_NWAIT,  MAX_NWAIT},
  {  MAX_NWAIT,  MAX_NWAIT,  MAX_NWAIT,  MAX_NWAIT},
  {  MAX_NWAIT,  MAX_NWAIT,  MAX_NWAIT,  MAX_NWAIT},
  {  MAX_NWAIT,  MAX_NWAIT,  MAX_NWAIT,  MAX_NWAIT}
};

void setupParameter(uint16_t temp){
  param_nnop[0][0] = 6;
  param_nnop[0][1] = 6;
  param_nnop[0][2] = 7; //
  param_nnop[0][3] = 6;

  param_nnop[1][0] = 15;
  param_nnop[1][1] =  2;
  param_nnop[1][2] = 18;
  param_nnop[1][3] = 18;

  param_nnop[2][0] = 12;
  param_nnop[2][1] = 4;
  param_nnop[2][2] = 2;
  param_nnop[2][3] = 13;

  param_nnop[3][0] = 6;
  param_nnop[3][1] = 12;
  param_nnop[3][2] = 12;
  param_nnop[3][3] = 5; //4;


  if(temp >= 1600) param_nnop[1][1] =  1;
  if(temp >= 1600) param_nnop[1][2] = 19;
  if(temp >= 1600) param_nnop[1][3] = 19;
  if(temp >= 1600) param_nnop[3][1] = 13;

  if(temp >= 2000) param_nnop[1][1] =  1;
  if(temp >= 2000) param_nnop[1][2] = 21;
  if(temp >= 2000) param_nnop[1][3] = 20;
  if(temp >= 2000) param_nnop[3][1] = 13;

  if(temp >= 2220) param_nnop[1][3] = 21;

  if(temp >= 2350) param_nnop[1][1] =  0;
  if(temp >= 2350) param_nnop[1][2] = 22;
  if(temp >= 2350) param_nnop[3][1] = 14;

  if(temp >= 2450) param_nnop[1][3] = 22;

}


#define CAPTURE_HEAD                       \
    asm volatile(                          \
      "cli                        \n\t"    \
      "out %[port], %[portv_oe]   \n\t"    \
      "nop                        \n\t"    \
      "nop                        \n\t"    
#define CAPTURE_NOP                        \
      "nop                        \n\t"
#define CAPTURE_TAIL                         \
      "in  %[res],  %[pin]        \n\t"      \
      "nop                        \n\t"      \
      "in  %[tmp],  %[pin]        \n\t"      \
      "and %[res],  %[tmp]        \n\t"      \
      "in  %[tmp],  %[pin]        \n\t"      \
      "and %[res],  %[tmp]        \n\t"      \
      "in  %[tmp],  %[pin]        \n\t"      \
      "and %[res],  %[tmp]        \n\t"      \
      "in  %[tmp],  %[pin]        \n\t"      \
      "and %[res],  %[tmp]        \n\t"      \
      "in  %[tmp],  %[pin]        \n\t"      \
      "and %[res],  %[tmp]        \n\t"      \
      "out %[port], %[portv_noe]  \n\t"      \
      "sei                        \n\t"      \
      :  [res]    "+r" (res),                \
         [tmp]    "+r" (tmp)                 \
      :  [port]   "I" (_SFR_IO_ADDR(PORTD)), \
         [pin]    "I" (_SFR_IO_ADDR(PINB)),  \
         [portv_oe]  "r" (portd_oe),         \
         [portv_noe] "r" (portd_noe) );



uint8_t writeCoreExe(uint8_t nnop){
  unsigned int portd_noe = (PORTD |   _BV(PIN_CORE_nOE));
  unsigned int portd_oe  = (PORTD & (~_BV(PIN_CORE_nOE)));
  uint8_t pb;
  uint8_t res, tmp;

  if( nnop == 0){
    CAPTURE_HEAD
    CAPTURE_TAIL
  }else if( nnop == 1 ){
    CAPTURE_HEAD
    CAPTURE_NOP
    CAPTURE_TAIL
  }else if( nnop == 2 ){
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }else if( nnop == 3 ){
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }else if( nnop == 4 ){
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }else if( nnop == 5 ){
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }else if( nnop == 6 ){
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }else if( nnop == 7 ){
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }else if( nnop == 8 ){
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }else if( nnop == 9 ){
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }else if( nnop == 10 ){
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }else if( nnop == 11 ){
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }else if( nnop == 12 ){
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }else if( nnop == 13 ){
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }else if( nnop == 14 ){
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }else if( nnop == 15 ){
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }else if( nnop == 16 ){
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }else if( nnop == 17 ){
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }else if( nnop == 18 ){
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }else if( nnop == 19 ){
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }else if( nnop == 20 ){
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }else if( nnop == 21 ){
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }else if( nnop == 22 ){
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }else{
    CAPTURE_HEAD
    CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP CAPTURE_NOP
    CAPTURE_TAIL
  }

  return res;
}

int writeCore(uint16_t temp, uint8_t row, uint8_t col, uint8_t val){
  uint8_t da, db;
  uint8_t pinc, pinr;
  uint8_t res;

  col &= 3;
  row &= 3;
  val = (val ? 1 : 0);

  switch(row){
    case 0: pinr = (val ? BIT_CORE_R0R : BIT_CORE_R0L); break;
    case 1: pinr = (val ? BIT_CORE_R1L : BIT_CORE_R1R); break;
    case 2: pinr = (val ? BIT_CORE_R2R : BIT_CORE_R2L); break;
    case 3: pinr = (val ? BIT_CORE_R3L : BIT_CORE_R3R); break;
  }
  switch(col){
    case 0: pinc = (val ? BIT_CORE_C0S : BIT_CORE_C0N); break;
    case 1: pinc = (val ? BIT_CORE_C1N : BIT_CORE_C1S); break;
    case 2: pinc = (val ? BIT_CORE_C2S : BIT_CORE_C2N); break;
    case 3: pinc = (val ? BIT_CORE_C3N : BIT_CORE_C3S); break;
  }

  digitalWrite(PIN_LED_BOARD, HIGH);
  digitalWrite(PIN_CORE_nOE , HIGH);

  digitalWrite(PIN_CORE_RCLK, LOW);
  shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, (1 << (pinc-8)) );
  shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, (1 <<  pinr   ) );
  digitalWrite(PIN_CORE_RCLK, HIGH);

  digitalWrite(PIN_CORE_RCLK, LOW);
  shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, 0 );
  shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, 0 );

  res = writeCoreExe(param_nnop[row][col]);
  digitalWrite(PIN_CORE_RCLK, HIGH);
  digitalWrite(PIN_CORE_nOE , LOW);
  digitalWrite(PIN_LED_BOARD, LOW);

  da = ((res & _BV(PIN_CORE_SENSEA-8)) ? 1 : 0);
  db = ((res & _BV(PIN_CORE_SENSEB-8)) ? 1 : 0);
  val = ( da^db ) ? 1-val : val;

//  delayMicroseconds(3000);
#if DELAY_AFTER_READ > 15000
  delay(DELAY_AFTER_READ/1000);
#else
  delayMicroseconds(DELAY_AFTER_READ);
#endif
  return val;
}

#define READ_VAL 0

void writeCoreInt(uint16_t temp, uint16_t val){

  if(((val&(1<< 7)) ? 1 : 0) != READ_VAL) writeCore(temp, 1, 3, 1-READ_VAL );
  if(((val&(1<< 6)) ? 1 : 0) != READ_VAL) writeCore(temp, 1, 2, 1-READ_VAL );
  if(((val&(1<< 4)) ? 1 : 0) != READ_VAL) writeCore(temp, 1, 0, 1-READ_VAL );

  if(((val&(1<< 3)) ? 1 : 0) != READ_VAL) writeCore(temp, 0, 3, 1-READ_VAL );
  if(((val&(1<< 2)) ? 1 : 0) != READ_VAL) writeCore(temp, 0, 2, 1-READ_VAL );
  if(((val&(1<< 1)) ? 1 : 0) != READ_VAL) writeCore(temp, 0, 1, 1-READ_VAL );
  if(((val&(1<< 0)) ? 1 : 0) != READ_VAL) writeCore(temp, 0, 0, 1-READ_VAL );

  if(((val&(1<<15)) ? 1 : 0) != READ_VAL) writeCore(temp, 3, 3, 1-READ_VAL );
  if(((val&(1<<14)) ? 1 : 0) != READ_VAL) writeCore(temp, 3, 2, 1-READ_VAL );
  if(((val&(1<<13)) ? 1 : 0) != READ_VAL) writeCore(temp, 3, 1, 1-READ_VAL );
  if(((val&(1<<12)) ? 1 : 0) != READ_VAL) writeCore(temp, 3, 0, 1-READ_VAL );

  if(((val&(1<<11)) ? 1 : 0) != READ_VAL) writeCore(temp, 2, 3, 1-READ_VAL );
  if(((val&(1<<10)) ? 1 : 0) != READ_VAL) writeCore(temp, 2, 2, 1-READ_VAL );
  if(((val&(1<< 9)) ? 1 : 0) != READ_VAL) writeCore(temp, 2, 1, 1-READ_VAL );
  if(((val&(1<< 8)) ? 1 : 0) != READ_VAL) writeCore(temp, 2, 0, 1-READ_VAL );

  if(((val&(1<< 5)) ? 1 : 0) != READ_VAL) writeCore(temp, 1, 1, 1-READ_VAL );

//  delayMicroseconds(DELAY_AFTER_READ*40);
}

unsigned int destReadCoreInt(uint16_t temp){
  unsigned int bit, res = 0;

  bit = writeCore(temp, 1, 3, READ_VAL ); res |= (bit<<7);
  bit = writeCore(temp, 1, 2, READ_VAL ); res |= (bit<<6);
  bit = writeCore(temp, 1, 0, READ_VAL ); res |= (bit<<4);

  bit = writeCore(temp, 0, 3, READ_VAL ); res |= (bit<<3);
  bit = writeCore(temp, 0, 2, READ_VAL ); res |= (bit<<2);
  bit = writeCore(temp, 0, 1, READ_VAL ); res |= (bit<<1);
  bit = writeCore(temp, 0, 0, READ_VAL ); res |= (bit<<0);

  bit = writeCore(temp, 3, 3, READ_VAL ); res |= (bit<<15);
  bit = writeCore(temp, 3, 2, READ_VAL ); res |= (bit<<14);
  bit = writeCore(temp, 3, 1, READ_VAL ); res |= (bit<<13);
  bit = writeCore(temp, 3, 0, READ_VAL ); res |= (bit<<12);

  bit = writeCore(temp, 2, 3, READ_VAL ); res |= (bit<<11);
  bit = writeCore(temp, 2, 2, READ_VAL ); res |= (bit<<10);
  bit = writeCore(temp, 2, 1, READ_VAL ); res |= (bit<<9);
  bit = writeCore(temp, 2, 0, READ_VAL ); res |= (bit<<8);

  bit = writeCore(temp, 1, 1, READ_VAL ); res |= (bit<<5);

  return res;
}


void setup() {
  Serial.begin( 115200 );

  digitalWrite(PIN_CORE_nOE, HIGH);
  pinMode     (PIN_CORE_nOE, OUTPUT);
  digitalWrite(PIN_CORE_nOE, HIGH);

  pinMode(PIN_CORE_SER,   OUTPUT);
  pinMode(PIN_CORE_RCLK , OUTPUT);
  pinMode(PIN_CORE_SRCLK, OUTPUT);

  pinMode(PIN_LED_INFO,  OUTPUT);
  pinMode(PIN_LED_ERR,   OUTPUT);
  pinMode(PIN_LED_BOARD, OUTPUT);

  pinMode(PIN_CORE_SENSEA, INPUT);
  pinMode(PIN_CORE_SENSEB, INPUT);

  digitalWrite(PIN_CORE_RCLK,  LOW);
  digitalWrite(PIN_CORE_SRCLK, LOW);
  shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, 0);
  shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, 0);
  digitalWrite(PIN_CORE_RCLK, HIGH);
  digitalWrite(PIN_CORE_nOE , LOW);

  delay(10);

  wire.setDelay_us(5);
  wire.begin();
  wire.setTxBuffer(swTxBuffer, sizeof(swTxBuffer));
  wire.setRxBuffer(swRxBuffer, sizeof(swRxBuffer));
  delay(50);

  LCD_write2Wire(0x00, false, false);
  delay(10);

  wire.beginTransmission(LCD_ADDRESS);
  int error = wire.endTransmission();
  Serial.print("Error: ");
  Serial.print(error);

  if( error == 0 ){
    LCD_sendNibble(0x03, false);
    delayMicroseconds(4500);
    LCD_sendNibble(0x03, false);
    delayMicroseconds(200);
    LCD_sendNibble(0x03, false);
    delayMicroseconds(200);
    LCD_sendNibble(0x02, false); // finally, set to 4-bit interface

    // Instruction: Function set = 0x20
    LCD_send(0x20 | 0x08 /*functionFlags*/, false);

    // Instruction: Display on/off control = 0x08
    LCD_send(0x08 | 0x04 | 0x02, false);
    LCD_send(0x01, false);
    delayMicroseconds(1600); // this command takes 1.5ms!

    LCD_send(0x04 | 0x02, false);

    LCD_cursorOff();
    LCD_printStr("4x4 Core Memory");
    delay(1000);

  } else {
    Serial.println(": LCD not found.");
  }

  uint16_t temp = getTemp();

#ifdef CALIB_MODE
  uint16_t  val = destReadCoreInt( temp );
  calibration();
  while(1) ;
#endif

  setupParameter(temp);
}

#define MODECNG_SEC 0x01


uint16_t lfsr16(uint16_t reg){
  uint16_t bit;

  bit = (reg & 0x0001) ^
        ((reg & 0x0004) >> 2) ^
        ((reg & 0x0008) >> 3) ^
        ((reg & 0x0020) >> 5);
  reg = ((reg >> 1) | (bit << 15));

  return reg;
}

void loop() {

  if( getSW() != 0 ){
    uint16_t temp = getTemp();
    destReadCoreInt( temp );
    setupParameter(temp);

    LCD_cursorOff();
    LCD_goto(0, 0);
    LCD_printStr("TEST ER   /     ");
    LCD_goto(1, 0);
    LCD_printStr("  .             ");
    LCD_goto(1, 0);
    LCD_printDEC(temp/100, 2);
    LCD_goto(1, 3);
    LCD_printDEC(temp% 10, 1);
    LCD_goto(1, 4);
    LCD_write(0xdf);
    LCD_write('C');

    while(1){
      static unsigned int pass  = 0;
      static unsigned int total = 0;

      static int i = 0;
      uint16_t val;
      uint16_t mask = 0xffff;// = 0xffff;

      static uint16_t testval = 1;

      testval = lfsr16(testval);

      if( ((i&0xff)==0) || (temp == 0xffff) ){
        temp = getTemp();
        setupParameter(temp);

        LCD_goto(1, 0);
        LCD_printDEC(temp/100, 2);
        LCD_goto(1, 3);
        LCD_printDEC(temp% 10, 1);

        LCD_goto(0,  7);
        LCD_printDEC(total - pass, 3);
        LCD_goto(0, 11);
        LCD_printDEC(total       , 5);
      }
      writeCoreInt(temp, testval&mask);
      //  delay(1);
      val = destReadCoreInt(temp);

      Serial.print("val: ");
      Serial.print(testval&mask, HEX);

      Serial.print(" -> ");
      Serial.print(val&mask, HEX);
      Serial.print( (val&mask) == (testval&mask) ? " Pass" : " Fail");

      if( ((val)&mask) == (testval&mask) ){
        pass++;
      }else{
        LCD_goto(1,  7);
        LCD_printHEX(testval&mask, 4);
        LCD_goto(1, 11);
        LCD_write(0x7e);
        LCD_goto(1, 12);
        LCD_printHEX(    val&mask, 4);
      }
      total++;
      Serial.print(" [Err ");
      Serial.print(total - pass);
      Serial.print("/");
      Serial.print(total);
      Serial.println("]");

      i++;
    }
  }else{

    uint8_t sw;
    uint8_t pos = 0;
    uint16_t temp;
    uint16_t  val;

    while(1){
      temp = getTemp();
      setupParameter(temp);
      val = destReadCoreInt( temp );
      writeCoreInt(temp, val);

      LCD_cursorOff();
      LCD_goto(1, 0);
      LCD_printStr("  .    val:     ");
      LCD_goto(1, 4);
      LCD_write(0xdf);
      LCD_write('C');

      LCD_goto(1, 0);
      LCD_printDEC(temp/100, 2);
      LCD_goto(1, 3);
      LCD_printDEC(temp%10, 1);

      LCD_goto(1, 12);
      LCD_printHEX(val, 4);

      LCD_goto(0, 0);

      for(int i=0; i<16; i++){
        LCD_send( (val & (1<<(15-i))) ? '1' : '0', true );
      }
      LCD_goto(0, 15 - pos);
      LCD_cursorOn();

      while( (sw=getSW()) == 0 ) ;
      delay(10);
      while( sw == getSW() ) ;

      if( sw == 1 ){
        pos = ((pos-1) & 0xf);
      }
      if( sw == 2 ){
        pos = ((pos+1) & 0xf);
      }
      if( (sw == 3) || (sw == 4) ){
        val = (val ^ (1U<<pos));
      }
      temp = getTemp();
      setupParameter(temp);
      destReadCoreInt(temp);
      writeCoreInt(temp, val);

      Serial.print(val,BIN);
      Serial.print(" ");
      Serial.println(val,HEX);
    }
  }
}
