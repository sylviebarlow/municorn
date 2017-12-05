#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define TIMER1_PRESCALE 1024
#define TIMER1_SLOW ( F_CPU / TIMER1_PRESCALE )
#define TIMER1_FAST ( F_CPU / TIMER1_PRESCALE / 16 )

#define F_LED 400000
#define LED_WIDTH ( F_CPU / F_LED )
#define LED_WIDTH_HIGH ( LED_WIDTH / 5 )
#define LED_WIDTH_LOW ( LED_WIDTH / 2 )

#define XCK_BIT 4
#define TXD_BIT 1
#define XCK_DDR DDRD
#define TXD_DDR DDRD
#define OC0B_BIT 5
#define OC0B_DDR DDRD

#define PIXELS 64

struct pixel {
	uint8_t green;
	uint8_t red;
	uint8_t blue;
} __attribute__ (( packed ));

struct picture {
	struct pixel pixels[PIXELS];
} __attribute__ (( packed ));

struct animation {
	const uint8_t *raw;
	size_t len;
};

const uint8_t michael_raw[] PROGMEM = {
	#include "michael.png.h"
};

const uint8_t molly_raw[] PROGMEM = {
	#include "molly.png.h"
};

const uint8_t gavan_raw[] PROGMEM = {
	#include "gavan.png.h"
};

const uint8_t anna_raw[] PROGMEM = {
	#include "anna.png.h"
};

const uint8_t richard_raw[] PROGMEM = {
	#include "richard.png.h"
};

const uint8_t nataliia_raw[] PROGMEM = {
	#include "nataliia.png.h"
};

const uint8_t iliya_raw[] PROGMEM = {
	#include "iliya.png.h"
};

const uint8_t calantha_raw[] PROGMEM = {
	#include "calantha.png.h"
};

const uint8_t peter_raw[] PROGMEM = {
	#include "peter.png.h"
};
struct animation animations[] = {
	{
		.raw = michael_raw,
		.len = sizeof ( michael_raw ),
	},
	{
		.raw = anna_raw,
		.len = sizeof ( anna_raw ),
	},
	{
		.raw = molly_raw,
		.len = sizeof ( molly_raw ),
	},
	{
		.raw = richard_raw,
		.len = sizeof ( richard_raw ),
	},
	{
		.raw = gavan_raw,
		.len = sizeof ( gavan_raw ),
	},
	{
		.raw = nataliia_raw,
		.len = sizeof ( nataliia_raw ),
	},
	{
		.raw = iliya_raw,
		.len = sizeof ( iliya_raw ),
	},
	{
		.raw = calantha_raw,
		.len = sizeof ( calantha_raw ),
	},
	{
		.raw = peter_raw,
		.len = sizeof ( peter_raw ),
	},
};

/* Current animation */
uint8_t animation = 0;

/* Current frame */
uint8_t frame = 0;

/* Current position within frame */
uint8_t position = 0;

void set_animation_speed ( void ) {

	/* Check if button is pressed*/
	if ( PIND & _BV ( PD7 ) ) {

		/* Go faster if button not pressed */
		OCR1A = TIMER1_FAST;

	} else {

		/* Go slower if button pressed */
		OCR1A = TIMER1_SLOW;

	}

	/* Avoid potentially overflowing the new threshold */
	TCNT1 = 0;
}

void set_which_animation ( void ) {
	static uint8_t previous;
	uint8_t current;

	/* Get current state of button */
	current = ( PIND & _BV ( PD6 ) );

	/* Check if button has just been pressed */
	if ( previous & ~current ) {

		/* Go to next animation */
		animation++;

		/* Reset to first frame to avoid overrun */
		frame = 0;

		/* Reset to first animation if at end */
		if ( animation == ( sizeof ( animations ) /
				    sizeof ( animations[0] ) ) ) {
			animation = 0;
		}
	}

	/* Save state of button */
	previous = current;
}

int main ( void ) {

	/* Configure USART0 */
	XCK_DDR |= _BV ( XCK_BIT );
	TXD_DDR |= _BV ( TXD_BIT );
	UCSR0B = ( _BV ( TXEN0 ) );
	UCSR0C = ( _BV ( UMSEL01 ) | _BV ( UMSEL00 ) | _BV ( UCSZ00 ) );

	/* Configure button and pin change interrupt */
	PORTD |= _BV ( PD6 );
	PORTD |= _BV ( PD7 );
	PCICR = _BV ( PCIE2 );
	PCMSK2 = _BV ( PCINT22 ) | _BV ( PCINT23 );

	/* Configure timer 0 */
	OC0B_DDR |= _BV ( OC0B_BIT );
	TCCR0A = ( _BV ( COM0B1 ) | _BV ( WGM01 ) | _BV ( WGM00 ) );
	TCCR0B = ( _BV ( WGM02 ) | _BV ( CS00 ) );
	OCR0A = ( LED_WIDTH - 1 );
	OCR0B = ( LED_WIDTH_HIGH - 1 );

	// sync timer 1
	TCNT0 = 0xe8;
	UBRR0L = ( LED_WIDTH_LOW - 1 );

	/* Configure timer 1 */
	TCCR1B = ( _BV ( WGM12 ) | _BV ( CS12 ) | _BV ( CS10) );
	TIMSK1 = ( _BV ( OCIE1A ) );

	/* Set initial animation speed */
	set_animation_speed();

	/* Enable interrupts */
	sei();

	/* Go to sleep */
	while ( 1 ) {
		sleep_mode();
	}
}

ISR ( USART_UDRE_vect ) {
	struct picture *pictures =
		( ( struct picture * ) animations[animation].raw );
	struct picture *picture = &pictures[frame];
	uint8_t *bytes = ( ( uint8_t * ) picture );

	/* Write next byte to USART0 */
	UDR0 = ( pgm_read_byte(&bytes[position]) / 3 );

	/* Move to next position */
	position++;

	/* Do nothing until we finish this frame */
	if ( position != sizeof ( pictures[0] ) )
		return;

	/* Disable UDR empty interrupt */
	UCSR0B = ( _BV ( TXEN0 ) );

	/* Reset position */
	position = 0;

	/* Move to next frame */
	frame++;

	/* Do nothing until we finish the last frame*/
	if ( frame != ( animations[animation].len / sizeof ( pictures[0] ) ) )
		return;

	/* Reset to first frame */
	frame = 0;
}

ISR ( PCINT2_vect ) {

	/* Update animation speed */
	set_animation_speed();

	/* Set which animation showing */
	set_which_animation();
}

ISR ( TIMER1_COMPA_vect ) {

	/* Enable UDR empty interrupt to start transmission */
	UCSR0B = ( _BV ( UDRIE0 ) | _BV ( TXEN0 ) );

}