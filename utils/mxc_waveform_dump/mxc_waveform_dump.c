/*
 Freescale/NXP EPDC waveform firmware dumper
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Type 	Initial State 	Final State 	Waveform Period
INIT 	0-F 			F 	~4000 ms 
DU 		0-F 			0 or F 	~260 ms
GC16 	0-F 			0-F 	~760 ms
GC4 	0-F 			0, 5, A, or F 	~500 ms
A2 		0 or F 			0 or F 	~120 ms
*/

// header structures from linux driver
struct waveform_data_header {
	unsigned int wi0;
	unsigned int wi1;
	unsigned int wi2;
	unsigned int wi3;
	unsigned int wi4;
	unsigned int wi5;
	unsigned int wi6;

	unsigned int xwia: 24;
	unsigned int cs1: 8;

	unsigned int wmta: 24;
	unsigned int fvsn: 8;
	unsigned int luts: 8;
	unsigned int mc: 8;
	unsigned int trc: 8;
	unsigned int advanced_wfm_flags: 8;
	unsigned int eb: 8;
	unsigned int sb: 8;
	unsigned int reserved0_1: 8;
	unsigned int reserved0_2: 8;
	unsigned int reserved0_3: 8;
	unsigned int reserved0_4: 8;
	unsigned int reserved0_5: 8;
	unsigned int cs2: 8;
};

struct mxcfb_waveform_data_file {
	struct waveform_data_header wdh;
	uint32_t *data;	/* Temperature Range Table + Waveform Data */
};

#define WAVEFORM_MODE_INIT 0
#define WAVEFORM_MODE_DU 1
#define WAVEFORM_MODE_GC16 2
#define WAVEFORM_MODE_GC4 3
#define WAVEFORM_MODE_A2 4

uint64_t wv_modes_temps[5][14];
uint8_t	*waveform_buffer;

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

void dump_phases(int mode, int temp, int size) {
	int i, j, k, l, x, y;
	uint64_t phases;
	uint64_t m = wv_modes_temps[mode][temp];
	phases = ((uint64_t)waveform_buffer[m + 7] << 56) + ((uint64_t)waveform_buffer[m + 6] << 48) + ((uint64_t)waveform_buffer[m + 5] << 40) +
	         ((uint64_t)waveform_buffer[m + 4] << 32) + ((uint64_t)waveform_buffer[m + 3] << 24) + ((uint64_t)waveform_buffer[m + 2] << 16) +
	         ((uint64_t)waveform_buffer[m + 1] << 8) + (uint64_t)waveform_buffer[m];

	fprintf(stderr, "Dump mode: %d, temp: %d, phases %ld (total %ld bits)\n",  mode, temp, phases, phases * 512);
	uint8_t luts[phases][16][16];

	k = 0;
	for (i = 0; i < phases * 256; i += 256) {
		fprintf(stderr, "mode %d phase %d/%ld : \n", mode, k + 1, phases);
		j = 0;
		for (x = 0; x < 16; x++) {
			for (y = 0; y < 16; y++) {
				luts[k][y][x] = waveform_buffer[m + 8 + i + j];
				fprintf(stderr, "[%x-%x %x] ", (15 - y), (15 - x), waveform_buffer[m + 8 + i + j]);
//				printf("%x ",waveform_buffer[m+8+i+j]);
				j++;
			}
			fprintf(stderr, "\n");
		}
		fprintf(stderr, "\n");
		k++;
	}

	for (i = 0; i < 16; i++) {
		for (j = 0; j < 16; j++) {
			fprintf(stderr, "%x -> %x : ", i, j);
			for (k = 0; k < phases; k++) {
				fprintf(stderr, "%x ", luts[k][i][j]);
			}
			fprintf(stderr, "\n");

		}
	}

	for (i = 0; i < size; i++) {
		if (i < phases) {
			int p = i;
			uint8_t hex = (luts[p][0xF][0xF] << 6) + (luts[p][0xF][0xA] << 4) + (luts[p][0xF][0x5] << 2) + luts[p][0xF][0x0];
			printf("%02x\n", hex);
			fprintf(stderr, "%02x %c%c%c%c%c%c%c%c\n", hex, BYTE_TO_BINARY(hex) );
		} else
		{
			printf("00\n");
		}
	}
}

int main(int argc, char **argv) {
//	const char *fw = "epdc_E060SCM.fw.bin";

	fprintf(stderr, "MXC EPDC waveform dumper\n");

	if (argc < 5) {
		fprintf(stderr, "usage: mxc_waveform_dump filename mode temperature size\n");
	} else {
		char *fw = argv[1];
		int mode = atoi(argv[2]);
		int temp = atoi(argv[3]);
		int size = atoi(argv[4]);

		FILE * pFile;
		long lSize;
		char * buffer;
		size_t result;

		pFile = fopen ( fw , "rb" );
		if (pFile == NULL) {fputs ("File error", stderr); exit (1);}

		// obtain file size:
		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		rewind (pFile);

		// allocate memory to contain the whole file:
		buffer = (char*) malloc (sizeof(char) * lSize);
		if (buffer == NULL) {fputs ("Memory error", stderr); exit (2);}

		// copy the file into the buffer:
		result = fread (buffer, 1, lSize, pFile);
		if (result != lSize) {fputs ("Reading error", stderr); exit (3);}

		/* the whole file is now loaded in the memory buffer. */

		struct mxcfb_waveform_data_file *wv_file;
		wv_file = (struct mxcfb_waveform_data_file *)buffer;

		int i, j;
		int trt_entries; //  temperature range table
		int wv_data_offs; //  offset for waveform data
		int waveform_buffer_size; // size for waveform data

		uint8_t *temp_range_bounds;
		trt_entries = wv_file->wdh.trc + 1;

		fprintf(stderr, "Temperatures count: %d\n", trt_entries);

		temp_range_bounds = (uint8_t*) malloc(trt_entries);

		memcpy(temp_range_bounds, &wv_file->data, trt_entries);


		/* Search temperature ranges for a match */
		for (i = 0; i < trt_entries; i++) {
			fprintf(stderr, "Temperature %d = %d°C\n", i, temp_range_bounds[i]);
		}

		wv_data_offs = sizeof(wv_file->wdh) + trt_entries + 1;
		waveform_buffer_size = lSize - wv_data_offs;

		fprintf(stderr, "Waveform data offset: %d, size: %d\n", wv_data_offs, waveform_buffer_size);

//	if ((wv_file->wdh.luts & 0xC) == 0x4) {
//		fprintf(stderr,"waveform 5bit\n");
//	} else {
//		fprintf(stderr,"waveform 4bit\n");
//	}

		waveform_buffer = (uint8_t *)malloc(waveform_buffer_size);
		memcpy(waveform_buffer, (uint8_t *)(buffer) + wv_data_offs, waveform_buffer_size);

		int k = 0;
		int l = 0;

		uint64_t wv_modes[5];

		uint64_t addr;
		// get modes addr
		for (i = 0; i < 0x28; i += 8) {
			addr = ((uint64_t)waveform_buffer[i + 7] << 56) + ((uint64_t)waveform_buffer[i + 6] << 48) + ((uint64_t)waveform_buffer[i + 5] << 40) +
			       ((uint64_t)waveform_buffer[i + 4] << 32) + ((uint64_t)waveform_buffer[i + 3] << 24) + ((uint64_t)waveform_buffer[i + 2] << 16) +
			       ((uint64_t)waveform_buffer[i + 1] << 8) + (uint64_t)waveform_buffer[i];
			fprintf(stderr, "wave #%d addr:%08x\n", k, addr);
			wv_modes[k] = addr;
			k++;
		}

		k = 0;
		// get modes temp addr
		for (j = 0; j < 5; j++) {
			uint64_t m = wv_modes[j];
			for (i = 0; i < 112; i += 8) { // 14 * 8

				addr = ((uint64_t)waveform_buffer[m + i + 7] << 56) + ((uint64_t)waveform_buffer[m + i + 6] << 48) + ((uint64_t)waveform_buffer[m + i + 5] << 40) +
				       ((uint64_t)waveform_buffer[m + i + 4] << 32) + ((uint64_t)waveform_buffer[m + i + 3] << 24) + ((uint64_t)waveform_buffer[m + i + 2] << 16) +
				       ((uint64_t)waveform_buffer[m + i + 1] << 8) + (uint64_t)waveform_buffer[m + i];

				fprintf(stderr, "wave #%d, temp #%d addr:%08x (%x)\n", j, k, addr, m + i);
				wv_modes_temps[j][k] = addr;
				k++;
			}
			k = 0;
		}



		dump_phases(mode, temp, size);

		free(temp_range_bounds);
		free(waveform_buffer);

		// terminate
		fclose (pFile);
		free (buffer);
	}
	return 0;
}