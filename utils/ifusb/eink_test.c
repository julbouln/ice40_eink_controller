/* simple eink test */
#include <stdio.h>
#include <stdint.h>

#include <sys/time.h>

#include "ifusb_host.h"
#include "eink.h"


#include "testcover4.h"
char cmap[4] = {2, 1, 0, 3};

//#include "testepub2.h"
//char cmap[4] = {3,1,0,2};

//#include "mire.h"
//char cmap[4] = {0,1,2,3};

double get_time()
{
	struct timeval t;
	struct timezone tzp;
	gettimeofday(&t, &tzp);
	return t.tv_sec + t.tv_usec * 1e-6;
}

int main() {
	int i, j, k, cur_cmd = 0;
	double t1, t2;
	uint8_t cmd[8];
	uint8_t recv[8];

	if (!ifusb_init())
		goto out;

	eink_init();

	printf("initiated.\n");
	srand (time(NULL));

	for (i = 0; i < 4; i++) {
		cmd[i] = 0x00;
	}

	eink_clear_clip();
	printf("clear\n");
	eink_clear();
	printf("cleared\n");
	// image to buf
	uint8_t img_buf[120000];
	int img_pos;

	int pass = 20;

	img_pos = 0;
	for (i = 0; i < 600; i++) {
		for (j = 0; j < 800; j += 4) {
			unsigned char pixel = 0;
			if (i < width && (j + 3) < height) {
				int p;
				for (p = 0; p < 4; p++) {
					unsigned char dp = header_data[(height - (j + p)) * width + i];
					pixel |= (cmap[dp]) << ((3 - p) * 2);
				}

			} else {
				pixel = WHITE_4;
			}
			img_buf[img_pos] = pixel;
			img_pos++;
		}
	}

	eink_set_mode(0x01);
	eink_write_fb(img_buf);


	for (k = 0; k < pass; k++) {
//		eink_set_clip(200 - 10 * k, 600 + 10 * k, 200 - 10 * k, 400 + 10 * k);
		eink_set_clip( 10 * k, 800 - 10 * k, 10 * k, 600 - 10 * k);
		printf("TEST %d\n", k);
		eink_flip(img_buf);

//		eink_draw(img_buf);
//		eink_flip();
	}
	sleep(3);

	eink_exit();
out:
	ifusb_close();


}