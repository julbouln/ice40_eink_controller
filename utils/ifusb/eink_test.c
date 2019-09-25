/* simple eink test */
#include <stdio.h>
#include <stdint.h>

#include <sys/time.h>

#include "ifusb_host.h"
#include "eink.h"


#include "testcover4.h"
char cmap[4] = {2, 1, 0, 3};

//char cmap[4] = {3, 0, 0, 3};

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

	int pass = 25;
	int step = 8;

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

	printf("test DU mode\n");
	eink_set_mode(MODE_DU);
	eink_write_fb(img_buf);
	eink_flip();

	sleep(1);
	eink_clear();

	printf("test GC4 mode\n");
	eink_set_mode(MODE_GC4);

	eink_write_fb(img_buf);
	eink_flip();
	sleep(1);
	eink_flip();

	sleep(1);

#if 0
	eink_write_fb(img_buf);
	eink_flip();
	sleep(1);
	eink_flip();
#endif

	printf("test DU mode\n");
	eink_set_mode(MODE_DU);
	eink_write_fb(img_buf);
	eink_flip();
	sleep(1);
	eink_flip();

#if 0
	eink_flip();

	for(int k=0;k<10;k++) {
	sleep(1);
	eink_write_fb(img_buf);
//	eink_set_clip(0,400,0,600);
	eink_flip();
	}
	eink_clear();
#endif

#if 0
	eink_flip();

	sleep(1);

	eink_write_fb(img_buf);
	eink_flip();

	eink_flip();
	eink_clear();
#endif

#if 0
	eink_set_clip(0, 199, 0, 600);
	eink_flip();

	eink_set_clip(400, 599, 0, 600);
	eink_flip();

	eink_set_clip(600, 800, 0, 600);
	eink_flip();

	eink_set_clip(200, 399, 0, 600);
	eink_flip();
	eink_clear();
#endif

#if 0
	for(int k=0;k<4;k++) {
	eink_set_clip(0, 800, 0, 149);
	eink_flip();

	eink_set_clip(0, 800, 150, 299);
	eink_flip();

	eink_set_clip(0, 800, 300, 449);
	eink_flip();

	eink_set_clip(0, 800, 450, 599);
	eink_flip();
}
	eink_clear();
#endif

#if 0
	for (j = 0; j < 800; j += 80) {
		for (i = 0; i < 600; i += 100) {
			printf("DRAW %dx%d:%dx%d\n", j, i, j + 79, i + 59);
			eink_set_clip(j, j + 79, i, i + 99);
			eink_flip();
//			sleep(1);
		}
	}
#endif
	/*
		eink_set_clip(0,200,0,150);
		eink_flip();

		eink_set_clip(0,200,150,300);
		eink_flip();

		eink_set_clip(200,400,150,300);
		eink_flip();

		eink_set_clip(200,400,0,150);
		eink_flip();
	*/

	/*
		eink_set_clip(200,150,400,300);
		eink_flip();

		eink_set_clip(400,300,600,450);
			eink_flip();

		eink_set_clip(600,450,800,600);
			eink_flip();

	*/
#if 0
	for (k = 0; k < pass; k++) {
		eink_set_clip(200 - step * k, 600 + step * k, 200 - step * k, 400 + step * k);
//		eink_set_clip( 10 * k, 800 - 10 * k, 10 * k, 600 - 10 * k);
		printf("TEST %d\n", k);
		eink_flip();

//		eink_draw(img_buf);
//		eink_flip();
	}
#endif
	/*
		for (k = 0; k < pass; k++) {
	//		eink_set_clip(200 - 10 * k, 600 + 10 * k, 200 - 10 * k, 400 + 10 * k);
			eink_set_clip( step * k, 800 - step * k, step * k, 600 - step * k);
			printf("TEST %d\n", k);
			eink_flip();

	//		eink_draw(img_buf);
	//		eink_flip();
		}
	*/

//	eink_clear();

//	printf("flip\n");
//	eink_flip();

//	eink_draw(img_buf);
//	eink_flip(img_buf);
	sleep(3);
	printf("exit\n");

	eink_exit();
out:
	ifusb_close();


}