
#define LCD_BIT_RS        0
#define LCD_BIT_E         2
#define LCD_BIT_BackLight 3
#define LCD_BIT_D4        4
#define LCD_BIT_D5        5
#define LCD_BIT_D6        6
#define LCD_BIT_D7        7


void LCD_writeNibble(uint8_t halfByte, bool isData){
  uint8_t data = isData ? (1<<LCD_BIT_RS) : 0; // RS

  data |= (1<<LCD_BIT_BackLight); // backlight

  // allow for arbitrary pin configuration
  if (halfByte & 0x01) data |= (1<<LCD_BIT_D4);
  if (halfByte & 0x02) data |= (1<<LCD_BIT_D5);
  if (halfByte & 0x04) data |= (1<<LCD_BIT_D6);
  if (halfByte & 0x08) data |= (1<<LCD_BIT_D7);

  wire.write(data | (1<<LCD_BIT_E) );
  delayMicroseconds(1); // enable pulse must be >450ns
  wire.write(data);
  delayMicroseconds(37); // commands need > 37us to settle
}

void LCD_sendNibble(uint8_t halfByte, bool isData){
  wire.beginTransmission(LCD_ADDRESS);
  LCD_writeNibble(halfByte, isData);
  wire.endTransmission();
}
// write either command or data
void LCD_send(uint8_t value, bool isData){
  // An I2C transmission has a significant overhead of ~10+1 I2C clock
  // cycles. We consequently only perform it only once per _send().

  wire.beginTransmission(LCD_ADDRESS);
  // write high 4 bits
  LCD_writeNibble((value >> 4 & 0x0F), isData);
  // write low 4 bits
  LCD_writeNibble((value & 0x0F), isData);
  wire.endTransmission();
}

void LCD_write2Wire(uint8_t data, bool isData, bool enable){
  if (isData) data |= (1<<LCD_BIT_RS);
  if (enable) data |= (1<<LCD_BIT_E);
  data |= (1<<LCD_BIT_BackLight);

  wire.beginTransmission(LCD_ADDRESS);
  wire.write(data);
  wire.endTransmission();
}

void LCD_goto(uint8_t row, uint8_t col){
  if( (col < 16) && (row < 2)) {
    LCD_send(0x80 | ((row ? 0x40 : 0) + col), false);
  }
}
void LCD_home(void){
  LCD_send(0x02, false);
  delay(2);
}
void LCD_write(uint8_t value){
  LCD_send(value, true);
}

void LCD_printDEC(unsigned int num, uint8_t width){
  if(width>=5) LCD_write( num > 10000? '0' + (num/10000)%10 : ' ' );
  if(width>=4) LCD_write( num > 1000 ? '0' + (num/1000 )%10 : ' ' );
  if(width>=3) LCD_write( num > 100  ? '0' + (num/100  )%10 : ' ' );
  if(width>=2) LCD_write( num > 10   ? '0' + (num/10   )%10 : ' ' );
  LCD_write( '0' + (num     )%10 );
}

void LCD_printHEX(uint16_t num, uint8_t width){
  char *str = "0123456789ABCDEF";

  if(width>=4) LCD_write( str[(num>>12) & 0xf] );
  if(width>=3) LCD_write( str[(num>> 8) & 0xf] );
  if(width>=2) LCD_write( str[(num>> 4) & 0xf] );
  LCD_write( str[ num & 0xf] );
}

void LCD_printStr(char *str){
  while(*str != '\0'){
    LCD_write( *str++ );
  }
}

void LCD_cursorOn(void){
  LCD_send(0x08 | 0x04 | 0x02, false);
}

void LCD_cursorOff(void){
  LCD_send(0x08 | 0x04, false);
}
