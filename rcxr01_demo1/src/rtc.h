// rtc using timer2 and 32.768khz watch crystal
// from AVR134 app note

#include <avr/interrupt.h>

volatile uint32_t seconds = 0;

// rtc 1-sec tick
ISR(TIMER2_OVF_vect)
{
    seconds++;
}

void rtc_init()
{
    cli();
    //delay(1000);

    // clear T2 interrupt masks
    TIMSK2 = 0;

    // use ext crystal osc on TOSC1/2
    ASSR &= ~(_BV(EXCLK));
    ASSR |= _BV(AS2);

    // wait for busy flags to clear
    loop_until_bit_is_clear(ASSR, TCR2BUB);    
    //while (ASSR & 0b00011111);

    // set clock source prescaler to clk_T2S / 128 -> 1 sec overflow
    TCCR2B &= ~(_BV(CS21));
    TCCR2B |= _BV(CS22) | _BV(CS20);
    
    delay(1);
    // clear overflow interrupt flags
    TIFR2 |= _BV(TOV2);

    // enable Timer2 overflow interrupt
    TIMSK2 |= _BV(TOIE2);

    // enable global interrupts
    sei();
}

