/*****************************************************************************************************
***         JLRS - Long Range RC & Data Link by Jacek Szostak                                      ***
***                                                                                                ***
***           Contains code from OpenLRS and Ultimate LRS                                          ***
***                         Optymized for MavLink and FrSky Telemetry                              ***
***                                                                                                ***
******************************************************************************************************
*/

#define NOP() __asm__ __volatile__("nop") 

#define RF22B_PWRSTATE_STANDBY                0b00000000
#define RF22B_PWRSTATE_READY                  0b00000001
#define RF22B_PWRSTATE_TX                     0b00001001  //0x09
#define RF22B_PWRSTATE_RX                     0b00000101  //0x05

#define RF22B_Rx_packet_received_interrupt    0b00000010
#define RF22B_PACKET_SENT_INTERRUPT           0b00000100

unsigned char ItStatus1, ItStatus2;                 //rejestry SI4432 do odczytu 

boolean SI4432_checkRX() {
  //If it occurs, then it means a packet received or CRC error happened or sync word occured
  if(nIRQ_LOW)
  {
    //read interrupt status registers
    ItStatus1 = SI4432_spi_read(0x03);            //read the Interrupt Status1 register
    ItStatus2 = SI4432_spi_read(0x04);            //read the Interrupt Status2 register

    //Sync Word interrupt occured
    if( (ItStatus2 & 0b10000000) == 0b10000000 )
    {
      RX_RSSI = SI4432_spi_read(0x26);
      Serial.println(RX_RSSI);         
    }
  }
  //CRC Error interrupt occured
  if( (ItStatus1 & 0x01) == 0x01 )
    { //reset the RX FIFO
        //disable the receiver chain
        SI4432_spi_write(0x07, 0x01);                 //write 0x01 to the Operating Function Control 1 register
        
        SI4432_spi_write(0x08, 0x02);           //write 0x02 to the Operating Function Control 2 register
        SI4432_spi_write(0x08, 0x00);           //write 0x00 to the Operating Function Control 2 register
        Serial.println(F("CRCerr"));
        SI4432_to_RX_mode();
        return false;
    }
    else 
    {
      //packet received interrupt occured  
      if( (ItStatus1 & 0x02) == 0x02 )
      {
        //disable the receiver chain
        SI4432_spi_write(0x07, 0x01);                 //write 0x01 to the Operating Function Control 1 register
        
        SI4432_receive_data();

        SI4432_to_RX_mode();
        return true;  
      }
      return false;
    }
            
}

/**
 * Receive data (first check if available)
 */
void SI4432_receive_data()
{

    //Read the length of the received payload
    uint8_t message_length = SI4432_spi_read(0x4B);       //read the Received Packet Length register
    //??? check whether the received payload is not longer than the allocated buffer in the MCU

    //Serial.print("RX RML: "); Serial.print(message_length); Serial.print(" .. ");
    //Serial.println(TX_RSSI);
    
    nSEL_LOW;

        Write8bitcommand(0x7f);                 //komenda czytamy ...
        
        //====================================
        if (message_length == 16)                           //czyli info o serwach
          {
            Serial.println("S");
            for (char i = 0; i < 16; i++)
              {
                RF_Servo_message[i] = read_8bit_data();
              }   
        
            for (char i = 0; i < 8; i++)                       //RC_CHANNEL_COUNT; i++)
              {
                unsigned int temp_int = (256 * RF_Servo_message[(2 * i)]) + RF_Servo_message[1 + (2 * i)];
      
                //if ((temp_int > 1500) && (temp_int < 5000))
                //    {
                        Servos[i] = temp_int ;
                        //Serial.print(F("/")); Serial.print(temp_int); 
                //    }
              }
            //Serial.println();
            
          }

        //====================================
        if (message_length == 18)                           //czyli info Telemetry_bridge
          {
            Serial.println("BbbbbbbbbbbbbbbB");         //PODCZAS TESTOWANIA!!!
            for (char i = 0; i < 18; i++)
              {
                RF_Bridge_message[i] = read_8bit_data();
              }   
        
          }


    nSEL_HIGH;
}

/**
 * Transmit data
 */
void SI4432_TX(unsigned char message_length)
{
    #if(status_pin > 0)
      status_pin_HIGH; 
    #endif
    
    //disable the receiver chain (but keep the XTAL running to have shorter TX on time!)
    SI4432_spi_write(0x07, 0x01);

    //The Tx deviation register has to set according to the deviation before every transmission 
    SI4432_spi_write(0x72, 0x2E);
     
    /*SET THE CONTENT OF THE PACKET*/
    SI4432_spi_write(0x3e, message_length);           //message length

    for (byte i = 0; i < message_length; i++)
      {
        SI4432_spi_write(0x7f, TX_Buffer[i]);
      }
    
    //Disable all other interrupts and enable the packet sent interrupt only.
    //This will be used for indicating the successfull packet transmission for the MCU
    SI4432_spi_write(0x05, 0x04); //write 0x04 to the Interrupt Enable 1 register
    SI4432_spi_write(0x06, 0x00); //write 0x03 to the Interrupt Enable 2 register

    //Read interrupt status regsiters. It clear all pending interrupts and the nIRQ pin goes back to high.
    ItStatus1 = SI4432_spi_read(0x03);//read the Interrupt Status1 register
    ItStatus2 = SI4432_spi_read(0x04);//read the Interrupt Status2 register
    
    /*enable transmitter*/
    SI4432_spi_write(0x07, 0x09);//write 0x09 to the Operating Function Control 1 register
    
    /*wait for the packet sent interrupt*/
    //The MCU just needs to wait for the 'ipksent' interrupt.
    while(nIRQ_HIGH);

    //read interrupt status registers to release the interrupt flags
    ItStatus1 = SI4432_spi_read(0x03); //read the Interrupt Status1 register
    ItStatus2 = SI4432_spi_read(0x04); //read the Interrupt Status2 register

    //Serial.print("\t"); Serial.println(ItStatus1,BIN);
    
    SI4432_to_RX_mode();

    #if(status_pin > 0)
      status_pin_LOW;
      blink_TX_done;
    #endif
}

void set_data_rate_high()
{
   //57600
    Serial.print("Data rate high\n");
    SI4432_spi_write(0x1c, 0x05);     //IF Filter Bandwidth
    SI4432_spi_write(0x1d, 0x40);     //AFC Loop Gearshift Override, 
    SI4432_spi_write(0x1e, 0x0A);     //Si4432_AFC_TIMING_CONTROL
    SI4432_spi_write(0x20, 0x45);     //Si4432_CLOCK_RECOVERY_OVERSAMPLING_RATIO
    SI4432_spi_write(0x21, 0x01);     //Si4432_CLOCK_RECOVERY_OFFSET_2
    SI4432_spi_write(0x22, 0xD7);     //Si4432_CLOCK_RECOVERY_OFFSET_1
    SI4432_spi_write(0x23, 0xDC);     //Si4432_CLOCK_RECOVERY_OFFSET_0                 
    SI4432_spi_write(0x24, 0x03);     //Si4432_CLOCK_RECOVERY_TIMING_LOOP_GAIN_1
    SI4432_spi_write(0x25, 0xB8);     //Si4432_CLOCK_RECOVERY_TIMING_LOOP_GAIN_0
    SI4432_spi_write(0x2a, 0x1E);     //Si4432_AFC_LIMIT 
    SI4432_spi_write(0x6e, 0x0E);     //TX Data Rate 1
    SI4432_spi_write(0x6f, 0xBF);     //TX Data Rate 0
    SI4432_spi_write(0x70, 0x00);     //Modulation Mode Control 1, 
    SI4432_spi_write(0x71, 0x23);     //Modulation Mode Control 2, FIFO Mode, GFSK
    SI4432_spi_write(0x72, 0x2E);     //frequency deviation setting to 28.8khz(for 57.6kbps)
    //0x1F dla RX (19375)   ===> 0x48 dla TX (45000)

    //dla 45k  dev..... RX 0x35 (33125)   TX  0x48 (45000)
    //dla 28,8dev     RX  0x28 (25000)   TX 0x2E (28750)   
    #ifdef DEBUG
    #endif
}

void set_data_rate_low()
{
 /*
 * OPENLRSng...
 *  uint8_t  r_1c, r_1d, r_1e, r_20, r_21, r_22, r_23, r_24, r_25, r_2a, r_6e, r_6f, r_70, r_71, r_72;
  { 4800, 0x1a, 0x40, 0x0a, 0xa1, 0x20, 0x4e, 0xa5, 0x00, 0x1b, 0x1e, 0x27, 0x52, 0x2c, 0x23, 0x30 }, 
  { 9600, 0x05, 0x40, 0x0a, 0xa1, 0x20, 0x4e, 0xa5, 0x00, 0x20, 0x24, 0x4e, 0xa5, 0x2c, 0x23, 0x30 }, 
  { 19200, 0x06, 0x40, 0x0a, 0xd0, 0x00, 0x9d, 0x49, 0x00, 0x7b, 0x28, 0x9d, 0x49, 0x2c, 0x23, 0x30 },
  { 57600, 0x05, 0x40, 0x0a, 0x45, 0x01, 0xd7, 0xdc, 0x03, 0xb8, 0x1e, 0x0e, 0xbf, 0x00, 0x23, 0x2e },
  { 125000, 0x8a, 0x40, 0x0a, 0x60, 0x01, 0x55, 0x55, 0x02, 0xad, 0x1e, 0x20, 0x00, 0x00, 0x23, 0xc8 },
 */
    Serial.print("Data rate low\n");
 // { 19200, 0x06, 0x40, 0x0a, 0xd0, 0x00, 0x9d, 0x49, 0x00, 0x7b, 0x28, 0x9d, 0x49, 0x2c, 0x23, 0x30 },
    SI4432_spi_write(0x1c, 0x06);     //IF Filter Bandwidth
    SI4432_spi_write(0x1d, 0x40);     //AFC Loop Gearshift Override, 
    SI4432_spi_write(0x1e, 0x0A);     //Si4432_AFC_TIMING_CONTROL
    SI4432_spi_write(0x20, 0xd0);     //Si4432_CLOCK_RECOVERY_OVERSAMPLING_RATIO
    SI4432_spi_write(0x21, 0x00);     //Si4432_CLOCK_RECOVERY_OFFSET_2
    SI4432_spi_write(0x22, 0x9d);     //Si4432_CLOCK_RECOVERY_OFFSET_1
    SI4432_spi_write(0x23, 0x49);     //Si4432_CLOCK_RECOVERY_OFFSET_0                 
    SI4432_spi_write(0x24, 0x00);     //Si4432_CLOCK_RECOVERY_TIMING_LOOP_GAIN_1
    SI4432_spi_write(0x25, 0x7b);     //Si4432_CLOCK_RECOVERY_TIMING_LOOP_GAIN_0
    SI4432_spi_write(0x2a, 0x28);     //Si4432_AFC_LIMIT 
    SI4432_spi_write(0x6e, 0x9d);     //TX Data Rate 1
    SI4432_spi_write(0x6f, 0x49);     //TX Data Rate 0
    SI4432_spi_write(0x70, 0x2c);     //Modulation Mode Control 1, 
    SI4432_spi_write(0x71, 0x23);     //Modulation Mode Control 2, FIFO Mode, GFSK
    SI4432_spi_write(0x72, 0x2E);     //frequency deviation setting to 28.8khz    
}

/**
 * SI44322 initialization
 */
void radio_init(void)
{
    //SW reset
    SI4432_spi_write(0x07, 0x80);                   //write 0x80 to the Operating & Function Control1 register

    //wait for chip ready interrupt from the radio (while the nIRQ pin is high)
    while (nIRQ_HIGH);
    
    //read interrupt status registers to clear the interrupt flags and release NIRQ pin
    ItStatus1 = SI4432_spi_read(0x03);              //read the Interrupt Status1 register
    ItStatus2 = SI4432_spi_read(0x04);              //read the Interrupt Status2 register
    
    SI4432_spi_write(0x06, 0x00);
    SI4432_spi_write(0x07, RF22B_PWRSTATE_READY);

    SI4432_spi_write(0x09, 0x7f);                   //30 MHz Crystal Oscillator Load Capacitance
    SI4432_spi_write(0x73, 0x00);                   //Frequency Offset 1
    SI4432_spi_write(0x74, 0x00);                   //Frequency Offset 2

      /*set the physical parameters*/
    Set_frequency_center(base_frequency);           //set the center frequency

    SI4432_spi_write(0x7a, 12);                     //HOPPING_STEP_SIZE in 10KHz
    SI4432_spi_write(0x79, channel);                //Frequency Hopping Channel Select

    SI4432_spi_write(0x0a, 0x05);                   //Microcontroller Output Clock
    SI4432_spi_write(0x0b, 0x12);                   //GPIO Configuration 0, TX State (output)
    SI4432_spi_write(0x0c, 0x15);                   //GPIO Configuration 1, RX State (output)
    SI4432_spi_write(0x0d, 0xff);                   //GPIO Configuration 2, 
    SI4432_spi_write(0x0e, 0x00);                   //I/O Port Configuration  

    set_data_rate_high();

    SI4432_spi_write(0x30, 0x8c);                   // enable packet handler, msb first, enable crc,
    SI4432_spi_write(0x32, 0x0f);                   // no broadcast, check header bytes 3,2,1,0             
    SI4432_spi_write(0x33, 0x42);                   // 4 byte header, 2 byte synch, variable pkt size
    SI4432_spi_write(0x34, 0x0A);                   // 40 bit preamble,
    SI4432_spi_write(0x35, 0x2A);                   //20 bits Preamble Detection Threshold          // Preamble Detection Control 1

    //nadawany HEADER
    SI4432_spi_write(0x3a, RF_Header[0]);
    SI4432_spi_write(0x3b, RF_Header[1]);
    SI4432_spi_write(0x3c, RF_Header[2]);
    SI4432_spi_write(0x3d, RF_Header[3]);
    SI4432_spi_write(0x3e, 5);                     //RF_PACK_SIZE, ale i tak jest ustawiany przy kazdym nadawaniu
  
    //HEADER odbiornika - bedzie porownywany z tym co przyjdzie
    SI4432_spi_write(0x3f, RF_Header[0]);
    SI4432_spi_write(0x40, RF_Header[1]);
    SI4432_spi_write(0x41, RF_Header[2]);
    SI4432_spi_write(0x42, RF_Header[3]);         


    //set the non-default Si4432 registers
    //set the VCO and PLL
    SI4432_spi_write(0x5A, 0x7F); //write 0x7F to the VCO Current Trimming register
    SI4432_spi_write(0x58, 0x80); //write 0x80 to the ChargepumpCurrentTrimmingOverride register
    SI4432_spi_write(0x59, 0x40); //write 0x40 to the Divider Current Trimming register
    //set the AGC
    SI4432_spi_write(0x6A, 0x0B); //write 0x0B to the AGC Override 2 register
    //set ADC reference voltage to 0.9V
    SI4432_spi_write(0x68, 0x04); //write 0x04 to the Deltasigma ADC Tuning 2 register
    SI4432_spi_write(0x1F, 0x03); //write 0x03 to the Clock Recovery Gearshift Override register
    SI4432_spi_write(0x6d, power); 

    #ifdef DEBUG
    Serial.print("RF_Version_code: "); Serial.println(SI4432_spi_read(0x01),BIN);
    #endif
  
}


//=======================================

void Hopping(void)
{
  static unsigned char hopping_channel = 0;

  //LED_feadback_8_HIGH;
  
  hopping_channel++;

  if (hopping_channel == sizeof(hop_list))   hopping_channel = 0;

  SI4432_spi_write(0x79, hop_list[hopping_channel]);

  //LED_feadback_8_LOW;
}

//=======================================

void SI4432_to_RX_mode(void)
{

    //Frequency Deviation register for RX
    SI4432_spi_write(0x72, 0x28);
    
    /*enable receiver chain*/
    SI4432_spi_write(0x07, 0b00000101);       //write 0x05 to the Operating Function Control 1 register
    
    //Enable 3 interrupts:
    // a) one which shows that a valid packet received: 'ipkval'
    // b) second shows if the packet received with incorrect CRC: 'icrcerror'
    // 3) Enable Sync Word Detected. When set to 1 the Syn Word Detected Interrupt will be enabled.
    SI4432_spi_write(0x05, 0x03);             //write 0x03 to the Interrupt Enable 1 register
    SI4432_spi_write(0x06, 0b10000000);             //interrupt Enable 2 register

    //read interrupt status registers to release all pending interrupts
    ItStatus1 = SI4432_spi_read(0x03);        //read the Interrupt Status1 register
    ItStatus2 = SI4432_spi_read(0x04);        //read the Interrupt Status2 register
    
}


//=======================================

void to_ready_mode(void)
{
  ItStatus1 = SI4432_spi_read(0x03);
  ItStatus2 = SI4432_spi_read(0x04);
  SI4432_spi_write(0x07, RF22B_PWRSTATE_READY);
    //clear RX&TX FIFO
    SI4432_spi_write(0x08, 0x03);
    SI4432_spi_write(0x08, 0x00);    
}

//=======================================

void to_Standby_mode(void)
{


      SI4432_spi_write(0x08, 0x03);
    SI4432_spi_write(0x08, 0x00);  
  SI4432_spi_write(0x07, 0b00000000);

  
}

//=======================================

void Set_frequency_center(uint32_t frequency)
{
  
  uint16_t fb, fc, hbsel;
  if (frequency < 480000000) {
    hbsel = 0;
    fb = frequency / 10000000 - 24;
    fc = (frequency - (fb + 24) * 10000000) * 4 / 625;
  } else {
    hbsel = 1;
    fb = frequency / 20000000 - 24;
    fc = (frequency - (fb + 24) * 20000000) * 2 / 625;
  }
  SI4432_spi_write(0x75, 0x40 + (hbsel ? 0x20 : 0) + (fb & 0x1f));
  SI4432_spi_write(0x76, (fc >> 8));
  SI4432_spi_write(0x77, (fc & 0xff));

}



//=======================================

void Write0(void)
{
  SCK_off;
  NOP();
  SDI_off;
  NOP();
  SCK_on;
  NOP();
}


//=======================================
void Write1(void)
{
  SCK_off;
  NOP();
  SDI_on;
  NOP();
  SCK_on;
  NOP();
}

//=======================================
void Write8bitcommand(unsigned char command)
{
  unsigned char n = 8;
  nSEL_HIGH;
        NOP();
  SCK_off;
        NOP();
  nSEL_LOW;
  while (n--)
  {
    if (command & 0x80)
    {
      Write1();
    }

    else
    {
      Write0();
    }

    command = command << 1;
  }

  SCK_off;
        NOP();
}


//=======================================
unsigned char SI4432_spi_read(unsigned char address)
{
  unsigned char result;
  send_read_address(address);
  result = read_8bit_data();
        NOP();
  nSEL_HIGH;
  return(result);
}


//=======================================
void SI4432_spi_write(unsigned char address, unsigned char data)
{
  address |= 0x80;
  Write8bitcommand(address);
  send_8bit_data(data);
  nSEL_HIGH;
}

//=======================================

void send_read_address(unsigned char i)
{
  i &= 0x7f;
  Write8bitcommand(i);
}

//=======================================

void send_8bit_data(unsigned char i)
{
  unsigned char n = 8;
  SCK_off;

  while (n--)
  {
    if (i & 0x80)
    {
      Write1();
    }

    else
    {
      Write0();
    }

    i = i << 1;
  }

  SCK_off;
}

//=======================================

unsigned char read_8bit_data(void)
{
  unsigned char Result, i;
  SCK_off;
  Result = 0;

  for (i = 0; i < 8; i++)
  {
    //read fifo data byte
    Result = Result << 1;
    SCK_on;
    NOP();

    if (SDO_HIGH)
    {
      Result |= 1;
    }

    SCK_off;
    NOP();
  }

  return(Result);
}


