

void Send_Servo_message()
{
  //servo_message musi mieć 16 bytów!!!  
        
    for (byte i = 0; i < 8; i++)
      {
        TX_Buffer[(i * 2)] = Servo_Buffer[i] / 256;
        TX_Buffer[(i * 2) + 1] = Servo_Buffer[i] % 256;
      }
            
    SI4432_TX(16);           //nadaj 16 bytów z bufora
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

