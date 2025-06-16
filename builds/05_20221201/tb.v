`timescale 10ns / 1ns
module tb;

// ---------------------------------------------------------------------
reg clock;
reg osc;

always #0.5 clock = ~clock;
always #2.0 osc   = ~osc;        // 5.5 MHZ

initial begin clock = 0; osc = 0; #2000 $finish; end
initial begin $dumpfile("tb.vcd"); $dumpvars(0, tb); end
initial begin $readmemh("rom.hex", rom, 0); end
// ---------------------------------------------------------------------

reg  [15:0] rom[512];
wire [ 8:0] pc;
wire        locked = 1'b1;

// Простой процессор
avr avr_inst
(
    .clock      (osc),
    .locked     (locked),
    .pc         (pc),
    .flash      (rom[pc])
);

endmodule
