#ifndef MUNICORN_H
#define MUNICORN_H

#include <stdint.h>

#define FRAME_HZ_SLOW 1

#define TIMER0_PRESCALE 1024
#define TIMER0_OCR_MAX 0xff
#define TIMER0_HZ ( F_CPU / TIMER0_PRESCALE )
#define TIMER0_SW_SCALE ( ( TIMER0_HZ / TIMER0_OCR_MAX / FRAME_HZ_SLOW ) + 1 )
#define TIMER0_VALUE( HZ ) ( TIMER0_HZ / TIMER0_SW_SCALE / HZ )
#define TIMER0_SLOW TIMER0_VALUE ( FRAME_HZ_SLOW )

#define F_LED 800000
#define LED_WIDTH_LOW ( ( F_CPU / F_LED ) / 2 )
#define LED_WIDTH_FULL ( LED_WIDTH_LOW * 2 )
#define LED_WIDTH_HIGH ( LED_WIDTH_FULL / 5 )

#define ICP_SYNC_DELAY 2

#define XCK_BIT 4
#define TXD_BIT 1
#define XCK_DDR DDRD
#define TXD_DDR DDRD
#define OC1B_BIT 2
#define OC1B_DDR DDRB

struct pixel {
	uint8_t green;
	uint8_t red;
	uint8_t blue;
} __attribute__ (( packed ));

struct animation {
	const uint8_t *raw;
	uint8_t total_frames;
};

#define ANIMATION( data )					\
	{							\
		.raw = data,					\
		.total_frames = ( sizeof ( data ) /		\
				  sizeof ( struct picture ) ),	\
	}

extern uint8_t timer0_fast;
extern uint16_t total_rgb;
extern uint8_t total_animations;
extern struct animation animations[];
extern uint8_t byte_output ( uint8_t animation, uint8_t frame,
			     uint16_t position );

#endif /* MUNICORN_H */
