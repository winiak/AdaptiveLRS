/**
 * See https://github.com/fdivitto/MSP/blob/master/MSP.h
 * and https://github.com/fdivitto/MSP/blob/master/MSP.cpp
 * 
 */


#ifdef RX_module
#ifdef MSP_module

#define MSP_MAX_SUPPORTED_SERVOS 8

void setup_module() {
    // module works on hardware serial - default 115200 - possible to change to softserial (but I see no reason at the moment)
}

void send_frame() {
  uint8_t frame_size;    // size TBC
  uint8_t messageID;     // message ID TBC
  uint8_t checksum = frame_size ^ messageID;
  char frame_buffer[32];
  unsigned int temp_servo, temp;
  byte i;
  
  frame_buffer[0] = '$';
  frame_buffer[1] = 'M';
  frame_buffer[2] = '<';
  frame_buffer[3] = frame_size;
  frame_buffer[4] = messageID;

  // Build frame
  for (i = 5; i < MSP_MAX_SUPPORTED_SERVOS + 5; i++) {
    temp_servo = (i < SERVO_CHANNELS ? (Servos[i] + SERVO_SHIFT)/2 : 1500);   // add shift (config) and make compatible (divide by 2)
    frame_buffer[(i * 2) + 2] =  temp_servo % 256;    
    checksum ^= (temp_servo % 256);
    frame_buffer[(i * 2) + 3] =  temp_servo / 256;
    checksum ^= (temp_servo / 256);
  }
  
  // Add CRC to frame
  frame_buffer[MSP_MAX_SUPPORTED_SERVOS * 2 + 5];

  // Send to serial (soft serial does not work at 115200) - HW UART MUST BE USED
  Serial.write(frame_buffer);
}

  /*
void MSP::send(uint8_t messageID, void * payload, uint8_t size)
{
  _stream->write('$');
  _stream->write('M');
  _stream->write('<');
  _stream->write(size);
  _stream->write(messageID);
  uint8_t checksum = size ^ messageID;
  uint8_t * payloadPtr = (uint8_t*)payload;
  for (uint8_t i = 0; i < size; ++i) {
    uint8_t b = *(payloadPtr++);
    checksum ^= b;
    _stream->write(b);
  }
  _stream->write(checksum);
}
   */
   
#endif // MSP_module
#endif // RX_module
