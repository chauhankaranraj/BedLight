#include <msp430.h> 

#define READ_ECHO_PIN BIT5

#define TRIGGER_PULSE_PIN BIT1
#define TRIGGER_PULSE_LOW_TIME 25000
#define TRIGGER_PULSE_TOTAL_TIME 25100

#define LED_OUTPUT_PIN BIT3

unsigned long long counter = 0;


void init_read_echo(void)
{
    P1DIR &= ~READ_ECHO_PIN;     // set as input pin
    P1OUT &= ~READ_ECHO_PIN;     // set resistor pull down
    P1REN |= READ_ECHO_PIN;      // internal pull resistor enable

    P1IE |= READ_ECHO_PIN;       // enable interrupt
    P1IES &= ~READ_ECHO_PIN;     // interrupt on edge low to high transition
    P1IFG &= ~READ_ECHO_PIN;     // clear interrupt flag
}


void init_trigger_pulse(void)
{
    P1DIR |= TRIGGER_PULSE_PIN;                      // P1.1 to output
    P1SEL |= TRIGGER_PULSE_PIN;                      // P1.1 to TA0.1

    TA0CTL = TASSEL_2                   // timer source SMCLK
           + MC_1;                      // up mode to CCR0

    TA0CCTL0 = OUTMOD_3;                // set/reset mode
    TA0CCR0 = TRIGGER_PULSE_LOW_TIME;                    // will set output high after 25000us of low
    TA0CCR1 = TRIGGER_PULSE_TOTAL_TIME;                    // will set output low after 100us of high
}


#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    if (P1IFG & READ_ECHO_PIN)
    {
        while(P1IN & READ_ECHO_PIN)
        {
            counter += 1;
        }
        if (counter > 500)
        {
            P1OUT &= ~LED_OUTPUT_PIN;
        }
        else
        {
            P1OUT |= LED_OUTPUT_PIN;
        }
        counter = 0;
        P1IFG &= ~READ_ECHO_PIN;
    }
}


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
    BCSCTL1 = CALBC1_1MHZ;      // configure clocks for 1MHz frequency
    DCOCTL = CALDCO_1MHZ;

    // led output pin
    P1DIR |= LED_OUTPUT_PIN;
    P1OUT &= ~LED_OUTPUT_PIN;

    init_read_echo();           // set up to read from echo pin

    init_trigger_pulse();       // start sending pulse on trigger pin

    _BIS_SR(LPM0_bits + GIE);           // Enter LPM0 with global interrupts enabled
}
