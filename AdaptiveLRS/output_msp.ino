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
