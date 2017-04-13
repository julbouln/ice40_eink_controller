/*
	TPS65185 PMIC ifusb i2c driver
*/
#ifndef TPS65185_H
#define TPS65185_H

#include <stdint.h>

#define TPS65185_ADDR 0x68

#define TSP65185_TMST_VALUE 0x00

// reg from datasheet

#define TSP65185_TMST_VALUE 0x00 // Thermistor value read by ADC
#define TSP65185_ENABLE 0x01 // Enable/disable bits for regulators
#define TSP65185_VADJ 0x02 // VPOS/VNEG voltage adjustment
#define TSP65185_VCOM1 0x03  // Voltage settings for VCOM
#define TSP65185_VCOM2 0x04  // Voltage settings for VCOM + control
#define TSP65185_INT_EN1 0x05  // Interrupt enable group1
#define TSP65185_INT_EN2 0x06  // Interrupt enable group2
#define TSP65185_INT1 0x07  // Interrupt group1
#define TSP65185_INT2 0x08  // Interrupt group2
#define TSP65185_UPSEQ0 0x09  // Power-up strobe assignment
#define TSP65185_UPSEQ1 0x0A  // Power-up sequence delay times
#define TSP65185_DWNSEQ0 0x0B  // Power-down strobe assignment
#define TSP65185_DWNSEQ1 0x0C  // Power-down sequence delay times
#define TSP65185_TMST1 0x0D  // Thermistor configuration
#define TSP65185_TMST2 0x0E  // Thermistor hot temp set
#define TSP65185_PG 0x0F  // Power good status each rails
#define TSP65185_REVID 0x10  // Device revision ID information

// reg mask from linux

// registers (write)....
#define TPS65185_REG_ENABLE_ACTIVE		0x80
#define TPS65185_REG_ENABLE_STANDBY		0x40
#define TPS65185_REG_ENABLE_V3P3_EN		0x20
#define TPS65185_REG_ENABLE_VCOM_EN		0x10
#define TPS65185_REG_ENABLE_VDDH_EN		0x08
#define TPS65185_REG_ENABLE_VPOS_EN		0x04
#define TPS65185_REG_ENABLE_VEE_EN		0x02
#define TPS65185_REG_ENABLE_VNEG_EN		0x01
#define TPS65185_REG_ENABLE_ALL			0xff


#define TPS65185_REG_VCOM1_ALL			0xff


#define TPS65185_REG_VCOM2_ACQ			0x80
#define TPS65185_REG_VCOM2_PROG			0x40
#define TPS65185_REG_VCOM2_HiZ			0x20
//#define TPS65185_REG_VCOM2_AVG			0x18
#define TPS65185_REG_VCOM2_VCOM8		0x01
#define TPS65185_REG_VCOM2_ALL			0xff

//#define TPS65185_REG_INT_EN1_DTX_EN			0x80
#define TPS65185_REG_INT_EN1_TSD_EN					0x40
#define TPS65185_REG_INT_EN1_HOT_EN					0x20
#define TPS65185_REG_INT_EN1_TMST_HOT_EN			0x10
#define TPS65185_REG_INT_EN1_TMST_COLD_EN			0x08
#define TPS65185_REG_INT_EN1_UVLO_EN			0x04
#define TPS65185_REG_INT_EN1_ACQC_EN			0x02
#define TPS65185_REG_INT_EN1_PRGC_EN			0x01
#define TPS65185_REG_INT_EN1_ALL			0xff

#define TPS65185_REG_INT_EN2_VBUVEN				0x80
#define TPS65185_REG_INT_EN2_VDDHUVEN			0x40
#define TPS65185_REG_INT_EN2_VNUV_EN			0x20
#define TPS65185_REG_INT_EN2_VPOSUVEN			0x10
#define TPS65185_REG_INT_EN2_VEEUVEN			0x08
#define TPS65185_REG_INT_EN2_VCOMFEN			0x04
#define TPS65185_REG_INT_EN2_VNEGUVEN			0x02
#define TPS65185_REG_INT_EN2_EOCEN				0x01
#define TPS65185_REG_INT_EN2_ALL			0xff

#define TPS65185_REG_INT1_ACQC			0x02
#define TPS65185_REG_INT1_PRGC			0x01
#define TPS65185_REG_INT1_UVLO			0x04
#define TPS65185_REG_INT1_HOT				0x20
#define TPS65185_REG_INT1_TSD				0x40

#define TPS65185_REG_INT2_VB_UV			0x80
#define TPS65185_REG_INT2_VDDH_UV		0x40
#define TPS65185_REG_INT2_VN_UV			0x20
#define TPS65185_REG_INT2_VPOS_UV		0x10
#define TPS65185_REG_INT2_VEE_UV		0x08
#define TPS65185_REG_INT2_VCOMF			0x04
#define TPS65185_REG_INT2_VNEG_UV		0x02
#define TPS65185_REG_INT2_EOC				0x01

uint8_t tps65185_read(uint8_t reg);
void tps65185_write(uint8_t reg, uint8_t val);

int tps65185_get_vcom();

#endif