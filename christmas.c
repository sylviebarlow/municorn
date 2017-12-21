#include <stddef.h>
#include "municorn.h"

#define TIMER0_FAST TIMER0_VALUE ( 16 )

#define PIXELS 511

struct picture {
	struct pixel pixels[PIXELS];
} __attribute__ (( packed ));

uint8_t timer0_fast = TIMER0_FAST;

uint16_t total_rgb = sizeof ( struct picture );

uint8_t colours;

void next_animation ( void ) {

	colours++;
	if ( colours == ( 1 << sizeof ( struct pixel ) ) )
		colours = 0;
}

static uint8_t brightnesses[16] = {
	2, 4, 16, 32, 48, 64, 80, 98,
	100, 98, 80, 64, 48, 32, 16, 4,
};

uint8_t next_byte ( void ) {
	static uint8_t magic;
	static uint8_t subpixel;
	uint8_t brightness;
	uint8_t byte;

	brightness = brightnesses[magic];
	if ( ( 1 << subpixel ) & colours ) {
		byte = brightness;
	} else {
		byte = ( brightness * 2 );
	}

	subpixel++;
	if ( subpixel == sizeof ( struct pixel ) ) {
		subpixel = 0;

		magic++;
		if ( magic == 16 )
			magic = 0;
	}

	return byte;
}
