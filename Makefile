BARE_HZ		= 20000000
BARE_MCU	= atmega168

NANO_HZ		= 16000000
NANO_MCU	= atmega328p

BOARD		= BARE
HZ		= $($(BOARD)_HZ)
MCU		= $($(BOARD)_MCU)

CFLAGS	+= -flto -Os -g -W -Wall -Wextra -Werror -I.
CFLAGS	+= -mmcu=$(MCU) -DF_CPU=$(HZ)

GIFS	:= $(wildcard *.gif)
PNGS	+= $(patsubst %.gif,%.png,$(GIFS))
PNGS	+= $(wildcard *.png)
PNG_HS	:= $(patsubst %.png,%.png.h,$(PNGS))

all : office.bin pokemon.bin christmas.bin

%.o : %.c municorn.h $(PNG_HS) Makefile
	avr-gcc $(CFLAGS) -o $@ -c $<

%.bin : %.o municorn.o Makefile
	avr-gcc $(CFLAGS) -o $@ $< municorn.o

%.lst : %.bin Makefile
	avr-objdump -h -S $< > $@

%.p : %.lst Makefile
	less $<

%.png.h : %.png Makefile
	convert -set colorspace RGB -separate -swap 0,1 -combine \
		-fx 'p{(j % 2) ? i : (w - 1 - i), j}' \
		$< RGB:- | xxd -i > $@

%.png : %.gif Makefile
	convert -coalesce $< - | montage -coalesce -tile 1x -geometry +0+0 - $@

%.install : %.bin Makefile
	avrdude -p $(MCU) -V -U $<

clean :
	rm -f *.bin *.lst *.png.h
