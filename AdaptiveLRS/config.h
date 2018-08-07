



/**
 * 1) Select type TX / RX
 * 2) Select HW configuration
 * 3) Select communication parameters
 * 4) Select PIN mapping
 * 
 * HW configuration " 1"Type 3 RX
 * SI4432 <-> Arduino
 * SDO    <-> A0
 * SDI    <-> A1
 * SCK    <-> A2    RFM22 <-> RFM22
 * SDN    <-> A3    ** optional
 * nSEL   <-> D4    GPIO0 <-> TX_ANT
 * nIRQ   <-> D2    GPIO1 <-> RX_ANT 
 * 
 * Transmitting power
 * 7 == 100mW (or 1000mW with M3)
 * 6 == 50mW (use this when using booster amp), (800mW with M3)
 * 5 == 25mW
 * 4 == 13mW
 * 3 == 6mW
 * 2 == 3mW
 * 1 == 1.6mW
 * 0 == 1.3mW
 */

<<<<<<< HEAD
// Debug mode
// Debug level: 0-none; 1-status only; 2-radio debug; 3-servo channels debug;

#define DEBUG 3
=======
// Debug mode / Level=0 -> none, Level=1->only states, Level=2->RSSI, Level=3->Channel Data
#define DEBUG 0
>>>>>>> refs/remotes/origin/rozjazd-z-pracy
#define servo_tester_module

// Transmitter or Receiver
#define TX_module
//#define RX_module
<<<<<<< HEAD
//#define BAND_scan
=======
//#define Band_scanner
>>>>>>> refs/remotes/origin/rozjazd-z-pracy

// Communication type
#define PPM_module  // using ICP for TX or declared for TX
//#define IBUS_module   // using UART
//#define FRSKY_module

// HW configuration
#define radio_SI4432
#define HW_config 1

// Output type (select one)
//#define Output_PPM
//#define Output_ibus
 
// Communication parameters
#define channel         1
#define power           3
#define base_frequency  867000000
const char RF_Header[4] = {'A', 'K', 'A', 'W'};
const uint8_t hop_list[] = {5,7,12};
<<<<<<< HEAD
static unsigned char hopping_channel = 0;
//uint8_t hopping_channel = 0;
=======
>>>>>>> refs/remotes/origin/rozjazd-z-pracy
//{10,9,8}; //{1,142,3,140,7,138,11,136,67,111}; 

// Servos & channels
#define SERVO_CHANNELS 8
volatile unsigned int Servo_Buffer[SERVO_CHANNELS] = {3000, 3000, 2000, 3000, 3000, 3000, 3000, 3000};
unsigned int Servos[SERVO_CHANNELS] = {3000, 3000, 2000, 3000, 3000, 3000, 3000, 3000};  
unsigned int Servo_Failsafe[SERVO_CHANNELS] = {3000, 3000, 1800, 3000, 3000, 3000, 3000, 3000};

  #define SERVO_SHIFT 0   //(-16)   // PPM = -16  |  other receivers  = 0

// PPM 
  #define PPM_OUT 8   
  #define PPM_OUT_HIGH PORTB |= _BV(0)
  #define PPM_OUT_LOW PORTB &= ~_BV(0)
  #define PPM_IN  8 // ICP1

// PWM
//#define Servo1_OUT pinMode(_pin_, OUTPUT);
//#define Servo1_OUT_HIGH PORT_ |= _BV(?)
//#define Servos_LOW PORTB &= 0x00; PORTD &= 0x1F; // pulling down the servo outputs

// IBUS
#define IBUS_TIMEOUT  100

#if(HW_config == 1)
  #define SDO_pin           A0
  #define SDI_pin           A1
  #define SCLK_pin          A2
  #define SDN_pin           A3
  #define IRQ_pin           2
  #define nSel_pin          4
  #define  nIRQ_HIGH    (PIND & 0x04)==0x04 //D2
  #define  nIRQ_LOW     (PIND & 0x04)==0x00 //D2
  #define  nSEL_HIGH    PORTD |= (1<<4) //D4
  #define  nSEL_LOW     PORTD &= 0xEF //D4
  #define  SCK_on     PORTC |= (1<<2) //A2
  #define  SCK_off    PORTC &= 0xFB //A2
  #define  SDI_on     PORTC |= (1<<1) //A1
  #define  SDI_off    PORTC &= 0xFD //A1
  #define  SDO_HIGH      (PINC & 0x01) == 0x01 //A0
  #define  SDO_LOW       (PINC & 0x01) == 0x00 //A0
  #define LED_feadback              13  
  #define LED_feadback_8_HIGH       PORTB |= _BV(5);
  #define LED_feadback_8_LOW        PORTB &= ~_BV(5)
 #endif
