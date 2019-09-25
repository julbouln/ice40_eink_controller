PROJ = eink_controller
PIN_DEF = ice40hx1k-evb.pcf
DEVICE = hx1k
CLK_CONSTRAINT = 70

all: $(PROJ).rpt $(PROJ).bin

%.json: %.v	
#	yosys -p 'synth_ice40 -top top -blif $@' $< > out.log
	yosys -p 'synth_ice40 -top top -json $@' $< > yosys.log
# ; show -format svg -prefix eink_controller -stretch -colors 123456

%.asc: %.json
#	arachne-pnr -d $(subst hx,,$(subst lp,,$(DEVICE))) -o $@ -p $^ -P vq100
	nextpnr-ice40 --hx1k --json $^ --pcf $(PIN_DEF) --asc $@ --package vq100 --pcf-allow-unconstrained 2> nextpnr.log

%.bin: %.asc
	icepack $< $@

%.rpt: %.asc
	icetime -c $(CLK_CONSTRAINT) -p ice40hx1k-evb.pcf -P vq100 -d $(DEVICE) -mtr $@ $<

prog: $(PROJ).bin
	sudo iceprogduino $<

sudo-prog: $(PROJ).bin
	@echo 'Executing prog as root!!!'
	sudo iceprogduino $<

sim:
	iverilog -DSIM -o /tmp/test_eink eink_sim.v
	/tmp/test_eink

test_spi:
	iverilog -o /tmp/test_spi spi_slave.v
	/tmp/test_spi

test_serial:
	gcc test_serial.c -o test_serial

clean:
	rm -f $(PROJ).rpt $(PROJ).blif $(PROJ).asc $(PROJ).bin out.log

waveform:
	cat waveform_init.hex > waveform.hex
	cat waveform_du.hex >> waveform.hex
	cat waveform_gc4.hex >> waveform.hex

.PHONY: all prog clean
