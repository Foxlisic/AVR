`timescale 10ns / 1ns

module tb;
// ---------------------------------------------------------------------
reg intr;
reg clock;     always #0.5 clock    = ~clock;
reg clock_25;  always #1.0 clock_50 = ~clock_50;
reg clock_50;  always #2.0 clock_25 = ~clock_25;
// ---------------------------------------------------------------------
initial begin intr = 0; clock = 0; clock_25 = 0; clock_50 = 0; #5 intr = 1; #15 intr = 0; #2000 $finish; end
initial begin $dumpfile("tb.vcd"); $dumpvars(0, tb); end
initial begin $readmemh("tb.hex", prog); end
// ---------------------------------------------------------------------
reg  [15:0] prog[65536];
reg  [ 7:0] sram[65536];

always @(posedge clock) if (we) sram[address] <= data_o;
// ---------------------------------------------------------------------
wire [15:0] pc;
wire [15:0] address;
wire [15:0] ir       = prog[pc];
wire [ 7:0] data_i   = sram[address];
wire [ 7:0] data_o;
wire        we;
// ---------------------------------------------------------------------

core C1
(
    .clock      (clock_25),
    .reset_n    (1'b1),
    .intr       (intr),
    .vect       (3'h0),
    .pc         (pc),
    .ir         (ir),
    .address    (address),
    .data_i     (data_i),
    .data_o     (data_o),
    .we         (wren)
);
// ---------------------------------------------------------------------

endmodule
