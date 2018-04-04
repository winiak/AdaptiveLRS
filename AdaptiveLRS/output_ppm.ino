
#ifdef RX_module && PPM_module

//volatile unsigned char current_Servo = 0;
//volatile unsigned int total_ppm_time=0;
static unsigned char Servo_Number = 0;
unsigned int total_ppm_time = 0;

void setup_module() {
  pinMode(PPM_OUT, OUTPUT);
}

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

//############ SERVO INTERRUPT ##############
// We configured the ICR1 value for 40.000 into the "init_servo_driver" function. 
// It's mean this interrupt works when the Timer1 value equal 40.000
// we are configuring it for 40.000 - servo_signal_time for each servo channel, and The interrupt generating perfect servo timings for us.
// Timer1 configured for 1/8 CPU clock. with this configuration, each clock time is equal 0.5us and we are driving the servo with 2048 step resolution.
ISR(TIMER1_OVF_vect)
  {
  unsigned int us; // this value is not real microseconds, we are using 0.5us resolution (2048 step), this is why the all values 2 times more than real microseconds.
  
  // Servo_Ports_LOW;
  PPM_OUT_LOW;

  Servo_Number++; // jump to next servo
  if (Servo_Number>8) // back to the first servo 
    {
    total_ppm_time = 0; // clear the total servo ppm time
    Servo_Number=0;
    }
 

  if (Servo_Number == 8)  // Check the servo number. 
      {
        //Servos accepting 50hz ppm signal, this is why we are waiting for 20ms before second signal brust. 
        us = 40000 - total_ppm_time; //wait for total 20ms loop.  waiting time = 20.000us - total servo times
      }
      else
        us = Servos[Servo_Number]+SERVO_SHIFT; // read the servo timing from buffer
  
  total_ppm_time += us; // calculate total servo signal times.

  PPM_OUT_HIGH;
  
/* 
    switch (Servo_Number) {
      case 0:
        Servo1_OUT_HIGH;
        break;
      case 1:
        Servo2_OUT_HIGH;
        break;
      case 2:
        Servo3_OUT_HIGH;
        break;
      case 3:
        Servo4_OUT_HIGH;
        break;
      case 4:
       Servo5_OUT_HIGH;
        break;
      case 5:
        Servo6_OUT_HIGH;
        break;
      case 6:
        Servo7_OUT_HIGH;
        break;
      case 7:
        Servo8_OUT_HIGH;
        break;
      }  
  */  
  TCNT1 = 40000 - us; // configure the timer interrupt for X micro seconds     
}

#endif //(RX_module && PPM_module)

