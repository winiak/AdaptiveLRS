#ifdef TX_module

/**
 * Configure Timer to capture PPM values
 */
void Config_ICP1_PPM()        // Use ICP1 in input capture mode
{  

   // Setup timer1 for input capture (PSC=8 -> 0.5ms precision)
   TCCR1A = ((1 << WGM10) | (1 << WGM11));
   TCCR1B = ((1 << WGM12) | (1 << WGM13) | (1 << CS11) | (1 <<ICNC1));  //input canceler, 
   // normally capture on rising edge, allow invertting via SW flag
   TCCR1B |= (1 << ICES1);    //ICES1 for edge selection 
   OCR1A = 65535;
   TIMSK1 |= (1 << ICIE1);   // Enable timer1 input capture interrupt

}

#endif

