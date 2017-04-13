/* simple eink test */
#include <stdio.h>
#include <stdint.h>

#include <sys/time.h>

#include "ifusb_host.h"
#include "eink.h"

int main() {
	if (!ifusb_init())
		goto out;

	ifusb_gpio_config(CS_PIN, IFUSB_OUTPUT_MODE);
	ifusb_gpio_set(CS_PIN);

	ifusb_gpio_config(IFUSB_IO2, IFUSB_OUTPUT_MODE);
	ifusb_gpio_set(IFUSB_IO2);

sleep(1);
	printf("PING %x\n",eink_ping());
sleep(1);

	out:
		ifusb_close();

}

