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

  unsigned long TX_period = 40000;  //us
  unsigned long ibus_frame_time = 7700; //us
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

  #ifdef RX_module
  Config_ICP1_PPM();
  #endif;

  set_data_rate_low();
}

void loop() {
  static unsigned long transmit_time = 0;
  wdt_reset();

  // send ibus every 7,7ms
  #ifdef RX_module
  if (micros() > ibus_frame_time) {
    
  }
  #endif
  
  // transmit every 10..20..40ms
  if (micros() > transmit_time) {
    transmit_time = micros() + TX_period;
    timer_start = micros();

    // Servos messsage - 16 chars
    for (byte i = 0; i < 8; i++)
    {
      TX_Buffer[(i * 2)] = Servo_Buffer[i] / 256;
      TX_Buffer[(i * 2) + 1] = Servo_Buffer[i] % 256;
    }
    SI4432_TX(16);
    
    timer_stop = micros();
    #ifdef DEBUG
      Serial.print(timer_stop - timer_start); Serial.println("us");
    #endif
  }
}

