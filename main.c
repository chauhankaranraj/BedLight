#include <msp430.h> 

// Defines for trigger pin and pulse
#define TRIG_PORT_ P1
#define TRIG_PIN BIT2
#define TRIG_LOW_TIME 2500-1
#define TRIG_HIGH_TIME 100


// TODO: find the correct port and pin to be associated
// Defines for echo pin and pulse
#define ECHO_PORT_ P2
#define ECHO_PIN BIT2
#define ECHO_LOW_TIME 2500-1
#define ECHO_HIGH_TIME 100


/*************************************************************************
 * Set up output from timer on set/reset mode
 * Set every 2400us, reset in 100us after set
 *
 * PWM will stay low for TRIG_LOW_TIME, then go high for TRIG_HIGH_TIME,
 * and then low again (duty cycle = ~3.85%)
 *************************************************************************/
void set_up_trig_pulse(void)
{
    TRIG_PORT_DIR |= TRIG_PIN;          // P1.2 to output
    TRIG_PORT_SEL |= TRIG_PIN;          // P1.2 to TA0.1

    TA0CTL = TASSEL_2                   // timer source SMCLK
            + MC_1;                     // up mode to CCR0

    TA0CCTL0 = OUTMOD_3;                // set/reset mode
    TA0CCR0 = TRIG_LOW_TIME;            // will set output low after 100us of high
    TA0CCR1 = TRIG_HIGH_TIME;           // will set output high when 100 have passed since low
}


/*************************************************************************
 * Set up output from timer on set/reset mode
 * Set every 2400us, reset in 100us after set
 *
 * PWM will stay low for TRIG_LOW_TIME, then go high for TRIG_HIGH_TIME,
 * and then low again (duty cycle = ~3.85%)
 *************************************************************************/
void set_up_read_echo(void)
{

}


/*************************************************************************
 * main.c
 *************************************************************************/
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer


    BCSCTL1 = CALBC1_1MHZ;      // configure clocks for 1MHz frequency
    DCOCTL = CALDCO_1MHZ;

    set_up_trig_pulse();        // set up sending trigger pulse on ultrasonic sensor

    _BIS_SR(LPM0_bits);         // Enter LPM0
}

