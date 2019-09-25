/*
 	ICE40 programmer with ifusb
 	works with https://www.olimex.com/Products/FPGA/iCE40/iCE40HX1K-EVB/open-source-hardware
*/
#include <stdio.h>
#include <stdint.h>

#include "ifusb_host.h"
#include "spi_flash.h"

#define CS_PIN IFUSB_IO2
#define CRESET_PIN IFUSB_IO3
#define CDONE_PIN IFUSB_IO4

uint8_t iceprog_done() {
	return ifusb_gpio_get(CDONE_PIN);
}

void iceprog_init() {
	ifusb_gpio_config(CRESET_PIN, IFUSB_OUTPUT_MODE);
	ifusb_gpio_config(CDONE_PIN, IFUSB_INPUT_MODE);
	spiflash_init(CS_PIN);
	ifusb_gpio_set(CRESET_PIN);

	usleep(100000);

//	printf("CDONE %d\n",iceprog_done());

//	printf("CDONE %d\n",iceprog_done());

}


void iceprog_reset() {
	ifusb_gpio_clear(CRESET_PIN);
	usleep(250000);
	ifusb_gpio_set(CRESET_PIN);
}

void iceprog_test() {

	ifusb_gpio_clear(CRESET_PIN);
	usleep(250000);
//	sleep(1);
//	printf("CDONE %d\n",iceprog_done());

	spiflash_power_up();
	uint8_t manufacturer_id, device_id1, device_id2;

	spiflash_readid(&manufacturer_id, &device_id1, &device_id2);
	printf("FLASH found %x,%x,%x\n", manufacturer_id, device_id1, device_id2);

//	spiflash_write_enable();
//	spiflash_status_write(0x00);

	spiflash_power_down();

	ifusb_gpio_set(CRESET_PIN);
	usleep(250000);

}

#define WRITE_BLOCK 256

void iceprog_program(const char *filename) {
	spiflash_write_enable();
	spiflash_status_write(0x00);

	printf("Programing ...\n");
	spiflash_write_enable();
	spiflash_erase();
	spiflash_wait(1000000);


	FILE *f = fopen(filename, "rb");

	if (f == NULL) {
		printf("Error: can't open %s\n", filename);
	}

	uint8_t buffer[WRITE_BLOCK];
	int l;
	int page = 0;
	while ((l = fread(buffer, 1, WRITE_BLOCK, f)) > 0) {
		int size = WRITE_BLOCK;
		int addr = page * WRITE_BLOCK;
		if (l < size)
			size = l;
		spiflash_write_enable();
		spiflash_write(addr, buffer, size);
		spiflash_wait(1000);
		page++;
	}

	fclose(f);

}

#define READ_BLOCK 128

void iceprog_check(const char *filename) {
	printf("Checking ...\n");

	FILE *f = fopen(filename, "rb");

	if (f == NULL) {
		printf("Error: can't open %s\n", filename);
	}

	uint8_t buffer[READ_BLOCK];
	int l;
	int page = 0;

	while ((l = fread(buffer, 1, READ_BLOCK, f)) > 0) {
		uint8_t recv[READ_BLOCK];
		int size = READ_BLOCK;
		int addr = page * READ_BLOCK;
//		printf("read %x %d\n", addr, l);
		if (l < size)
			size = l;
		spiflash_read(addr, recv, size);

		if (memcmp(buffer, recv, size)) {
			printf("iceprog block %d no not match\n", page);
			printf("iceprog FILE: \n");
			ifusb_dbg_buf(buffer, l);
			printf("iceprog FLASH: \n");
			ifusb_dbg_buf(recv, l);
		} else {
//			printf("iceprog block %d OK\n", page);
		}

		page++;
	}

	fclose(f);
}

int main(int argc, char **argv)
{
	int i;
	if (!ifusb_init())
		goto out;

	ifusb_spi_set_freq(IFUSB_SPI_FREQ_6MHZ);
//	ifusb_spi_set_freq(IFUSB_SPI_FREQ_3MHZ);
//	ifusb_spi_set_freq(IFUSB_SPI_FREQ_325KHZ);

	iceprog_init();

	usleep(100000);
//	ifusb_gpio_clear(CRESET_PIN);
//	iceprog_program(argv[1]);
//	sleep(1);

	ifusb_gpio_clear(CRESET_PIN);
	usleep(250000);
//	sleep(1);
//	printf("CDONE %d\n",iceprog_done());

	spiflash_power_up();
	uint8_t manufacturer_id, device_id1, device_id2;

	spiflash_readid(&manufacturer_id, &device_id1, &device_id2);
	printf("FLASH found %x,%x,%x\n", manufacturer_id, device_id1, device_id2);

		iceprog_program(argv[1]);
		usleep(100000);
//	for(i=0;i<16;i++)
		iceprog_check(argv[1]);

	spiflash_power_down();

//	printf("CDONE %d\n",iceprog_done());
	usleep(100000);

	ifusb_gpio_config(CS_PIN, IFUSB_INPUT_MODE);

	ifusb_gpio_set(CRESET_PIN);

//	printf("CDONE %d\n",iceprog_done());

out:
	ifusb_close();

}