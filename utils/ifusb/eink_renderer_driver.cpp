#include "eink.h"
#include "eink_renderer_driver.h"

namespace Epub {

EinkRendererDriver::EinkRendererDriver() : DefaultBitmapRendererDriver(NULL) {
}

void EinkRendererDriver::draw_page_start(int page) {

/*	eink_clear(); */
	int k;
	for(k=0;k<480000;k++) {
		screen_buffer[k]=0xFFFFFFFF; // white
	}
}

void EinkRendererDriver::draw_page_end(int page) {
	uint32_t rmask, gmask, bmask, amask;

	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;

	int i, j;
	int pix_pos = 0;

	for (i = 0; i < 600; i++) {
		for (j = 0; j < 800; j += 4) {
			unsigned char pixel = 0;
//			if (i >= pos.x && j >= pos.y && i < (pos.x+sz.w) && (j + 3) < (pos.y+sz.h)) {
				int p;
				for (p = 0; p < 4; p++) {
					uint32_t pix = screen_buffer[(800 - (j + p)) * 600 + i];
					uint8_t a = ((uint32_t)(pix & amask) >> 24) & 0xFF;
					uint8_t b = ((uint32_t)(pix & bmask) >> 16) & 0xFF;
					uint8_t g = ((uint32_t)(pix & gmask) >> 8) & 0xFF;
					uint8_t r = ((uint32_t)(pix & rmask)) & 0xFF;

					uint8_t gray = (uint8_t)((0.30*(float)r + 0.59*(float)g + 0.11*(float)b));


					// convert to 2-bits
					if (gray > 0xAA + (0xFF - 0xAA) / 2 && gray <= 0xFF) {
						pixel |= (0b11 << ((3 - p) * 2));
					} else if (gray > 0x55 + (0xAA - 0x55) / 2 && gray <= 0xAA + (0xFF - 0xAA) / 2) {
						pixel |= (0b10 << ((3 - p) * 2));
					} else if (gray > 0x55 / 2 && gray <= 0x55 + (0xAA - 0x55) / 2) {
						pixel |= (0b01 << ((3 - p) * 2));
					} else {
						pixel |= (0b00 << ((3 - p) * 2));
					}
					
				}
//			} else {
//				pixel = WHITE_4;
//			}
//				printf("[%d %x] ",pix_pos,pixel);
			eink_buffer[pix_pos] = pixel;
			pix_pos++;

		}
	}
//		printf("DONE \n");
//	printf("REDRAW PAGE\n");
	eink_draw(eink_buffer);
}

// only needed method for BitmapRendererDriver
void EinkRendererDriver::draw_bitmap(void *data, Position pos, Size sz, int stride) {

	uint32_t rmask, gmask, bmask, amask;

	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;

	int i, j;

	for(j=0;j<sz.h;j++) {
		for(i=0;i<sz.w;i++) {
				int screen_pos=(j+pos.y) * 600 + (i+pos.x);

				if(screen_pos >= 0 && screen_pos < 480000) {
					uint8_t r;
					uint8_t g;
					uint8_t b;
					
					uint32_t npix = ((uint32_t*)data)[j * sz.w + i];

					uint8_t a2 = ((uint32_t)(npix & amask) >> 24) & 0xFF;
					uint8_t b2 = ((uint32_t)(npix & bmask) >> 16) & 0xFF;
					uint8_t g2 = ((uint32_t)(npix & gmask) >> 8) & 0xFF;
					uint8_t r2 = ((uint32_t)(npix & rmask)) & 0xFF;

					uint32_t opix = screen_buffer[screen_pos];
					uint8_t a3 = ((uint32_t)(opix & amask) >> 24) & 0xFF;
					uint8_t b3 = ((uint32_t)(opix & bmask) >> 16) & 0xFF;
					uint8_t g3 = ((uint32_t)(opix & gmask) >> 8) & 0xFF;
					uint8_t r3 = ((uint32_t)(opix & rmask)) & 0xFF;

					r=(r2*(float)a2/255.0) + (r3*(1.0 - (float)a2/255));
					g=(g2*(float)a2/255.0) + (g3*(1.0 - (float)a2/255));
					b=(b2*(float)a2/255.0) + (b3*(1.0 - (float)a2/255));
					
//					printf("[%d %x,%x,%x:%x,%x,%x(%x)-> %x,%x,%x] ",screen_pos,r3,g3,b3,r2,g2,b2,a2,r,g,b);
					screen_buffer[screen_pos]=(0xFF << 24) + (b << 16) + (g << 8) + r;
				}
		}
	}
//	printf("DONE \n");
/*
	
	*/
//	printf("\n");
}
};
