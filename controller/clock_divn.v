module clock_divn #(
           //parameter WIDTH = 2,
           parameter N = 5)
       (input clk,input reset, output clk_out);

localparam WIDTH = $clog2(N);
reg [WIDTH-1:0] pos_count = 0;
reg [WIDTH-1:0] neg_count = 0;

always @(posedge clk)
    if (reset)
        pos_count <= 0;
    else if (pos_count == N-1) pos_count <= 0;
    else pos_count <= pos_count +1;

always @(negedge clk)
    if (reset)
        neg_count <= 0;
    else  if (neg_count == N-1) neg_count <= 0;
    else neg_count <= neg_count +1;

assign clk_out = ((pos_count > (N>>1)) | (neg_count > (N>>1)));
endmodule
