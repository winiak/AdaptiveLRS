/*
 * This is TODO file. List of further tasks / features to be implemented:
 * - reading ppm signal, check with "output_ppm" and with any receiver
 * - time display function - send to serial time from last call of this function
 * - setup basic communication between modules - set RX to respond with Echo
 * + read iBus frame from receiver
 *    - addreaction on lack of incomming signal (nothing on serial or checksum)
 * + generating ibus frame, 
 *    + check if frame is correct (debug)
 *    + check with FC
 * - build communication frame (TYPE, LENGTH, DATA)
 * - send non-servo data when available (on request) but not more often that every second servo frame
 * - hopping!
 * - failsafe (servo values out of range / no PPM / ibus stopped)
 * - change TX power on request - test
 * - change TX power based on RSSI
 * - change communication speed on request - test
 * - change (by TX) communication speed based on RSSI, look for slow speed by RX when no connection and hopping fails
 * - connect two SI4432 modules to RX, 
 *    - set up both for receiveing
 *    - look for nIRQ on both
 *    - read RSSI
 *    - use data from the one with better RSSI
 *    - transmit frame from the one with better RSSI
 *    ALTERNATIVE
 *    - use Hopping to switch between RX modules (every second channel)
 * - generating sbus frame, check with FC
 * - generating fport frame, check with FC
 * - read sbus frame
 * - read fport frame
*/
