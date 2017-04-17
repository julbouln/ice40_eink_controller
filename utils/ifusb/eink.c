/*
	eink controller ifusb driver
*/
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>

#include "ifusb.h"
#include "ifusb_host.h"

#include "tps65185.h"
#include "eink.h"

double eink_bench()
{
	struct timeval t;
	struct timezone tzp;
	gettimeofday(&t, &tzp);
	return t.tv_sec + t.tv_usec * 1e-6;
}

void eink_init() {
	printf("ifusb_spi_set_freq\n");
//	ifusb_spi_set_freq(IFUSB_SPI_FREQ_325KHZ);
//	ifusb_spi_set_freq(IFUSB_SPI_FREQ_3MHZ);
	ifusb_spi_set_freq(IFUSB_SPI_FREQ_12MHZ);
//	ifusb_spi_set_freq(IFUSB_SPI_FREQ_24MHZ);

	ifusb_gpio_config(CS_PIN, IFUSB_OUTPUT_MODE);
	ifusb_gpio_set(CS_PIN);

	ifusb_gpio_config(PWRUP_PIN, IFUSB_OUTPUT_MODE);
	ifusb_gpio_clear(PWRUP_PIN);

	ifusb_gpio_config(WAKEUP_PIN, IFUSB_OUTPUT_MODE);
	ifusb_gpio_clear(WAKEUP_PIN);

	printf("eink wake up ... ");
	ifusb_gpio_set(WAKEUP_PIN);
	printf("OK\n");

	usleep(100000);
//int i;
//for(i=0;i<128;i++) {
//	printf("TPS65185 RevID %x\n",tps65185_read(TSP65185_REVID));

	printf("TPS65185 VCOM %dmV\n", tps65185_get_vcom());
	usleep(10000);
	printf("TPS65185 TEMP %d°\n", tps65185_get_temp());
	usleep(10000);

//	tps65185_write(TSP65185_VCOM1,0x64); // 100
//	tps65185_write(TSP65185_VCOM1,0x7d); // 125
//	tps65185_write(TSP65185_VCOM1,0x96); // 150
//	tps65185_write(TSP65185_VCOM1,0xaf); // 175
	tps65185_write(TSP65185_VCOM1, 0xbe); // 200
//	tps65185_write(TSP65185_VCOM1,0xc8); // 190
//	tps65185_write(TSP65185_VCOM1,0xe1); // 225
	usleep(10000);
	printf("TPS65185 VCOM %dmV\n", tps65185_get_vcom());
//}

	printf("eink power up ... ");
	ifusb_gpio_set(PWRUP_PIN);

	printf("OK\n");
	usleep(100000);

}

void eink_exit() {
	ifusb_gpio_clear(WAKEUP_PIN);
	ifusb_gpio_clear(PWRUP_PIN);
}

uint8_t eink_ping() {
	uint8_t cmd[4];
	uint8_t recv[4];

	cmd[0] = EINK_PING;
	cmd[1] = 0xAA;
	ifusb_gpio_clear(CS_PIN);
	ifusb_spi_xfer(cmd, recv, 4);
	ifusb_gpio_set(CS_PIN);

	printf("PING %x %x %x %x\n", recv[0], recv[1], recv[2], recv[3]);

	return recv[2];
}

uint8_t eink_ready() {
	uint8_t cmd[4];
	uint8_t recv[4];

	cmd[0] = EINK_STATUS;
	cmd[1] = 0x00;
	ifusb_gpio_clear(CS_PIN);
	ifusb_spi_xfer(cmd, recv, 4);
	ifusb_gpio_set(CS_PIN);

	return recv[2] & 0b1;
}

#define CHUNK_SIZE 4096

void eink_clear_clip() {
	uint8_t cmd[4];
	cmd[0] = EINK_CLEAR_CLIP;
	cmd[1] = 0x00;

	ifusb_gpio_clear(CS_PIN);
	ifusb_spi_send(cmd, 2);
	ifusb_gpio_set(CS_PIN);
	usleep(100);
}

void eink_set_clip(int x1, int x2, int y1, int y2) {
	uint8_t cmd[4];
	cmd[0] = EINK_SET_CLIP;
	cmd[1] = (uint8_t)(x1/4);
	cmd[2] = (uint8_t)(x2/4);
	cmd[3] = (uint8_t)((y1 >> 8) & 0xFF);
	cmd[4] = (uint8_t)(y1 & 0xFF);
	cmd[5] = (uint8_t)((y2 >> 8) & 0xFF);
	cmd[6] = (uint8_t)(y2 & 0xFF);

//	printf("CLIP %x %x %x %x %x %x\n",cmd[1],cmd[2],cmd[3],cmd[4],cmd[5],cmd[6]);

	ifusb_gpio_clear(CS_PIN);
	ifusb_spi_send(cmd, 7);
	ifusb_gpio_set(CS_PIN);
	usleep(100);
}

void eink_set_mode(int mode) {
	uint8_t cmd[2];

	// set draw mode
	cmd[0] = EINK_SET_MODE;
	cmd[1] = mode;

	ifusb_gpio_clear(CS_PIN);
	ifusb_spi_send(cmd, 2);
	ifusb_gpio_set(CS_PIN);
}

void eink_flip() {
	uint8_t cmd[4];
	cmd[0] = EINK_DRAW;
	cmd[1] = 0x00;

	ifusb_gpio_clear(CS_PIN);
	ifusb_spi_send(cmd, 2);
	ifusb_gpio_set(CS_PIN);

	while (eink_ready() == 0) {
		usleep(100);
	}
	usleep(1000);
}

void eink_clear() {
	double t1, t2;
	int i, k;
	uint8_t cmd[4];
	uint8_t recv[4];

	int chunk_pos = 0;
	uint8_t chunk_buf[CHUNK_SIZE];

	t1 = eink_bench();

	eink_set_mode(MODE_INIT);

	// clear data
	cmd[0] = EINK_WRITE;
	ifusb_gpio_clear(CS_PIN);
	ifusb_spi_send(cmd, 1);

	// set pixels to white
	for (i = 0; i < 120000; i++) {
		chunk_buf[chunk_pos] = 0xFF;
		chunk_pos++;
		// flush
		if (chunk_pos >= CHUNK_SIZE) {
			ifusb_spi_send(chunk_buf, CHUNK_SIZE);
			chunk_pos = 0;
		}
	}

	ifusb_spi_send(chunk_buf, chunk_pos);
	ifusb_gpio_set(CS_PIN);

	eink_flip();
	t2 = eink_bench();
	printf("eink_clear in %f\n", t2 - t1);
//sleep(3);

}

void eink_write_fb(uint8_t *buf) {
	double t1, t2;
	uint8_t cmd[2];
	int chunk_pos = 0;
	uint8_t chunk_buf[CHUNK_SIZE];
	int i, k;

	t1 = eink_bench();
	// send data

	cmd[0] = EINK_WRITE;
	ifusb_gpio_clear(CS_PIN);
	ifusb_spi_send(cmd, 1);

	for (i = 0; i < 120000; i++) {
		chunk_buf[chunk_pos] = buf[i];
		chunk_pos++;
		// flush
		if (chunk_pos >= CHUNK_SIZE) {
			ifusb_spi_send(chunk_buf, CHUNK_SIZE);
			chunk_pos = 0;
		}
	}
	ifusb_spi_send(chunk_buf, chunk_pos);
	ifusb_gpio_set(CS_PIN);

	t2 = eink_bench();
	printf("eink_draw data sent in %f\n", t2 - t1);

}

void eink_draw(uint8_t *buf) {
	double t1, t2;
	uint8_t cmd[2];

	t1 = eink_bench();

	eink_set_mode(MODE_GC4);
	eink_write_fb(buf);
	eink_flip();

	t2 = eink_bench();
	printf("eink_draw in %f\n", t2 - t1);

}
