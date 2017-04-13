/*
Async SRAM controller
Based on Alliance AS6C1008 128x8 RAM chip mem_ctl by Jeremiah Mahler <jmmahler@gmail.com> 
*/
module asram #(
           parameter DATA_WIDTH = 16,
           parameter ADDR_WIDTH = 18)
       (
           input   clk,
           input             read_n,
           write_n,
           ce_n,
           input      [ADDR_WIDTH-1:0]  address_bus,
           input      [DATA_WIDTH-1:0]  data_bus_in,
           output     [DATA_WIDTH-1:0]  data_bus_out,

           input      [DATA_WIDTH-1:0]  mem_data_in,
           output reg      [DATA_WIDTH-1:0]  mem_data_out,
           output     [ADDR_WIDTH-1:0] mem_address,
           output wire       ceh_n,
           we_n,
           oe_n);

assign mem_address = address_bus;


// for READ
// if read enabled, drive current data, otherwise go hi Z
assign data_bus_out = (~(ce_n | read_n | ~write_n)) ? mem_data_in : DATA_WIDTH'bz;

// for WRITE
always @(posedge clk) begin
    mem_data_out <= (~(ce_n | write_n | ~read_n)) ? data_bus_in : DATA_WIDTH'bz;
end
// No "timing" is required for the control lines.
// This just follows the truth table given on page 3 of
// the Alliance RAM data sheet.

assign ceh_n = 1'b0;

// for READ
assign oe_n = (~(ce_n | read_n)) ? 1'b0 : 1'b1;

// for WRITE
assign we_n = (~(ce_n | write_n | ~read_n)) ? 1'b0 : 1'b1;

endmodule

