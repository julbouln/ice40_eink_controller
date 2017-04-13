// adapted from http://www.fpga4fun.com/SPI2.html

module spi_slave(input clk,

                 output [7:0] rx_byte,
                 input [7:0] tx_byte,
                 output received,
                 output start_message,
                 output end_message,

                 input sck,
                 input mosi,
                 output miso,
                 input ssel
                );

// sync sck to the FPGA clock using a 3-bits shift register
reg [2:0] sckr;
always @(posedge clk) sckr <= {sckr[1:0], sck};
wire sck_risingedge = (sckr[2:1]==2'b01);  // now we can detect sck rising edges
wire sck_fallingedge = (sckr[2:1]==2'b10);  // and falling edges

// same thing for ssel
reg [2:0] sselr;
always @(posedge clk) sselr <= {sselr[1:0], ssel};
wire ssel_active = ~sselr[1];  // ssel is active low
wire ssel_startmessage = (sselr[2:1]==2'b10);  // message starts at falling edge
wire ssel_endmessage = (sselr[2:1]==2'b01);  // message stops at rising edge

// and for mosi
reg [1:0] mosir;
always @(posedge clk) mosir <= {mosir[0], mosi};
wire mosi_data = mosir[1];

// we handle SPI in 8-bits format, so we need a 3 bits counter to count the bits as they come in
reg [2:0] bitcnt;

reg byte_received;  // high when a byte has been received
reg [7:0] byte_data_received;

assign rx_byte = byte_data_received;
assign received = byte_received;
assign start_message = ssel_startmessage;
assign end_message = ssel_endmessage;

always @(posedge clk) begin
    if(ssel_active) begin
        if(sck_risingedge) begin
            bitcnt <= bitcnt + 1;
            // implement a shift-left register (since we receive the data MSB first)
            byte_data_received <= {byte_data_received[6:0], mosi_data};
        end
    end else begin
        bitcnt <= 3'b000;
    end
end

always @(posedge clk) byte_received <= ssel_active && sck_risingedge && (bitcnt==3'b111);

reg [7:0] byte_data_sent;

always @(posedge clk) begin
    if(ssel_active) begin

        if(ssel_startmessage) begin
            byte_data_sent <= 8'h0;  // first byte sent in a message is the message count
        end else
        begin
            if(sck_fallingedge) begin
                if(bitcnt==3'b000) begin
                    byte_data_sent <= tx_byte;
                end else begin
                    byte_data_sent <= {byte_data_sent[6:0], 1'b0};
                end
            end
        end
    end
end

assign miso = ssel_active ? byte_data_sent[7] : 1'bz;  // send MSB first

endmodule
