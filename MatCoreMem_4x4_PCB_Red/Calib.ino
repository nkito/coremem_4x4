
//extern uint8_t param_nnop[4][4];

int TrainCore(uint16_t temp, uint8_t row, uint8_t col){
  uint8_t da[2];
  uint8_t db[2];
  uint8_t pinc[2];
  uint8_t pinr[2];
  uint8_t res, i, min=MAX_NWAIT, max=0;

  col &= 3;
  row &= 3;

  switch(row){
    case 0: pinr[0] = BIT_CORE_R0L; pinr[1] = BIT_CORE_R0R; break;
    case 1: pinr[0] = BIT_CORE_R1R; pinr[1] = BIT_CORE_R1L; break;
    case 2: pinr[0] = BIT_CORE_R2L; pinr[1] = BIT_CORE_R2R; break;
    case 3: pinr[0] = BIT_CORE_R3R; pinr[1] = BIT_CORE_R3L; break;
  }
  switch(col){
    case 0: pinc[0] = BIT_CORE_C0N; pinc[1] = BIT_CORE_C0S; break;
    case 1: pinc[0] = BIT_CORE_C1S; pinc[1] = BIT_CORE_C1N; break;
    case 2: pinc[0] = BIT_CORE_C2N; pinc[1] = BIT_CORE_C2S; break;
    case 3: pinc[0] = BIT_CORE_C3S; pinc[1] = BIT_CORE_C3N; break;
  }

  Serial.print("(");
  Serial.print(row);
  Serial.print(", ");
  Serial.print(col);
  Serial.print(") : ");


  // Write 0
  digitalWrite(PIN_CORE_nOE , HIGH);
  digitalWrite(PIN_CORE_RCLK, LOW);
  shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, (1 << (pinc[0]-8)) );
  shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, (1 <<  pinr[0]   ) );
  digitalWrite(PIN_CORE_RCLK, HIGH);
  digitalWrite(PIN_CORE_RCLK, LOW);
  shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, 0 );
  shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, 0 );

  writeCoreExe(0);
  digitalWrite(PIN_CORE_RCLK, HIGH);
  digitalWrite(PIN_CORE_nOE , LOW);
#if DELAY_IN_CALIB > 15000
  delay(DELAY_IN_CALIB/1000);
#else
  delayMicroseconds(DELAY_IN_CALIB);
#endif

  for(i=0; i<MAX_NWAIT; i++){
    // Write 0
    digitalWrite(PIN_CORE_nOE , HIGH);
    digitalWrite(PIN_CORE_RCLK, LOW);
    shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, (1 << (pinc[0]-8)) );
    shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, (1 <<  pinr[0]   ) );
    digitalWrite(PIN_CORE_RCLK, HIGH);
    digitalWrite(PIN_CORE_RCLK, LOW);
    shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, 0 );
    shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, 0 );

    res = writeCoreExe(i);
    digitalWrite(PIN_CORE_RCLK, HIGH);
    digitalWrite(PIN_CORE_nOE , LOW);
#if DELAY_IN_CALIB > 15000
    delay(DELAY_IN_CALIB/1000);
#else
    delayMicroseconds(DELAY_IN_CALIB);
#endif

    da[0] = ((res & _BV(PIN_CORE_SENSEA-8)) ? 1 : 0);
    db[0] = ((res & _BV(PIN_CORE_SENSEB-8)) ? 1 : 0);

    // Write 1
    digitalWrite(PIN_CORE_nOE , HIGH);
    digitalWrite(PIN_CORE_RCLK, LOW);
    shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, (1 << (pinc[1]-8)) );
    shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, (1 <<  pinr[1]   ) );
    digitalWrite(PIN_CORE_RCLK, HIGH);
    digitalWrite(PIN_CORE_RCLK, LOW);
    shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, 0 );
    shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, 0 );

    res = writeCoreExe(i);
    digitalWrite(PIN_CORE_RCLK, HIGH);
    digitalWrite(PIN_CORE_nOE , LOW);
#if DELAY_IN_CALIB > 15000
    delay(DELAY_IN_CALIB/1000);
#else
    delayMicroseconds(DELAY_IN_CALIB);
#endif

    da[1] = ((res & _BV(PIN_CORE_SENSEA-8)) ? 1 : 0);
    db[1] = ((res & _BV(PIN_CORE_SENSEB-8)) ? 1 : 0);

    if( (da[0]&db[0]) && (da[1]^db[1]) ){
      min = ((min>i) ? i : min);
      max = ((max<i) ? i : max);
      Serial.print('*');
    }else{
      Serial.print(' ');
    }

    // Write 0
    digitalWrite(PIN_CORE_nOE , HIGH);
    digitalWrite(PIN_CORE_RCLK, LOW);
    shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, (1 << (pinc[0]-8)) );
    shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, (1 <<  pinr[0]   ) );
    digitalWrite(PIN_CORE_RCLK, HIGH);
    digitalWrite(PIN_CORE_RCLK, LOW);
    shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, 0 );
    shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, 0 );

    writeCoreExe(i);
    digitalWrite(PIN_CORE_RCLK, HIGH);
    digitalWrite(PIN_CORE_nOE , LOW);
#if DELAY_IN_CALIB > 15000
    delay(DELAY_IN_CALIB/1000);
#else
    delayMicroseconds(DELAY_IN_CALIB);
#endif
  }

  param_nnop[row][col] = (2*min + max)/4;
  Serial.println(param_nnop[row][col]);

  digitalWrite(PIN_CORE_nOE , HIGH);
  digitalWrite(PIN_CORE_RCLK, LOW);
  shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, 0 );
  shiftOut(PIN_CORE_SER, PIN_CORE_SRCLK, MSBFIRST, 0 );
  digitalWrite(PIN_CORE_RCLK, HIGH);
  digitalWrite(PIN_CORE_nOE , LOW);
}

void calibration(void){
  uint16_t temp = getTemp();

  Serial.println("- calibration 1st step -----------------------");
  for(int i=0; i<4; i++){
    for(int j=0; j<4; j++){
      TrainCore(temp, i, j);
      destReadCoreInt( temp );
    }
  }
  Serial.println("- calibration 2nd step -----------------------");
  for(int i=0; i<4; i++){
    for(int j=0; j<4; j++){
      TrainCore(temp, i, j);
      destReadCoreInt( temp );
    }
  }

/*
  Serial.println("- calibration 3rd step -----------------------");
  for(int loop=0; loop<5; loop++){
    Serial.print("-- loop : ");
    Serial.println(loop+1);

    unsigned int bit = 1;
    for(int i=0; i<16; bit=bit<<1, i++){
      destReadCoreInt( temp );
      writeCoreInt(temp, bit);
      Serial.print("(");
      Serial.print(i/4);
      Serial.print(", ");
      Serial.print(i&3);
      Serial.print(") : ");
      if( bit != destReadCoreInt( temp ) ){
        param_nnop[i/4][i&3]--;
        Serial.print("*");
      }else{
        Serial.print(" ");
      }
      Serial.println(param_nnop[i/4][i&3]);
    }
  }
*/
  for(int loop=0; loop<5; loop++){
    Serial.print("-- loop : ");
    Serial.println(loop+1);

    destReadCoreInt( temp );
    writeCoreInt(temp, 0xffff);
    unsigned int t = destReadCoreInt( temp );
    Serial.print(t, HEX);
    Serial.print(" ");

    unsigned int bit = 1;
    for(int i=0; i<16; bit=bit<<1, i++){
      if( (t & bit) == 0 ){
        Serial.print("(");
        Serial.print(i/4);
        Serial.print(", ");
        Serial.print(i&3);
        Serial.print(") : ");

        param_nnop[i/4][i&3]--;
        Serial.print("-");
        Serial.println(param_nnop[i/4][i&3]);
      }
    }
    Serial.println("");
  }

  for(int loop=0; loop<10; loop++){
    Serial.print("-- loop : ");
    Serial.println(loop+1);

    unsigned int bit = 1;
    unsigned int ibit;
    for(int i=0; i<16; bit=bit<<1, i++){
      ibit = 0xffff - bit;
      destReadCoreInt( temp );
      writeCoreInt(temp, ibit);
      Serial.print("(");
      Serial.print(i/4);
      Serial.print(", ");
      Serial.print(i&3);
      Serial.print(") : ");
      unsigned int t = destReadCoreInt( temp );
      Serial.print(t, HEX);
      Serial.print(" ");
      if( ibit != t && (t & bit) ){
        param_nnop[i/4][i&3]++;
        Serial.print("+");
      }
      Serial.println(param_nnop[i/4][i&3]);
    }
  }

  for(int loop=0; loop<5; loop++){
    Serial.print("-- loop : ");
    Serial.println(loop+1);

    unsigned int bit = 1;
    for(int i=0; i<16; bit=bit<<1, i++){
      destReadCoreInt( temp );
      writeCoreInt(temp, bit);
      Serial.print("(");
      Serial.print(i/4);
      Serial.print(", ");
      Serial.print(i&3);
      Serial.print(") : ");
      unsigned int t = destReadCoreInt( temp );
      Serial.print(t, HEX);
      Serial.print(" ");
      if( (bit != t) && ((bit&t)==0) ){
        param_nnop[i/4][i&3]--;
        Serial.print("*");
      }
      Serial.println(param_nnop[i/4][i&3]);
    }
  }

  for(int loop=0; loop<5; loop++){
    Serial.print("-- loop : ");
    Serial.println(loop+1);

    unsigned int bit = 1;
    unsigned int ibit;
    for(int i=0; i<16; bit=bit<<1, i++){
      ibit = 0xffff - bit;
      destReadCoreInt( temp );
      writeCoreInt(temp, ibit);
      Serial.print("(");
      Serial.print(i/4);
      Serial.print(", ");
      Serial.print(i&3);
      Serial.print(") : ");
      unsigned int t = destReadCoreInt( temp );
      Serial.print(t, HEX);
      Serial.print(" ");
      if( ibit != t && (t & bit) ){
        param_nnop[i/4][i&3]++;
        Serial.print("+");
      }
      Serial.println(param_nnop[i/4][i&3]);
    }
  }


  Serial.println("- calibration results ------------------------");
  Serial.print("- Temperature: ");
  Serial.println(temp);
  for(int i=0; i<16; i++){
    Serial.print("row ");
    Serial.print(i/4);
    Serial.print(", col ");
    Serial.print(i&3);
    Serial.print(" : ");
    Serial.println(param_nnop[i/4][i&3]);
  }
  Serial.println("----------------------------------------------");
}
