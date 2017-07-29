#include <msp430.h> 


/*********************************************************************
 * Set up output from timer on set/reset mode
 * Set every 2400us, reset in 100us after set
 *********************************************************************/
void set_up_trig_pulse(void)
{
    P2DIR = 0;              // set port 2 to inputs

    P1DIR |= BIT2;          // P1.2 to output
    P1SEL |= BIT2;          // P1.2 to TA0.1

    TA0CTL = TASSEL_2       // timer source SMCLK
            + MC_1;         // up mode to CCR0

    TA0CCTL0 = OUTMOD_3;    // set/reset mode
    TA0CCR0 = 2500-1;       // will set output low after 100us of high
    TA0CCR1 = 100;          // will set output high when 100 have passed since low
}


/*********************************************************************
 * main.c
 *********************************************************************/
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer


    BCSCTL1 = CALBC1_1MHZ;      // configure clocks for 1MHz frequency
    DCOCTL = CALDCO_1MHZ;

    set_up_trig_pulse();        // set up sending trigger pulse on ultrasonic sensor

    _BIS_SR(LPM0_bits);         // Enter LPM0
}

