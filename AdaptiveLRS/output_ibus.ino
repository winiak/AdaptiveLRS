/**
 * iBus protocol generator by Konrad Winiarski
 * - why iBus? Latency! Frame sent to FC every 7ms!
 * - 14 channels by default !!
 * - support bi-directional communication (just in case)
 * 
 * Frame example:
 * x20x40   = Header (length of 32 bytes + command)
 * xDCx05  = 1500 ch 1
 * xDBx05  = 1499 ch 2
 * xEFx03  = 1007 ch 3
 * xDDx05  = 1501 ch 4
 * xD0x07  = 2000 ch 5
 * xD0x07  = 2000 ch 6
 * xDCx05  = 1500 ch 7
 * xDCx05  = 1500 ch 8
 * xDCx05  = 1500 ch 9
 * xDCx05  = 1500 ch 10
 * xDCx05  = 1500 ch 11
 * xDCx05  = 1500 ch 12
 * xDCx05  = 1500 ch 13
 * xDCx05  = 1500 ch 14
 * x54xF3  = Checksum:  0xFFFF - (0x20 + 0x40 ... sum of all !)
 * 
 * Thanks to Basejunction for introduction :)
 * https://basejunction.wordpress.com/2015/08/23/en-flysky-i6-14-channels-part1/
 */
#ifdef RX_module
#ifdef IBUS_module


#define IBUS_MAXCHANNELS 14
#include <SoftwareSerial.h>

SoftwareSerial mySerial(IBUS_RX_PIN, IBUS_TX_PIN); // RX, TX
void setup_module() {
  pinMode(IBUS_TX_PIN, OUTPUT);
  // serial setup
  mySerial.begin(115200);
}

//  if (mySerial.available()) {
//    Serial.write(mySerial.read());
//  }
//
//if (Serial.available()) {
//    mySerial.write(Serial.read());
//  }
  
void send_frame() {
  char frame_buffer[32];
  char ptr, len;
  unsigned int chksum = 0xFFFF - 0x20 - 0x40;
  unsigned int temp_servo;

  frame_buffer[0] = 0x20;   // frame length = 32 bytes 
  frame_buffer[1] = 0x40;   // command - servo data

  //
  for (byte i = 0; i < IBUS_MAXCHANNELS; i++) {
    temp_servo = (Servos[i] + SERVO_SHIFT) / 2;   // add shift (config) and make compatible (divide by 2)
    frame_buffer[(i * 2) + 2] =  (i < SERVO_CHANNELS ? temp_servo % 256 : 0xDC);  // set 1500 for unused channels
    chksum -= frame_buffer[(i * 2) + 2];
    frame_buffer[(i * 2) + 3] = (i < SERVO_CHANNELS ? temp_servo / 256 : 0x05);
    chksum -= frame_buffer[(i * 2) + 3];
  }

  frame_buffer[30] = chksum % 256;
  frame_buffer[31] = chksum / 256;

  mySerial.write(frame_buffer);
  
  #ifdef DEBUG
    PrintHex8(frame_buffer, 32);
    Serial.println();
    /*
    for (byte i = 0; i < 8; i++)
    {
      Serial.print((Servos[i] + SERVO_SHIFT) / 2);
      Serial.print(" ");
    }
    Serial.println();
    */
  #endif // DEBUG
}

void read_frame() {
  
}

#endif //IBUS_module
#endif //RX_module
