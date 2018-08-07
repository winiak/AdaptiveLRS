void configurationStatus()
{
  Serial.print("Configuration status: ");
  // TX / RX / BAND scanner
  #ifdef TX_module
    Serial.println("TRANSMITTER");
  #endif
  #ifdef RX_module
    Serial.println("RECEIVER");
  #endif
  #ifdef BAND_scanner
   Serial.println("BAND SCANNER");
  #endif

  // Hardware configuration
  Serial.print("Hardware configuration: "); Serial.println(HW_config);

  // Transmit power
  Serial.print("Transmit power: "); 
  switch (power) {
    case 7 : Serial.println("7 == 100mW"); break;
    case 6 : Serial.println("6 == 50mW"); break;
    case 5 : Serial.println("5 == 25mW"); break;
    case 4 : Serial.println("4 == 13mW"); break;
    case 3 : Serial.println("3 == 6mW"); break;
    case 2 : Serial.println("2 == 3mW"); break;
    case 1 : Serial.println("1 == 1.6mW"); break;
    case 0 : Serial.println("0 == 1.3mW"); break;
    default: Serial.println("Incorrect power value!"); break;
  }
  
  // Base frequency
  Serial.print("Base frequency: "); Serial.println(base_frequency);
  
  // Main channel
  Serial.print("Main channel: "); Serial.println(channel);
  
  // Hopping channels
  Serial.print("Hopping channels: ");
  for (byte i = 0; i < sizeof(hop_list); i++) {
    Serial.print(hop_list[i]); Serial.print(" ");
  }
  Serial.println();
  
  // Header 
  Serial.print("Header: ");
  for (byte i = 0; i < sizeof(RF_Header); i++) {
    Serial.print(RF_Header[i]); Serial.print(" ");
  }
  Serial.println();
   
  // Input/communication type
  Serial.print("Communication type: ");
  #ifdef PPM_module
    Serial.println("Input PPM, using ICP for input/output signal, serial not used");
  #endif
  #ifdef IBUS_module
    Serial.println("Input IBUS, using UART RX/TX");
  #endif
  #ifdef FRSKY_module
    Serial.println("FRSKY not implemented yet");
  #endif
  
  // TX period
  Serial.print("Transmitting rate/period [us]: ");
  Serial.println(TX_period);
  
  // Data bus frame period
}

void Send_Servo_message()
{
  //servo_message musi mieć 16 bytów!!! 
    TX_Buffer[0] =  'M';
    TX_Buffer[1] = 103;   // MSP SERVO MESSAGE
    
    for (byte i = 1; i < 9; i++)
      {
        TX_Buffer[(i * 2)] = Servos[i] / 256;
        TX_Buffer[(i * 2) + 1] = Servos[i] % 256;
      }
            
    SI4432_TX(18);           //nadaj 18 bytów z bufora
}

void Send_RX_RSSI_message() {
  //TX_Buffer[0] = RX_RSSI;
}

void Send_Bridge_message()
{
  //servo_message musi mieć 18 bytów!!!  
        
    for (byte i = 0; i < 9; i++)
      {
        TX_Buffer[(i * 2)] = Servo_Buffer[i] / 256;
        TX_Buffer[(i * 2) + 1] = Servo_Buffer[i] % 256;
      }
            
    SI4432_TX(18);           //nadaj 18 bytów z bufora

}


void servoTester() {
  static unsigned int swipeval = 2000;
  static char sign = 1;
  static unsigned int sstep = 20;
  
  if (swipeval <= 2000)
    sign = 1;
  if (swipeval >= 4000)
    sign = -1;
  swipeval = swipeval + sstep * sign;

  for (char i=0; i<8; i++)
    Servos[i] = (i != 2 ? swipeval : Servos[i]);
}

#ifdef DEBUG > 0
void PrintHex8(uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes
{
   Serial.print("0x");
   for (int i=0; i<length; i++) {
     if (data[i]<0x10) {Serial.print("0");}
     Serial.print(data[i],HEX);
     Serial.print(" ");
   }
}
#endif


