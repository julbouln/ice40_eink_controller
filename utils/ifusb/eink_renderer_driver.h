#ifndef EINK_RENDERER_DRIVER_H
#define EINK_RENDERER_DRIVER_H

#include "eink.h"
#include "default_bitmap_renderer_driver.h"
namespace Epub {

class EinkRendererDriver : public DefaultBitmapRendererDriver {
	uint8_t eink_buffer[120000];
	uint32_t screen_buffer[480000];
public:
	EinkRendererDriver();
	void draw_page_start(int page);
	void draw_page_end(int page);
	// only needed method for BitmapRendererDriver
	void draw_bitmap(void *data,Position pos, Size sz, int stride);
};
}
#endif