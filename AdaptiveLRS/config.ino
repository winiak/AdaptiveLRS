



/**
 * 1) Select type TX / RX
 * 2) Select HW configuration
 * 3) Select communication parameters
 * 4) Select PIN mapping
 * 
 * HW configuration "1"
 * SI4432 <-> Arduino
 * SDO    <-> A0
 * SDI    <-> A1
 * SCK    <-> A2    RFM22 <-> RFM22
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

// Transmitter or Receiver
#define TX_module 1
 //#define RX_module 1

// HW configuration
#define HW_config 1

// Output type (select one)
#define Output_PPM
//#define Output_ibus
 
// Communication parameters
#define channel         1
#define power           3
#define base_frequency  867000000
#define RF_Header       {'A', 'A', 'A', 'J'}
#define hop_list        {5,7,12}
//{10,9,8}; //{1,142,3,140,7,138,11,136,67,111}; 


 
