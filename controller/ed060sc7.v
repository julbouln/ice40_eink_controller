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
           output reg [7:0] data
       );

wire [1:0] mode;

// clock divider
// CL_DIV=1 50Mhz cl - 20ns
// CL_DIV=2 25Mhz cl - 40ns
parameter CL_DIV = 2;
reg [2:0] cl_counter=0;
wire cl = cl_counter[CL_DIV] == 1'b1;
always @(posedge clk) begin
    cl_counter<=cl_counter+1;
end

reg [6:0] phase = 0;

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
            data_in
           );
`endif

parameter MODE_CLEAR = 0;
parameter MODE_DRAW = 1;
parameter MODE_TEST = 2;

reg [7:0] source;
reg [9:0] gate;
reg [16:0] address;

parameter GATE_MAX_TIM = 613;
parameter GATE_SPV_TIM = 607;
parameter GATE_OE_TIM = 606;

parameter SOURCE_MAX_TIM=205;

wire run = ~ready;

initial begin
    le<=1'b0;
    oe<=1'b0;
    sph<=1'b1;
    data<=8'h0;
    //      cl<=1'b0;
    ckv<=1'b0;
    gmode<=1'b1;
    spv<=1'b1;

    source<=SOURCE_MAX_TIM;
    gate<=GATE_OE_TIM;
    address<=0;

    ready<=1'b1;
end

// CKV
always @(negedge cl) begin
    if(run) begin
        if(source > 203) begin
            ckv <= 1'b1;
        end else begin
            if(source > 150) begin
                ckv <= 1'b0;
            end
        end
    end
end

// SPH
always @(negedge cl) begin
    if(run) begin
//        if(source > 205) begin
        if(source == 0) begin
            if(gate < 599) begin
                sph <= 1'b0;
            end else begin
                if(gate==0) begin
                    sph <= 1'b0;                
                end
            end
        end else begin
            if(source > 130) begin
                sph <= 1'b1; 
            end
        end
    end
end

// LE
always @(negedge cl) begin
    if(run) begin
        if(source==202) begin
            le<=1'b1;
        end else begin
            le<=1'b0;
        end
    end else begin
        le<=1'b0;
    end
end

// OE + SPV
always @(posedge cl) begin
    if(run) begin
        if(gate==GATE_OE_TIM) begin
            oe<=1'b0;
        end else begin
            oe<=1'b1;
        end

        if(gate==GATE_SPV_TIM) begin
            spv<=1'b0;
        end else begin
            spv<=1'b1;
        end

    end else begin
        oe<=1'b0;
        spv<=1'b1;
    end
end

// DATA

always @(posedge clk) begin
    data <= data_in;    
end

always @(posedge cl) begin
    if(run) begin
        if(source < 200) begin
            if(gate < 600) begin
                address <= address + 1;
            end else begin
                address <= 0;
            end
        end
    end else begin
        address<=0;
    end
end

always @(negedge cl) begin
    if(run) begin
        if(source < SOURCE_MAX_TIM) begin
            source <= source + 1;
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
//    if(source==SOURCE_MAX_TIM && gate==GATE_MAX_TIM) begin
    if(source==0 && gate==0) begin
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
