
#ifdef RX_module

volatile unsigned char current_Servo = 0;
volatile unsigned int total_ppm_time=0;


//=============================================================

void INIT_SERVO_DRIVER(void)
{
   TCCR1B   =   0x00;   //stop timer
   TCNT1H   =   0x00;   //setup
   TCNT1L   =   0x00;
   ICR1   =   40000;   // used for TOP, makes for 50 hz
   TCCR1A   =   0x02;   
   TCCR1B   =   0x1A; //start timer with 1/8 prescaler for 0.5us PPM resolution
   TIMSK1 = _BV (TOIE1); 
}


ISR(TIMER1_OVF_vect)
{
  unsigned int us;                    // this value is not real microseconds, we are using 0.5us resolution (2048 step), this is why the all values 2 times more than real microseconds.

  digitalWrite(PPM_OUT, LOW);
  digitalWrite(Servo1_OUT, LOW);
  digitalWrite(Servo2_OUT, LOW);
  digitalWrite(Servo3_OUT, LOW);
  digitalWrite(Servo4_OUT, LOW);
  digitalWrite(Servo5_OUT, LOW);
  digitalWrite(Servo6_OUT, LOW);
  digitalWrite(Servo7_OUT, LOW);
  digitalWrite(Servo8_OUT, LOW);
  //PORTD &= 0b00010111;                //zerowanie wszystkich wyjsc servos
  //PORTB &= 0b11110000;

  static  volatile byte chort_pulse = 0;  
  if (!chort_pulse)
    {
      digitalWrite(PPM_OUT, HIGH);   
      chort_pulse = 1;
      current_Servo++;                    // jump to next servo

     switch (current_Servo) 
      {
          case 1:
              digitalWrite(Servo1_OUT, HIGH);
              us = Servos[0];                   // read the servo timing from buffer
              total_ppm_time += us;             // calculate total servo signal times.
              break;
          case 2:
              digitalWrite(Servo2_OUT, HIGH);
              us = Servos[1];                   // read the servo timing from buffer
              total_ppm_time += us;             // calculate total servo signal times.
              break;
          case 3:
              digitalWrite(Servo3_OUT, HIGH);
              us = Servos[2];                   // read the servo timing from buffer
              total_ppm_time += us;             // calculate total servo signal times.
              break;
          case 4:
              digitalWrite(Servo4_OUT, HIGH);
              us = Servos[3];                   // read the servo timing from buffer
              total_ppm_time += us;             // calculate total servo signal times.
              break;
          case 5:
              digitalWrite(Servo5_OUT, HIGH);
              us = Servos[4];                   // read the servo timing from buffer
              total_ppm_time += us;             // calculate total servo signal times.
              break;
          case 6:
              digitalWrite(Servo6_OUT, HIGH);
              us = Servos[5];                   // read the servo timing from buffer
              total_ppm_time += us;             // calculate total servo signal times.
              break;
          case 7:
              digitalWrite(Servo7_OUT, HIGH);
              us = Servos[6];                   // read the servo timing from buffer
              total_ppm_time += us;             // calculate total servo signal times.
              break;
          case 8:
              digitalWrite(Servo8_OUT, HIGH);
              us = Servos[7];                   // read the servo timing from buffer
              total_ppm_time += us;             // calculate total servo signal times.
              break;
          case 9:
              us = 40000 - total_ppm_time;      //wait for total 20ms loop.  waiting time = 20.000us - total servo times
              total_ppm_time = 0;
              current_Servo = 0;
              break;
      }

}
else
{
  chort_pulse = 0;
  us = 600;
  total_ppm_time += us;
}
  TCNT1 = 40000 - us; // configure the timer interrupt for X micro seconds     

}

#endif

