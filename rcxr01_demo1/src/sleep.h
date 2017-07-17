#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <avr/cpufunc.h>

/*
  contains various functions for sleep & wdt
  from: http://donalmorrissey.blogspot.com/2010/04/sleeping-arduino-part-5-wake-up-via.html

*/

// Watchdog Interrupt Service. This is executed when watchdog timed out.
EMPTY_INTERRUPT(WDT_vect);

void enable_prr() 
{ 
    power_spi_disable(); 
    power_timer0_disable(); 
    power_timer1_disable(); 
    power_twi_disable();
    power_adc_disable();
    power_usart1_disable();
    power_usart0_disable();
}

void disable_prr() 
{ 
    power_spi_enable(); 
    power_timer0_enable(); 
    power_adc_enable();
    power_usart0_enable();
}

// Enters the arduino into sleep mode.
void do_sleep(void)
{
  // There are five different sleep modes in order of power saving:
  // SLEEP_MODE_IDLE - the lowest power saving mode
  // SLEEP_MODE_ADC
  // SLEEP_MODE_PWR_SAVE
  // SLEEP_MODE_STANDBY
  // SLEEP_MODE_PWR_DOWN - the highest power saving mode
  set_sleep_mode(SLEEP_MODE_PWR_SAVE);

  enable_prr();
  sleep_bod_disable();
  // Now enter sleep mode.
  sleep_mode();

  // The program will continue from here after the WDT timeout
  // Re-enable the peripherals.
  disable_prr();
  //power_all_enable();
}


// Setup the Watch Dog Timer (WDT)
void setupWatchDogTimer() {
  // The MCU Status Register (MCUSR) is used to tell the cause of the last
  // reset, such as brown-out reset, watchdog reset, etc.
  // NOTE: for security reasons, there is a timed sequence for clearing the
  // WDE and changing the time-out configuration. If you don't use this
  // sequence properly, you'll get unexpected results.

  // Clear the reset flag on the MCUSR, the WDRF bit (bit 3).
  MCUSR &= ~(1<<WDRF);

  // Configure the Watchdog timer Control Register (WDTCSR)
  // The WDTCSR is used for configuring the time-out, mode of operation, etc

  // In order to change WDE or the pre-scaler, we need to set WDCE (This will
  // allow updates for 4 clock cycles).

  // Set the WDCE bit (bit 4) and the WDE bit (bit 3) of the WDTCSR. The WDCE
  // bit must be set in order to change WDE or the watchdog pre-scalers.
  // Setting the WDCE bit will allow updates to the pre-scalers and WDE for 4
  // clock cycles then it will be reset by hardware.
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  /**
   *  Setting the watchdog pre-scaler value with VCC = 5.0V
   *  WDP3 WDP2 WDP1 WDP0 | Number of WDT | Typical Time-out at Oscillator Cycles
   *  0    0    0    0    |   2K cycles   | 16 ms
   *  0    0    0    1    |   4K cycles   | 32 ms
   *  0    0    1    0    |   8K cycles   | 64 ms
   *  0    0    1    1    |  16K cycles   | 0.125 s
   *  0    1    0    0    |  32K cycles   | 0.25 s
   *  0    1    0    1    |  64K cycles   | 0.5 s
   *  0    1    1    0    |  128K cycles  | 1.0 s
   *  0    1    1    1    |  256K cycles  | 2.0 s
   *  1    0    0    0    |  512K cycles  | 4.0 s
   *  1    0    0    1    | 1024K cycles  | 8.0 s
  */
  // we are using 1MHz Internal RC Osc.
  
  WDTCSR = WDTO_120MS;
  // Enable the WD interrupt (note: no reset).
  WDTCSR |= _BV(WDIE);
}
