#include <avr/pgmspace.h>
#include "municorn.h"

#define TIMER0_FAST TIMER0_VALUE ( 16 )

#define PIXELS 64

struct picture {
	struct pixel pixels[PIXELS];
} __attribute__ (( packed ));

struct animation {
	const uint8_t *start;
	const uint8_t *end;
};

#define ANIMATION( data )					\
	{							\
		.start = data,					\
		.end = ( data + sizeof ( data ) ),		\
	}

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

const uint8_t * start;
const uint8_t * end;
const uint8_t * position;
uint8_t animation;

uint8_t next_byte ( void ) {
	uint8_t byte = pgm_read_byte ( position );
	position++;
	if ( position == end ) {
		position = start;
	}
	return byte;
}

void next_animation ( void ) {
	start = animations[animation].start;
	position = start;
	end = animations[animation].end;
	animation++;
	if ( animation == ( sizeof ( animations ) /
			    sizeof ( animations[0] ) ) ) {
		animation = 0;
	}
}
