#include "seeed_pwm.h"

void seeed_pwm::init()
{
    TCCR0A = 0;                                 // clear control register A
    TCCR0B = _BV(WGM13);                        // set mode 8: phase and frequency correct pwm, stop the timer
}

void seeed_pwm::setFreq(long freq)                // AR modified for atomic access
{

    long cycles = (F_CPU / 2000000) * (1000000/freq);                               // the counter runs backwards after TOP, interrupt is at BOTTOM so divide microseconds by 2
    if(cycles < TIMER1COUNT)              clockSelectBits = _BV(CS10);              // no prescale, full xtal
    else if((cycles >>= 3) < TIMER1COUNT) clockSelectBits = _BV(CS11);              // prescale by /8
    else if((cycles >>= 3) < TIMER1COUNT) clockSelectBits = _BV(CS11) | _BV(CS10);  // prescale by /64
    else if((cycles >>= 2) < TIMER1COUNT) clockSelectBits = _BV(CS12);              // prescale by /256
    else if((cycles >>= 2) < TIMER1COUNT) clockSelectBits = _BV(CS12) | _BV(CS10);  // prescale by /1024
    else        cycles = TIMER1COUNT - 1, clockSelectBits = _BV(CS12) | _BV(CS10);  // request was out of bounds, set as maximum

    char tmp = SREG;
    cli();                                                                          // Disable interrupts for 16 bit register access
    ICR1 = pwmPeriod = cycles;                                                      // ICR1 is TOP in p & f correct setPwm mode
    SREG = tmp;
    TCCR0B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
    TCCR0B |= clockSelectBits;                                                      // reset clock select register, and starts the clock
}

// duty, 0-100
void seeed_pwm::setPwmDuty(char pin, int duty)
{
    if(duty >100)duty = 100;
    if(duty < 0) duty = 0;
    unsigned long dutyCycle = pwmPeriod*10;
    dutyCycle *= duty;
    dutyCycle >>= 10;
    char tmp = SREG;
    cli();
    if(pin == 1 || pin == 5)       OCR0B = dutyCycle;
    else if(pin == 2 || pin == 6)  OCR0A = dutyCycle;
    SREG = tmp;
}

// setPwm, pin: 5, 6
void seeed_pwm::setPwm(char pin, int duty, long freq)                 // expects duty cycle to be 10 bit (1024)
{
    if((pin != 5 && pin != 6) || freq <= 0)return ;                // error parament

    setFreq(freq);
    
    if(pin == 5) {
        DDRD |= _BV(PORTD5);                                        // sets data direction register for setPwm output pin
        TCCR0A |= _BV(COM0A1);                                      // activates the output pin
    }
    else
    {
        DDRD |= _BV(PORTD6);
        TCCR0A |= _BV(COM0B1);
    }
    setPwmDuty(pin, duty);

}

void seeed_pwm::disablePwm(char pin)
{
    if(pin == 1 || pin == 5)       TCCR0A &= ~_BV(COM0A1);          // clear the bit that enables setPwm on PB1
    else if(pin == 2 || pin == 6) TCCR0A &= ~_BV(COM0B1);          // clear the bit that enables setPwm on PB2
}

seeed_pwm PWM;                                                        // preinstatiate

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/