#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#define STBI_ONLY_BMP
#define STBI_ONLY_GIF

#include "stb_image.h"
#include "stb_image_write.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gif_result_t {
	int delay;
	unsigned char *data;
	struct gif_result_t *next;
} gif_result;

STBIDEF unsigned char *stbi_xload(char const *filename, int *x, int *y, int *frames)
{
	FILE *f;
	stbi__context s;
	unsigned char *result = 0;

	if (!(f = stbi__fopen(filename, "rb")))
		return stbi__errpuc("can't fopen", "Unable to open file");

	stbi__start_file(&s, f);

	if (stbi__gif_test(&s))
	{
		int c;
		stbi__gif g;
		gif_result head;
		gif_result *prev = 0, *gr = &head;

		memset(&g, 0, sizeof(g));
		memset(&head, 0, sizeof(head));

		*frames = 0;

		while (gr->data = stbi__gif_load_next(&s, &g, &c, 4))
		{
			if (gr->data == (unsigned char*)&s)
			{
				gr->data = 0;
				break;
			}

			if (prev) prev->next = gr;
			gr->delay = g.delay;
			prev = gr;
			gr = (gif_result*) stbi__malloc(sizeof(gif_result));
			memset(gr, 0, sizeof(gif_result));
			++(*frames);
		}

		STBI_FREE(g.out);

		if (gr != &head)
			STBI_FREE(gr);

		if (*frames > 0)
		{
			*x = g.w;
			*y = g.h;
		}

		result = head.data;

		if (*frames > 1)
		{
			unsigned int size = 4 * g.w * g.h;
			unsigned char *p = 0;

			result = (unsigned char*)stbi__malloc(*frames * (size + 2));
			gr = &head;
			p = result;

			while (gr)
			{
				prev = gr;
				memcpy(p, gr->data, size);
				p += size;
				*p++ = gr->delay & 0xFF;
				*p++ = (gr->delay & 0xFF00) >> 8;
				gr = gr->next;

				STBI_FREE(prev->data);
				if (prev != &head) STBI_FREE(prev);
			}
		}
	}
	else
	{
		stbi__result_info ri;

		result = stbi__load_main(&s, x, y, frames, 4, &ri, 4);
		*frames = !!result;
	}

	fclose(f);
	return result;
}


#include "ifusb_host.h"
#include "eink.h"

int main(int argc, char **argv)
{
	int i, j, k, cur_cmd = 0;
	double t1, t2;
	uint8_t cmd[8];
	uint8_t recv[8];


	uint8_t img_buf[120000];
	int img_pos;

	int frame;
	int w, h, frames;
	unsigned char *res;


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

	res = stbi_xload(argv[1], &w, &h, &frames);
	printf("%d %d %d\n", w, h, frames);

	eink_set_mode(MODE_GC4);
	eink_set_clip(0, h, 0, w);

	int pass = 20;

	for (k = 0; k < pass; k++) {
		for ( frame = 0; frame < frames - 1; frame++) {
			int offset = frame * (sizeof(unsigned char) * 4 * w * h + 2);
//		printf("%d %x%x%x%x\n", offset, res[offset]);
			img_pos = 0;
			for (i = 0; i < 600; i++) {
				for (j = 0; j < 800; j += 4) {

					unsigned char pixel = 0;
					if (i < w && (j + 3) < h) {
						int p;
						for (p = 0; p < 4; p++) {
//						unsigned char dp = res[offset+(w - (j + p)) * w + i];
							unsigned char *dp = res + offset + ((h - (j + p)) * w * 4 + i * 4);
//						unsigned char dp = res + offset + (w - (j + p)) * w + i;
//							if(dp)
							pixel |= (dp[0] >> 6) << ((3 - p) * 2);
						}

					} else {
						pixel = WHITE_4;
					}
					img_buf[img_pos] = pixel;
					img_pos++;

//				printf("%x|", res[offset + i * h + j]);
				}
//			printf("\n");
			}
//			eink_draw(img_buf);

			eink_write_fb(img_buf);
			eink_flip(img_buf);

		}
	}
	eink_exit();
out:
	ifusb_close();

#if 0


#endif
}

#ifdef __cplusplus
}
#endif
