/// adapted from https://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/

uint16_t readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  ADMUX = 0b01011110; // REFS[1:0] = 01, MUX[4:0] = 11110
  delay(1); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both
  uint16_t result = (high<<8) | low;
  // reset ADMUX, to fix PA7 getting stolen by ADC7
  ADMUX = 0;
  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}

