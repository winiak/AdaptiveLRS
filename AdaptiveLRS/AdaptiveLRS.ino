/*****************************************************************************************************
***         AdaptiveLRS - Long range RC and data link with adaptive data rate by Konrad WiniJLRS   ***
***         based on Long Range RC & Data Link by Jacek Szostak                                    ***
***                                                                                                ***
***           Contains code from OpenLRS and Ultimate LRS                                          ***
***                         Optymized for MavLink and FrSky Telemetry                              ***
***                                                                                                ***
******************************************************************************************************
*/

#define SERIAL_TX_BUFFER_SIZE 128
#define SERIAL_RX_BUFFER_SIZE 128

#include "config.h"
#include <avr/wdt.h>

  unsigned char RF_Message_ID;
  unsigned char RF_Message_Len;
  unsigned char RF_Servo_message[16];
  unsigned char RF_Bridge_message[18];
  unsigned char TX_Buffer[64];

  uint8_t RX_RSSI = 0;
  uint8_t TX_RSSI = 0;

  unsigned long TX_period = 7000;  // 7700 / 20000 / 40000  us
  unsigned long ibus_frame_period = 7000; //us
  unsigned long timer_start, timer_stop;
  
  // temporary variables

void setup() {
  pinMode(SDO_pin, INPUT);
  pinMode(SDI_pin, OUTPUT);
  pinMode(SCLK_pin, OUTPUT);
  pinMode(IRQ_pin, INPUT);
  pinMode(nSel_pin, OUTPUT);

  Serial.begin(115200);
  Serial.println("Transciever starting...");
  wdt_enable(WDTO_250MS);  
  radio_init();

  setup_module();
  
  #ifdef RX_module
    #ifdef PPM_module
      //
    #endif //PPM_module
  #endif //RX_module
  
  #ifdef TX_module
    #ifdef PPM_module

    #endif //PPM_module
  #endif;
  //set_data_rate_low();
}

void loop() {
  static unsigned long transmit_time = 0;
  static unsigned long ibus_frame_time = 0;
  char i;
  wdt_reset();

  /**
   * RX module
   * - iBus frame tackle
   * - Servo tester
   * - PPM generation is done by TIMER interruption called by setup_module();
   */
  #ifdef RX_module
    #ifdef IBUS_module
    // send ibus every 7,7ms
    if (micros() > ibus_frame_time) {
      ibus_frame_time = micros() + ibus_frame_period;
      send_frame();
      timer_stop = micros();
      //timer_start = micros(); Serial.print(timer_stop - timer_start); Serial.println("us");
    }
    #endif //ibus_module
    
    #ifdef servo_tester_module
      if (micros() > transmit_time) {
        transmit_time = micros() + TX_period;
        servoTester();
      }
    #endif  //servo_tester_module
  #endif  //RX_module

  /**
   * TX module
   * - iBus module frame read
   */
  #ifdef TX_module  

    #ifdef IBUS_module
    read_frame();
    if (iBus_failed())
      for (i = 0; i < SERVO_CHANNELS; i++)
        Servos[i] = Servo_Failsafe[i];
    #endif //ibus_module
    
    #ifdef PPM_module
    if (check_PPM_corrupted())
      for (i = 0; i < SERVO_CHANNELS; i++)
        Servos[i] = Servo_Failsafe[i];
    else
      for (i = 0; i < SERVO_CHANNELS; i++)
        Servos[i] = Servo_Buffer[i];
    #endif //ppm_module


  // transmit every 10..20..40ms  -> let's try 7 (iBus only) / 20 / 40
  if (micros() > transmit_time) {
    transmit_time = micros() + TX_period;
    timer_start = micros();

    // Hopping
    Hopping();
    
    // Prepare and sent TX control packet
    Send_Servo_message();
    
 
    timer_stop = micros();
    #ifdef DEBUG
      Serial.print(timer_stop - timer_start); Serial.println("us");
    #endif
 }
 
  
  // Wait for RX to respond
  SI4432_checkRX();
  //Serial.println(RF_Servo_message);
  
  // Get RSSI
  // Serial.println(RX_RSSI);
  


   
  #endif  // TX_module

  /*
        //LED_feadback_8_HIGH;        
         last_message_time = micros();    //za XX ms powinno nastąpić kolejne odebranie danych
        failsafe_mode = 0;
        forced_hopping_counter = 0;
        //not_recived_frames = 0;
        forced_hopping_time = 41500;

        LED_feadback_8_LOW;
        Telemetry_Bridge_Write();
        Hopping();
   * /
   */
}

