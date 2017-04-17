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
	for(k=0;k<20;k++)
		eink_clear();
	printf("cleared\n");
	sleep(3);

	eink_exit();
out:
	ifusb_close();


}