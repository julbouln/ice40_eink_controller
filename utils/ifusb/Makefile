EPUBTK_DIR=/home/julbouln/Travail/exp/epubtk
IFUSB_DIR=/home/julbouln/Perso/ifusb/host
OBJS=layer1.o
CFLAGS=-I$(IFUSB_DIR) -I$(IFUSB_DIR)/../common -g

READER_CFLAGS=-std=c++11 -g -O3 -fpermissive `freetype-config --cflags` -I$(EPUBTK_DIR)  -I$(EPUBTK_DIR)/libhnj -I$(EPUBTK_DIR)/epub -I$(EPUBTK_DIR)/utf8 -I$(EPUBTK_DIR)/miniz -I$(EPUBTK_DIR)/tinyxml2 -I$(EPUBTK_DIR)/drivers

%.o : %.c
	gcc $(CFLAGS) -o $@ -c $<

%.o : %.cpp
	gcc $(READER_CFLAGS) $(CFLAGS) -o $@ -c $<

all: eink_ping eink_test

eink_ping: tps65185.o eink.o eink_ping.o
	gcc -g -o eink_ping tps65185.o eink.o eink_ping.o $(IFUSB_DIR)/libifusb.a -lm -lusb-1.0

eink_test: tps65185.o eink.o eink_test.o
	gcc -g -o eink_test tps65185.o eink.o eink_test.o $(IFUSB_DIR)/libifusb.a -lm -lusb-1.0

eink_clear: tps65185.o eink.o eink_clear.o
	gcc -g -o eink_clear tps65185.o eink.o eink_clear.o $(IFUSB_DIR)/libifusb.a -lm -lusb-1.0

eink_reader: tps65185.o eink.o eink_renderer_driver.o eink_reader.o
	g++ $(CFLAGS) $(READER_CFLAGS) tps65185.o eink.o eink_renderer_driver.o eink_reader.o -o eink_reader $(EPUBTK_DIR)/libepubtk_default.a $(IFUSB_DIR)/libifusb.a -lm -lusb-1.0 `freetype-config --libs` -lncurses

eink_gif: tps65185.o eink.o eink_gif.o
	gcc -g -o eink_gif tps65185.o eink.o eink_gif.o $(IFUSB_DIR)/libifusb.a -lm -lusb-1.0

eink_iceprog: spi_flash_sst25.o ifusb_iceprog.o
	gcc -o eink_iceprog spi_flash_sst25.o ifusb_iceprog.o $(IFUSB_DIR)/libifusb.a -lm -lusb-1.0

test_vt100: tps65185.o eink.o
	gcc $(CFLAGS) tps65185.o eink.o vt100/display.c vt100/vt100.c vt100/test.c -o test_vt100 $(IFUSB_DIR)/libifusb.a -lm -lusb-1.0 -lSDL2

clean:
	rm -f *.o
	rm -f *.a
	rm -f eink_ping
	rm -f eink_test
	rm -f eink_reader

