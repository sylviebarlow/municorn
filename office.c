#include <avr/pgmspace.h>
#include "municorn.h"

#define TIMER0_FAST TIMER0_VALUE ( 16 )

#define PIXELS 64

struct picture {
	struct pixel pixels[PIXELS];
} __attribute__ (( packed ));

const uint8_t michael[] PROGMEM = {
	#include "michael.png.h"
};

const uint8_t molly[] PROGMEM = {
	#include "molly.png.h"
};

const uint8_t gavan[] PROGMEM = {
	#include "gavan.png.h"
};

const uint8_t anna[] PROGMEM = {
	#include "anna.png.h"
};

const uint8_t richard[] PROGMEM = {
	#include "richard.png.h"
};

const uint8_t nataliia[] PROGMEM = {
	#include "nataliia.png.h"
};

const uint8_t iliya[] PROGMEM = {
	#include "iliya.png.h"
};

const uint8_t calantha[] PROGMEM = {
	#include "calantha.png.h"
};

const uint8_t peter[] PROGMEM = {
	#include "peter.png.h"
};

struct animation animations[] = {
	ANIMATION ( michael ),
	ANIMATION ( anna ),
	ANIMATION ( molly ),
	ANIMATION ( richard ),
	ANIMATION ( gavan ),
	ANIMATION ( nataliia ),
	ANIMATION ( iliya ),
	ANIMATION ( calantha ),
	ANIMATION ( peter ),
};

uint8_t timer0_fast = TIMER0_FAST;

uint16_t total_rgb = sizeof ( struct picture );

uint8_t total_animations = ( sizeof ( animations ) / sizeof ( animations[0] ) );

uint8_t byte_output ( uint8_t animation, uint8_t frame, uint16_t position ) {
	struct picture *pictures =
		( ( struct picture * ) animations[animation].raw );
	struct picture *picture = &pictures[frame];
	uint8_t *bytes = ( ( uint8_t * ) picture );

	return pgm_read_byte(&bytes[position]);
}
