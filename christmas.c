#include <stddef.h>
#include "municorn.h"

#define TIMER0_FAST TIMER0_VALUE ( 16 )

#define PIXELS 511

struct picture {
	struct pixel pixels[PIXELS];
} __attribute__ (( packed ));

uint8_t timer0_fast = TIMER0_FAST;

uint16_t total_rgb = sizeof ( struct picture );

uint8_t spacings[] = { 4, 7, 8, 10, 15, 16 };

uint8_t spacing;

void next_animation ( void ) {

	spacing++;
	if ( spacing == sizeof ( spacings ) )
		spacing = 0;
}

#define RED ( 1 << offsetof ( struct pixel, red ) )
#define GREEN ( 1 << offsetof ( struct pixel, green ) )
#define BLUE ( 1 << offsetof ( struct pixel, blue ) )

uint8_t magics[] = {
	GREEN, BLUE, RED, ( BLUE | RED ), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

uint8_t next_byte ( void ) {
	static uint8_t magic = 0;
	static uint8_t subpixel = 1;
	static uint8_t brightness = 0;
	uint8_t byte;

	if ( subpixel & magics[magic] ) {
		byte = brightness;
	} else {
		byte = 0x00;
	}

	subpixel <<= 1;
	if ( subpixel == ( 1 << sizeof ( struct pixel ) ) ) {
		subpixel = 1;

		magic++;
		if ( magic == spacings[spacing] )
			magic = 0;

		brightness++;
	}

	return byte;
}
