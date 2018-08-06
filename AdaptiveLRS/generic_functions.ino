
void Send_Servo_message()
{
  //servo_message musi mieć 16 bytów!!!  
        
    for (byte i = 0; i < 8; i++)
      {
        TX_Buffer[(i * 2)] = Servos[i] / 256;
        TX_Buffer[(i * 2) + 1] = Servos[i] % 256;
      }
            
    SI4432_TX(16);           //nadaj 16 bytów z bufora
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


