// rtc using timer2 and 32.768khz watch crystal
// ref: AVR130 & AVR134 app notes

#include <avr/interrupt.h>

volatile uint32_t seconds = 0;

// rtc 1-sec tick
ISR(TIMER2_OVF_vect)
{
    seconds++;
}

void rtc_init()
{
    // clear T2 interrupt masks
    TIMSK2 = 0;

    // use ext crystal osc on TOSC1/2
    ASSR |= _BV(AS2);

    TCNT2 = 0;

    // required because somebody else is mucking with this register (probably arduino libs)
    TCCR2A = 0;

    // set clock source prescaler to clk_T2S / 128 -> 1 sec overflow
    TCCR2B = _BV(CS22) | _BV(CS20);
    loop_until_bit_is_clear(ASSR, TCR2BUB);

    //reset prescalers    
    GTCCR |= _BV(PSRASY); // not sure if necessary
    
    // clear interrupt flags
    TIFR2 = 0;

    // enable Timer2 overflow interrupt
    TIMSK2 |= _BV(TOIE2);
}

