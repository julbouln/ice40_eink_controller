/*
	TPS65185 PMIC ifusb i2c driver
*/
#include "tps65185.h"

uint8_t tps65185_read(uint8_t reg) {
	int i;
	uint8_t val[4];

	ifusb_i2c_xfer(TPS65185_ADDR,&reg,1,val,4);
	for(i=0;i<4;i++) {
//		printf("R %x\n",val[i]);
	}


	return val[0];
}

void tps65185_write(uint8_t reg, uint8_t val) {
	uint8_t data[2];
	data[0]=reg;
	data[1]=val;

	ifusb_i2c_send(TPS65185_ADDR,data,2);

}

int tps65185_get_temp() {
	int temp=0;

	tps65185_write(TSP65185_TMST1,0b10000000);
	usleep(1000);
	temp=tps65185_read(TSP65185_TMST_VALUE);
	return temp;
}

int tps65185_get_vcom() {
	int vcom=0;
	uint8_t vcom1,vcom2;


	vcom2=tps65185_read(TSP65185_VCOM2);
	vcom1=tps65185_read(TSP65185_VCOM1);

//	printf("VCOM1 %x\n",vcom1);
//	printf("VCOM2 %x\n",vcom2);

	vcom=-((vcom1|vcom2<<8)&0x1ff)*10;

	return vcom;
}
