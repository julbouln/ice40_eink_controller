`include "waveform.v"
`timescale 10ns/10ps
module ed060sc7(
           input clk,
`ifdef SIM
           output [7:0] source,
           output [9:0] gate,
           output [16:0] address,
           output [6:0] phase,
`else
           output [16:0] address,
           input [15:0] raw_data_in,
`endif            

           input [1:0] mode,
           input start,
           output reg ready,

           output reg gmode,
           output reg spv,
           output reg ckv,
           output cl,
           output reg le,
           output reg oe,
           output reg sph,
           output reg [7:0] data, // 4 x 2-bits pixel signal,

           input clip,
           input [7:0] clip_x1,
           input [7:0] clip_x2,
           input [9:0] clip_y1,
           input [9:0] clip_y2

       );

wire [1:0] mode;

// clock divider
// CL_DIV=1 50Mhz cl - 20ns clk/2
// CL_DIV=2 25Mhz cl - 40ns clk/4
parameter CL_DIV = 1;
reg [2:0] cl_counter=0;
wire cl = cl_counter[CL_DIV] == 1'b1;
always @(posedge clk) begin
    cl_counter<=cl_counter+1;
end

reg [6:0] phase = 0;

wire identical;

`ifdef SIM
    reg [15:0] data_in;
    reg [7:0] raw_data_in;
    reg [6:0] phase_count = 4;
always @(posedge clk) begin
    if(address%2 == 0) begin
        //      data_in = 8'h55;
        data_in <= 16'h00;
    end else begin
        //      data_in = 8'hAA;
        data_in <= 16'hFF;
    end
end
    
`else
wire [6:0] phase_count;
wire [7:0] data_in;
waveform wv(clk,
            phase,
            phase_count,
            mode,
            raw_data_in,
            data_in,
            identical
           );
`endif

parameter MODE_CLEAR = 0;
parameter MODE_DRAW = 1;
parameter MODE_TEST = 2;

reg [7:0] source;
reg [9:0] gate;
reg [16:0] address;

parameter GATE_SIZE = 600;
parameter SOURCE_SIZE = 200;

parameter GATE_MAX_TIM = 613;
parameter GATE_SPV_TIM = 607;
parameter GATE_OE_TIM = 606;

parameter SOURCE_MAX_TIM = 205;
parameter SOURCE_LE_TIM = 202;
parameter SOURCE_CKV_START = 150;
parameter SOURCE_CKV_END = 203;
parameter SOURCE_SPH_START = 130;

wire run = ~ready;

wire skip=clip && ~(gate >= clip_y1 && gate <= clip_y2);
//reg skip = 0;

initial begin
    le <= 1'b0;
    oe <= 1'b0;
    sph <= 1'b1;
    data <= 8'h0;
    ckv <= 1'b0;
    gmode <= 1'b1;
    spv <= 1'b1;

    source <= SOURCE_MAX_TIM;
    gate <= GATE_OE_TIM;
    address <= 0;

    ready <= 1'b1;
end

// CKV
always @(negedge cl) begin
    if(run) begin
        if(source > SOURCE_CKV_END) begin
            ckv <= 1'b1;
        end else begin
            if(source > SOURCE_CKV_START) begin
                ckv <= 1'b0;
            end
        end
    end
end

// SPH
always @(negedge cl) begin
    if(run && ~skip) begin
        if(source == 0) begin
            if(gate < GATE_SIZE) begin
                sph <= 1'b0;
            end else begin
                if(gate == 0) begin
                    sph <= 1'b0;                
                end
            end
        end else begin
            if(source > SOURCE_SPH_START) begin
                sph <= 1'b1; 
            end
        end
    end
end

// LE
always @(negedge cl) begin
    if(run && ~skip) begin
        if(source == SOURCE_LE_TIM) begin
            le <= 1'b1;
        end else begin
            le <= 1'b0;
        end
    end else begin
        le <= 1'b0;
    end
end

// SPV
always @(posedge cl) begin
    if(run) begin
        if(gate == GATE_SPV_TIM) begin
            spv <= 1'b0;
        end else begin
            spv <= 1'b1;
        end
    end else begin
        spv <= 1'b1;
    end
end

// OE
always @(posedge cl) begin
    if(run && ~skip) begin
        if(gate == GATE_OE_TIM) begin
            oe <= 1'b0;
        end else begin
            oe <= 1'b1;
        end
    end else begin
        oe <= 1'b0;        
    end
end

// DATA

always @(posedge clk) begin
    if(~clip || (source >= clip_x1 && source <= clip_x2 && gate >= clip_y1 && gate <= clip_y2)) begin
     if(identical && mode > 0) begin
            data <= 8'h0;
        end else begin
            data <= data_in;
        end
    end else begin
        data <= 8'h0;
    end
end

always @(negedge cl) begin
    if(run) begin
        if(source < SOURCE_SIZE) begin
            if(gate < GATE_SIZE) begin
                if(skip) begin
                    address <= address + SOURCE_SIZE;
                end else begin
                    address <= address + 1;
                end
//                address <= address + 1;
            end else begin
                address <= 0;
            end
        end
    end else begin
        address <= 0;
    end
end

always @(negedge cl) begin
    if(run) begin       
        if(source < SOURCE_MAX_TIM) begin
            if(skip && source < SOURCE_SIZE) begin
                source <= SOURCE_SIZE;
            end else begin
                source <= source + 1;
            end
        end else begin
            source <= 0;
            if(gate < GATE_MAX_TIM) begin
                gate <= gate + 1;
            end else begin
                gate <= 0;
            end
        end
    end
end

always @(posedge cl) begin
    if(source == 0 && gate == 0) begin
        phase <= phase + 1;
    end
    if(phase < (phase_count)) begin
        if(start) begin
            ready <= 1'b0;
        end
    end else begin
        phase <= 0;
        ready <= 1'b1;
    end
end

endmodule
