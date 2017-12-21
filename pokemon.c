#include <avr/pgmspace.h>
#include "municorn.h"

#define TIMER0_FAST TIMER0_VALUE ( 4 )

#define PIXELS 256

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

const uint8_t fire[] PROGMEM = {
	#include "fire.png.h"
};

const uint8_t seahorsey[] PROGMEM = {
	#include "seahorsey.png.h"
};

const uint8_t seamonster[] PROGMEM = {
	#include "seamonster.png.h"
};

const uint8_t sheepy[] PROGMEM = {
	#include "sheepy.png.h"
};

const uint8_t mrmonster[] PROGMEM = {
	#include "mrmonster.png.h"
};

const uint8_t pigglepuff[] PROGMEM = {
	#include "pigglepuff.png.h"
};

const uint8_t blueblob[] PROGMEM = {
	#include "blueblob.png.h"
};

const uint8_t bluedog[] PROGMEM = {
	#include "bluedog.png.h"
};

const uint8_t dino[] PROGMEM = {
	#include "dino.png.h"
};

const uint8_t greenbean[] PROGMEM = {
	#include "greenbean.png.h"
};

const uint8_t kanga[] PROGMEM = {
	#include "kanga.png.h"
};

const uint8_t monster[] PROGMEM = {
	#include "monster.png.h"
};

const uint8_t robin[] PROGMEM = {
	#include "robin.png.h"
};

const uint8_t birdey[] PROGMEM = {
	#include "birdey.png.h"
};

struct animation animations[] = {
	ANIMATION ( fire ),
	ANIMATION ( seahorsey ),
	ANIMATION ( seamonster ),
	ANIMATION ( sheepy ),
	ANIMATION ( mrmonster ),
	ANIMATION ( pigglepuff ),
	ANIMATION ( dino ),
	ANIMATION ( greenbean ),
	ANIMATION ( birdey ),
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
