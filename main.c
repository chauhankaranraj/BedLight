#include <msp430.h> 

// Defines for LED
#define LED_PORT P1
#define LED_PIN BIT5

// Defines for trigger pin and pulse
#define TRIG_PORT P1
#define TRIG_PIN BIT2
#define TRIG_LOW_TIME 25000-1
#define TRIG_HIGH_TIME 100

// Defines for echo pin and pulse
#define ECHO_PORT P2
#define ECHO_PIN BIT2
#define MAX_ECHO_HIGH_TIME 23500


/*************************************************************************
 * Set up output on led pin
 *************************************************************************/
void set_up_led_output(void)
{
    P1DIR |= LED_PIN;        // set as output pin
    P1OUT &= ~LED_PIN;       // set output low on pin
}


/*************************************************************************
 * Set up output from timer on set/reset mode
 * Set every 2400us, reset in 100us after set
 *
 * PWM will stay low for TRIG_LOW_TIME, then go high for TRIG_HIGH_TIME,
 * and then low again (duty cycle = ~0.385%)
 *************************************************************************/
void set_up_trig_pulse(void)
{
    P1DIR |= TRIG_PIN;          // P1.2 to output
    P1SEL |= TRIG_PIN;          // P1.2 to TA0.1

    TA0CTL = TASSEL_2                   // timer source SMCLK
            + MC_1;                     // up mode to CCR0

    TA0CCTL0 = OUTMOD_3;                // set/reset mode
    TA0CCR0 = TRIG_LOW_TIME;            // will set output low after 100us of high
    TA0CCR1 = TRIG_HIGH_TIME;           // will set output high when 100 have passed since low
}


/*************************************************************************
 * Set up an input pin to read echo pin of ultrasonic sensor
 *************************************************************************/
void set_up_read_echo(void)
{
    // set up echo pin to be input pin with pull down resistor
    P2SEL &= ~ECHO_PIN;     // i/o function selected, 0
    P2DIR &= ~ECHO_PIN;     // input mode (p3.5), 0
    P2OUT &= ~ECHO_PIN;     // select pull-down mode, 0
    P2REN |= ECHO_PIN;      // enable internal pull up, 1

    // set up interrupt on echo pin reading low to high
    P2IE |= ECHO_PIN;       // enable interrupt for echo pin
    P2IES &= ~ECHO_PIN;     // low to high transition
    P2IFG &= ~ ECHO_PIN;    // clear the interrupt flag
}


/*************************************************************************
 * ECHO port ISR to read echo pulse
 *************************************************************************/
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    //P2IE &= ~ECHO_PIN;      // disable interrupt don't interrupt if already inside interrupt

    // count how long the echo input pin stays high
    unsigned long long highTimeCount = 0;
    while ((P2IN & ECHO_PIN) != 0)
    {
        highTimeCount++;
    }

    // if it stayed high for anything less than the max value, turn on LED. Otherwise, turn it off
    if (highTimeCount < MAX_ECHO_HIGH_TIME)
    {
        P1OUT |= LED_PIN;
    }
    else
    {
        P1OUT &= ~LED_PIN;
    }
}


/*************************************************************************
 * main.c
 *************************************************************************/
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    BCSCTL1 = CALBC1_1MHZ;      // configure clocks for 1MHz frequency
    DCOCTL = CALDCO_1MHZ;

    set_up_led_output();        // set up output on LED pin
    set_up_trig_pulse();        // set up sending trigger pulse on ultrasonic sensor
    set_up_read_echo();         // set up to read echo from ultrasonic sensor

    _BIS_SR(LPM0_bits);         // Enter LPM0
}
