/*
	eink controller ifusb driver
*/
#ifndef EINK_H
#define EINK_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WHITE_4 0xFF
#define	BLACK_4 0x00
#define WHITE 0b11
#define LIGHT_GRAY 0b10
#define DARK_GRAY 0b01
#define BLACK 0b00

#define CS_PIN IFUSB_IO5

#define PWRUP_PIN IFUSB_IO0
#define WAKEUP_PIN IFUSB_IO1

#define EINK_PING 0x01
#define EINK_STATUS 0x02
#define EINK_WRITE 0x03
#define EINK_DRAW 0x04
#define EINK_SET_MODE 0x05

void eink_init();
uint8_t eink_ready();
void eink_clear();
void eink_draw(uint8_t *buf);
void eink_exit();
#ifdef __cplusplus
}
#endif
#endif