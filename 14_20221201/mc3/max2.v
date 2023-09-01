module max2
(
    input  wire         clock,
    input  wire [3:0]   key,
    inout  wire [7:0]   led,
    inout  wire [9:0]   f0,
    inout  wire [9:0]   f1,
    inout  wire [9:0]   f2,
    inout  wire [9:0]   f3,
    inout  wire [9:0]   f4,
    inout  wire [9:0]   f5,
    inout  wire         dp,
    inout  wire         dn,
    inout  wire         pt
);

assign f0 = 10'hz; assign f1 = 10'hz; assign f2 = 10'hz;
assign f3 = 10'hz; assign f4 = 10'hz; assign f5 = 10'hz;

// ---------------------------------------------------------------------

wire [8:0]  pc;
wire [15:0] datain;
wire        ready;

// Простой процессор
avr avr_inst
(
    .clock      (osc),
    .locked     (ready),
    .pc         (pc),
    .flash      (datain),
    .port0      ({1'b0, dp, dn, pt, key[3:0]}),
    .port1      (led)
);

// ---------------------------------------------------------------------

ufm_reader UFMReaderUnit
(
    .osc        (osc),
    .address    (pc[8:0]),
    .datain     (datain),
    .ready      (ready)
);

endmodule

`include "../avr.v"
