module waveform(
           input clk,

           input [6:0] phase,
           output [6:0] phase_count,
           input [1:0] phase_type,
           
           input [15:0] data_in,
           output [7:0] data_out,
           output identical
       );

reg [31:0] waveform_rom [0:511];

wire identical = (data_in[7:6] == data_in[15:14] && data_in[5:4] == data_in[13:12] && data_in[3:2] == data_in[11:10] && data_in[1:0] == data_in[9:8]);

parameter PHASE_INIT_OFFSET = 0;
parameter PHASE_DU_OFFSET = 1;
parameter PHASE_GC4_OFFSET = 2;

//parameter PHASE_INIT_COUNT = 101;
//parameter PHASE_GC4_COUNT = 56;

parameter PHASE_INIT_COUNT = 98;
parameter PHASE_DU_COUNT = 22;
parameter PHASE_GC4_COUNT = 52;

initial begin
  $readmemh("waveform.hex", waveform_rom);
end

reg [31:0] waveform_phase = 0;

wire [4:0] pixel4=data_in[7:6] << 1;
wire [4:0] pixel3=data_in[5:4] << 1;
wire [4:0] pixel2=data_in[3:2] << 1;
wire [4:0] pixel1=data_in[1:0] << 1;

wire [4:0] prev_pixel4=data_in[15:14] << 3;
wire [4:0] prev_pixel3=data_in[13:12] << 3;
wire [4:0] prev_pixel2=data_in[11:10] << 3;
wire [4:0] prev_pixel1=data_in[9:8] << 3;

wire [7:0] data_out;

assign data_out = {
           waveform_phase[prev_pixel4 + pixel4 + 1],waveform_phase[prev_pixel4 + pixel4],
           waveform_phase[prev_pixel3 + pixel3 + 1],waveform_phase[prev_pixel3 + pixel3],
           waveform_phase[prev_pixel2 + pixel2 + 1],waveform_phase[prev_pixel2 + pixel2],
           waveform_phase[prev_pixel1 + pixel1 + 1],waveform_phase[prev_pixel1 + pixel1]
       };

assign phase_count = phase_type==2'b00 ? PHASE_INIT_COUNT : (phase_type==2'b01 ? PHASE_DU_COUNT : PHASE_GC4_COUNT);
wire [1:0] phase_offset = phase_type==2'b00 ? PHASE_INIT_OFFSET : (phase_type==2'b01 ? PHASE_DU_OFFSET : PHASE_GC4_OFFSET);

always @(posedge clk) begin
  waveform_phase <= waveform_rom[(phase_offset << 7) + phase];
end

endmodule
