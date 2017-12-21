#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "municorn.h"

/* Current position within frame */
uint16_t idx = 0;

/* Measure XCK phase offset */
uint8_t measure_offset ( void ) {

	/* Clear transmit complete flag */
	UCSR0A = ( _BV ( TXC0 ) );

	/* Start transmitting any (dummy) byte */
	UDR0 = 0;

	/* Wait for transmit to complete */
	while ( ! ( UCSR0A & _BV ( TXC0 ) ) ) {
		/* Do nothing */
	}

	/* Return offset measurement */
	return ICR1L;
}

/* Adjust OC1B phase offset */
void adjust_offset ( uint8_t change ) {

	/* Delay timer to adjust phase */
	TCNT1 -= change;
}

/* Align OC1B with XCK */
void align_clocks ( void ) {
	uint8_t offset;

	/* Adjust OC1B by offset amount */
	offset = measure_offset();
	adjust_offset ( offset );

	/* Correct for time to adjust, synchronise & perform edge detection */
	offset = measure_offset();
	offset *= 2;
	offset -= ICP_SYNC_DELAY;
	adjust_offset ( offset );
}

void set_animation_speed ( void ) {

	/* Check if button is pressed*/
	if ( PIND & _BV ( PD7 ) ) {

		/* Go faster if button not pressed */
		OCR0A = timer0_fast;

	} else {

		/* Go slower if button pressed */
		OCR0A = TIMER0_SLOW;

	}

	/* Avoid potentially overflowing the new threshold */
	TCNT0 = 0;
}

void set_which_animation ( void ) {
	static uint8_t previous;
	uint8_t current;

	/* Get current state of button */
	current = ( ( ~PIND ) & _BV ( PD6 ) );

	/* Check if button has just been pressed */
	if ( current & ~previous ) {

		/* Go to next animation */
		next_animation();
	}

	/* Save state of button */
	previous = current;
}

int main ( void ) {

	/* Configure USART0 */
	XCK_DDR |= _BV ( XCK_BIT );
	UCSR0B = ( _BV ( TXEN0 ) );
	UCSR0C = ( _BV ( UMSEL01 ) | _BV ( UMSEL00 ) | _BV ( UCSZ00 ) );
	UBRR0L = ( LED_WIDTH_LOW - 1 );

	/* Configure button and pin change interrupt */
	PORTD |= _BV ( PD6 );
	PORTD |= _BV ( PD7 );
	PCICR = _BV ( PCIE2 );
	PCMSK2 = _BV ( PCINT22 ) | _BV ( PCINT23 );

	/* Configure timer 0 */
	TCCR0A = ( _BV ( WGM01 ) );
	TCCR0B = ( _BV ( CS02 ) | _BV ( CS00 ) );
	TIMSK0 = ( _BV ( OCIE0A ) );

	/* Configure timer 1 */
	TCCR1A = ( _BV ( COM1B1 ) | _BV ( WGM11 ) | _BV ( WGM10 ) );
	TCCR1B = ( _BV ( ICES1 ) | _BV ( WGM13 ) | _BV ( WGM12 ) |
		   _BV ( CS10 ) );
	OCR1AL = ( LED_WIDTH_FULL - 1 );
	OCR1BL = ( LED_WIDTH_HIGH - 1 );

	/* Align clocks */
	align_clocks();

	/* Set initial animation speed */
	set_animation_speed();

	/* Go to first animation */
	next_animation();

	/* Enable interrupts */
	sei();

	/* Go to sleep */
	while ( 1 ) {
		sleep_mode();
	}
}

ISR ( USART_UDRE_vect ) {

	/* Write next byte to USART0 */
	UDR0 = next_byte();

	/* Move to next index */
	idx++;

	/* Do nothing until we finish this frame */
	if ( idx != total_rgb )
		return;

	/* Disable UDR empty interrupt & enable TX complete interupt */
	UCSR0B = ( _BV ( TXCIE0 ) | _BV ( TXEN0 ) );

	/* Reset index */
	idx = 0;
}

ISR ( PCINT2_vect ) {

	/* Update animation speed */
	set_animation_speed();

	/* Set which animation showing */
	set_which_animation();
}

ISR ( TIMER0_COMPA_vect ) {
	static uint8_t scaler;

	/* Scale interrupt rate (in addition to the hardware prescaler) */
	scaler++;
	if ( scaler != TIMER0_SW_SCALE )
		return;

	/* Reset scaler */
	scaler = 0;

	/* Turn on OC1B and TXD */
	OC1B_DDR |= _BV ( OC1B_BIT );
	TXD_DDR |= _BV ( TXD_BIT );

	/* Enable UDR empty interrupt to start transmission */
	UCSR0B = ( _BV ( UDRIE0 ) | _BV ( TXEN0 ) );
}

ISR ( USART_TX_vect ) {

	/* Turn off OC1B and TXD */
	OC1B_DDR &= ~_BV ( OC1B_BIT );
	TXD_DDR &= ~_BV ( TXD_BIT );
}
