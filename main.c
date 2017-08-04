#include <msp430.h> 

// Defines for LED
#define LED_PORT_ P1
#define LED_PIN BIT5

// Defines for trigger pin and pulse
#define TRIG_PORT_ P1
#define TRIG_PIN BIT2
#define TRIG_LOW_TIME 25000-1
#define TRIG_HIGH_TIME 100

// Defines for echo pin and pulse
#define ECHO_PORT_ P2
#define ECHO_PIN BIT2
#define MAX_ECHO_HIGH_TIME 23500


/*************************************************************************
 * Set up output from timer on set/reset mode
 * Set every 2400us, reset in 100us after set
 *
 * PWM will stay low for TRIG_LOW_TIME, then go high for TRIG_HIGH_TIME,
 * and then low again (duty cycle = ~0.385%)
 *************************************************************************/
void set_up_led_output(void)
{
    LED_PORT_DIR |= LED_PIN;        // set as output pin
    LED_PORT_OUT &= ~LED_PIN;       // set output low on pin
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
    // set up echo pin to be input pin with pull down resistor
    ECHO_PORT_SEL &= ~ECHO_PIN;     // i/o function selected, 0
    ECHO_PORT_DIR &= ~ECHO_PIN;     // input mode (p3.5), 0
    ECHO_PORT_OUT &= ~ECHO_PIN;     // select pull-down mode, 0
    ECHO_PORT_REN |= ECHO_PIN;      // enable internal pull up, 1

    // set up interrupt on echo pin reading low to high
    ECHO_PORT_IE |= ECHO_PIN;       // enable interrupt for echo pin
    ECHO_PORT_IES &= ~ECHO_PIN;     // low to high transition
    ECHO_PORT_IFG &= ~ ECHO_PIN     // clear the interrupt flag
}


/*************************************************************************
 * ECHO port ISR to read echo pulse
 *************************************************************************/
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    //ECHO_PORT_IE &= ~ECHO_PIN;      // disable interrupt don't interrupt if already inside interrupt

    // count how long the echo input pin stays high
    unsigned long long highTimeCount = 0;
    while ((ECHO_PORT_IN & ECHO_PIN) != 0)
    {
        highTimeCount++;
    }

    // if it stayed high for anything less than the max value, turn on LED. Otherwise, turn it off
    if (highTimeCount < MAX_ECHO_HIGH_TIME)
    {
        LED_PORT_OUT |= LED_PIN;
    }
    else
    {
        LED_PORT_OUT &= ~LED_PIN;
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

