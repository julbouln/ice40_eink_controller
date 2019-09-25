`include "spi_slave.v"
`include "asram.v"
`include "ed060sc7.v"

module top(
           input			CLK,
           input			BUT1,
           input			BUT2,
           output			LED1,
           output			LED2,

           input 	  RX,
           output 	  TX,

           output 	  CE_N,
           output 	  WE_N,
           output 	  OE_N,
           inout [15:0]   DQ,
           output [17:0] A,

           output 	  GMODE,
           output 	  SPV,
           output 	  CKV,
           output 	  CL,
           output 	  LE,
           output 	  OE,
           output 	  SPH,
           output [7:0]  DATA,
           input SSEL,
           input SCK,
           input MOSI,
           output MISO
       );

// 100Mhz clock
wire clk = CLK;

reg read_n = 1'b1,
     write_n = 1'b1,
     ce_n = 1'b1;

reg [17:0] address_bus = 0;
reg [15:0] data_bus_in;
wire [15:0] data_bus_out;

wire [7:0] data_bus_out_h = data_bus_out[15:8];
wire [7:0] data_bus_out_l = data_bus_out[7:0];


reg rst = 0;

// bidirectional IO
// https://www.reddit.com/r/yosys/comments/4p8723/iceblink40hx1k_and_yosys/

wire mem_out_en = ~write_n;
wire [15:0] mem_out;
wire [15:0] mem_in;

SB_IO #(
          .PIN_TYPE(6'b 1010_01),
          .PULLUP(1'b 0)
      ) mem_io [15:0] (
          .PACKAGE_PIN(DQ),
          .OUTPUT_ENABLE(mem_out_en),
          .D_OUT_0(mem_out),
          .D_IN_0(mem_in)
      );

wire [17:0] address;

// SPI interface

reg start = 1'b0;
wire ready;
reg [1:0] mode = 1;

parameter SPI_CMD_NONE = 3'h00;
parameter SPI_CMD_PING = 3'h01;
parameter SPI_CMD_STATUS = 3'h02;
parameter SPI_CMD_WRITE = 3'h03;
parameter SPI_CMD_DRAW = 3'h04;
parameter SPI_CMD_SET_MODE = 3'h05;
parameter SPI_CMD_CLEAR_CLIP = 3'h06;
parameter SPI_CMD_SET_CLIP = 3'h07;

wire [7:0] spi_rx_byte;
reg [7:0] spi_tx_byte = 8'h0;
wire spi_received;
reg spi_incoming = 0;
reg spi_first_byte = 0;
reg [2:0] spi_bytes_count = 0;
wire spi_start_message;
wire spi_end_message;

reg [2:0] spi_cmd = SPI_CMD_NONE;

reg [7:0] data_out_r = 0;

reg swap = 0;

reg clip=0;

reg [7:0] clip_x1 = 0;
reg [7:0] clip_x2 = 200;

reg [9:0] clip_y1 = 0;
reg [9:0] clip_y2 = 600;

parameter SPI_CLIP_X1 = 2;
parameter SPI_CLIP_X2 = 3;
parameter SPI_CLIP_Y1_H = 4;
parameter SPI_CLIP_Y1_L = 5;
parameter SPI_CLIP_Y2_H = 6;
parameter SPI_CLIP_Y2_L = 7;

spi_slave spi(clk,
              spi_rx_byte,
              spi_tx_byte,
              spi_received,
              spi_start_message,
              spi_end_message,

              SCK,
              MOSI,
              MISO,
              SSEL
             );

parameter WRITE_IDLE = 0;
parameter WRITE_START = 1;
parameter WRITE_FLIP = 2;
parameter WRITE_DONE = 3;

reg [2:0] write_state = WRITE_IDLE;
reg [1:0] wait_flip = 0;

always @(posedge clk) begin
    if(ready) begin // waiting for new command
        if(spi_start_message) begin // new SPI command received
            spi_first_byte <= 1'b1;
            spi_bytes_count <= 1;
        end else begin
            if(spi_received) begin // new SPI data received
                if(spi_bytes_count==1) begin // first byte is command
                    spi_tx_byte <= 8'h0;
                    spi_cmd <= spi_rx_byte[2:0];
                    spi_first_byte <= 1'b0;
                    spi_bytes_count <= spi_bytes_count+1;
                    address_bus <= 0;
                end else begin // other bytes are command's data
                    case(spi_cmd)
                        SPI_CMD_WRITE: begin
                            write_state <= WRITE_START;
                            spi_bytes_count <= 0;
                        end                        
                        SPI_CMD_PING: begin
                            spi_tx_byte <= spi_rx_byte;
                            spi_bytes_count <= 0;
                        end
                        SPI_CMD_DRAW: begin
                            start <= 1;
                            spi_bytes_count <= 0;
                        end
                        SPI_CMD_SET_MODE: begin
                            mode <= spi_rx_byte;
                            spi_bytes_count <= 0;
                        end
                        SPI_CMD_STATUS: begin
                            spi_tx_byte <= {5'h0,mode,ready};
                            spi_bytes_count <= 0;
                        end
                        SPI_CMD_CLEAR_CLIP: begin
                            clip <= 0;
                            spi_bytes_count <= 0;
                        end
                        SPI_CMD_SET_CLIP: begin
                          case(spi_bytes_count)
                            SPI_CLIP_X1: begin
                              clip_x1 <= spi_rx_byte;
                            end
                            SPI_CLIP_X2: begin
                              clip_x2 <= spi_rx_byte;
                            end
                            SPI_CLIP_Y1_H: begin
                              clip_y1[9:8] <= spi_rx_byte[1:0];
                            end
                            SPI_CLIP_Y1_L: begin
                              clip_y1[7:0] <= spi_rx_byte;
                            end
                            SPI_CLIP_Y2_H: begin
                              clip_y2[9:8] <= spi_rx_byte[1:0];
                            end
                            SPI_CLIP_Y2_L: begin
                              clip_y2[7:0] <= spi_rx_byte;
                            end
                            default: begin
                            end
                          endcase
                          if(spi_bytes_count < SPI_CLIP_Y2_L) begin
                            spi_bytes_count <= spi_bytes_count+1;
                          end else begin
                            clip <= 1;
                            spi_bytes_count <= 0;
                          end
                        end
                        default: begin
                            spi_bytes_count <= 0;
                            // unknown command
                        end
                    endcase
                end
            end else begin              
              // keep previous pixel value
              case(write_state) 
              WRITE_IDLE: begin
                // disable memory access
                write_n <= 1'b1;
                read_n <= 1'b1;
                ce_n <= 1'b1;
              end
              WRITE_START: begin
                // keep previous data 
                address_bus <= address_bus + 1;
                write_n <= 1'b1;
                read_n <= 1'b0;
                ce_n <= 1'b0;
                write_state <= WRITE_FLIP;
              end
              WRITE_FLIP: begin
                // data_bus_out_prev <= data_bus_out;
                // wait memory read
                wait_flip <= wait_flip + 1;           
                if(wait_flip == 3) begin
                  data_out_r <= data_bus_out_l;
                  write_state <= WRITE_DONE;
                end
              end
              WRITE_DONE: begin
                // address_bus <= address_bus + 1;
                // data_bus_in <= data_bus_out_prev;
                // put previous pixel value in memory high byte
                data_bus_in <= {data_out_r,spi_rx_byte};
                write_n <= 1'b0;
                read_n <= 1'b1;
                ce_n <= 1'b0;
                write_state <= WRITE_IDLE;
              end
              default: begin
              end
              endcase
            end
        end
    end else begin // panel drawing
        // reading sram
        start <= 0;
        address_bus <= address; // let controller manage address bus
        write_n <= 1'b1;
        read_n <= 1'b0;
        ce_n <= 1'b0;
        if(spi_start_message) begin
            spi_first_byte <= 1'b1;
        end else begin
            if(spi_received) begin
                if(spi_first_byte) begin
                    spi_cmd <= spi_rx_byte;
                    spi_first_byte <= 1'b0;
                end else begin
                    case(spi_cmd)
                        SPI_CMD_STATUS: begin
                            spi_tx_byte <= {5'h0,mode,ready};
                        end
                        default: begin
                            // other commands are ignored during drawing
                        end
                    endcase
                end
            end
        end
    end
end

/* 
  8-10ns 256K x 16bits SRAM
  https://www.olimex.com/Products/_resources/ds_k6r4016v1d_rev40.pdf
  Notes:
  2-bits buffer = 800*600 * 8/4 = 960kBits = 120kBytes
  4-bits buffer = 800*600 * 8/2 = 1920kBits = 240kBytes
  4-bits buffer with previous framebuffer = 480kBytes
  available 44288 kBytes = max 173 4-bits phase waveform
*/

asram #(.DATA_WIDTH(16),.ADDR_WIDTH(18)) sram(clk,
        read_n,
        write_n,
        ce_n,
        address_bus,
        data_bus_in,
        data_bus_out,
        mem_in,mem_out,
        A,
        CE_N,
        WE_N,
        OE_N);

//   assign {LED1, LED2} = address_bus[15:14];
//   assign {LED1, LED2} = {rx_cnt,rx_recv};
//   assign {LED1, LED2} = {received,transmit};

//   wire [7:0] source;
//   wire [9:0] gate;


ed060sc7 eink(clk,
              //   		 source,
              //   		 gate,
              address,
//              phase,
              data_bus_out,
              mode,
              start,
              ready,
              GMODE,
              SPV,
              CKV,
              CL,
              LE,
              OE,
              SPH,
              DATA,
              clip,
              clip_x1,
              clip_x2,
              clip_y1,
              clip_y2
              );
endmodule
