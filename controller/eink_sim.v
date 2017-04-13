`include "ed060sc7.v"
`timescale 10ns/10ps
module eink_sim();
reg clk=0;
reg rst=0;
wire [7:0] x;
wire [9:0] y;
wire [16:0] addr;
reg [15:0] data_in = 16'hFFFF;
reg start = 1'b0;
wire ready;
wire [7:0] out;

wire gmode,spv,ckv,cl,le,oe,sph;


reg [1:0] mode = 0;

always #1 clk = ~clk;

always @(posedge clk) begin
    if(x%2 == 0) begin
        //      data_in = 8'h55;
        data_in <= 16'h00FF;
    end else begin
        //      data_in = 8'hAA;
        data_in <= 16'hFF00;
    end
end

wire [2:0] state;
wire [6:0] phase;
initial begin
    $dumpfile("test.vcd");
    $dumpvars(0,gmode,spv,ckv,cl,le,oe,sph,out,clk,start,x,y,addr,mode,ready,phase);
    #1000 mode = 1;
    #1000000 start=1'b1;
    #1000 start=1'b0;
    #6000000 start=1'b1;
    #1000 start=1'b0;
    #6000000 $finish();

/*    #100000 start=1'b1;
    #1000 start=1'b0;
    #100000000 mode = 1;
    #100000 start=1'b1;
    #1000 start=1'b0;

    #100000000 $finish();
    */
end

//wire [5:0] frame;

ed060sc7 eink(clk,x,y,addr,phase,mode,start,ready,gmode,spv,ckv,cl,le,oe,sph,out);

endmodule
