
#ifdef TX_module
#ifdef IBUS_module

#define IBUS_BUFFSIZE 32
#define IBUS_MAXCHANNELS 14
static boolean rxFrameDone;
static uint8_t ibusIndex = 0;
static uint8_t ibus[IBUS_BUFFSIZE] = {0};
static long last_frame_timer = 0;
static bool no_tx_status_changed = false;

//SERVO_CHANNELS
//Servos[Servo_Number]

bool iBus_failed() {
  return no_tx_status_changed;  
}

void setup_module() {
    // module works on hardware serial - must be 115200
}

void read_frame() {
  uint8_t i;
  uint16_t chksum, rxsum;

  rxFrameDone = false;

  uint8_t avail = Serial.available();

      if (!no_tx_status_changed && millis() - last_frame_timer > 1000) {
        no_tx_status_changed = true;
        Serial.println("No TX !");
        for (i = 0; i < SERVO_CHANNELS; i++)
          Servos[i] = 0;
      }

  if (avail) {
    uint8_t val = Serial.read();
    // Look for 0x2040 as start of packet
    if (ibusIndex == 0 && val != 0x20) {
      return;
    }
    if (ibusIndex == 1 && val != 0x40) {
      ibusIndex = 0;
      return;
    }
 
    if (ibusIndex < IBUS_BUFFSIZE) ibus[ibusIndex] = val;
    ibusIndex++;

    if (ibusIndex == IBUS_BUFFSIZE)
    {
      // ibusIndex = 0; moved to end
      chksum = 0xFFFF;
      for (i = 0; i < 30; i++)
        chksum -= ibus[i];

      rxsum = ibus[30] + (ibus[31] << 8);
      if (chksum == rxsum)
      {
        // MODIFY IF MORE CHANNELS NEEDED
        for (i = 1; i <= SERVO_CHANNELS; i++)
          Servos[i-1] = ((ibus[ i*2+1] << 8) + ibus[ i*2]) * 2; // get two bytes together and multiply by 2 in order to make compliant with LRS "standard" == PPM read
          #ifdef DEBUG
          Serial.print(millis() - last_frame_timer); Serial.println("ms");
          #endif       
        last_frame_timer = millis();
        no_tx_status_changed = false;
      } else {
        
        Serial.println("TX CRC!");
          
          /*
          // Now we need to disable interrupts to copy 16-bit values atomicly
          // Only copy needed signals (10 channels default)
          // MODIFY IF MORE CHANNELS NEEDED
          cli(); // disable interrupts.
          rcValueSafe[0] = rcValue[0];
          rcValueSafe[1] = rcValue[1];
          rcValueSafe[2] = rcValue[2];
          rcValueSafe[3] = rcValue[3];
          rcValueSafe[4] = rcValue[4];
          rcValueSafe[5] = rcValue[5];
          rcValueSafe[6] = rcValue[6];
          rcValueSafe[7] = rcValue[7];
          rcValueSafe[8] = rcValue[8];
          rcValueSafe[9] = rcValue[9];
          sei();
          digitalWrite(13, LOW); // OK packet - Clear error LED
          */
        }
        ibusIndex = 0;
      } else {
        // no buffer is built
        // TODO: do something if no data is comming to TX
      }
      return;
    }
}

#endif // IBUS_module
#endif // TX_module
