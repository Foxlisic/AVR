`timescale 10ns / 1ns
module tb;
// ---------------------------------------------------------------------
reg         clock, clock25, reset_n;
always #0.5 clock       = ~clock;
always #2.0 clock25     = ~clock25;
// ---------------------------------------------------------------------
initial begin reset_n = 0; clock = 0; clock25 = 0; #3.0 reset_n = 1; #2500 $finish; end
initial begin $dumpfile("tb.vcd"); $dumpvars(0, tb); end
// ---------------------------------------------------------------------
wire [3:0]  vga_r, vga_g, vga_b;
wire        vga_hs, vga_vs;
wire        dram_clk, dram_cas, dram_ras, dram_udqm, dram_ldqm;
wire [15:0] dram_dq;
wire [12:0] dram_addr;
wire [ 1:0] dram_ba;
// ---------------------------------------------------------------------
video VIDEO
(
    .CLK        (clock25),
    .RESET_N    (reset_n),
    .R          (vga_r),
    .G          (vga_g),
    .B          (vga_b),
    .HS         (vga_hs),
    .VS         (vga_vs),
    .DRAM_CLK   (dram_clk),
    .DRAM_BA    (dram_ba),
    .DRAM_ADDR  (dram_addr),
    .DRAM_DQ    (dram_dq),
    .DRAM_CAS   (dram_cas),
    .DRAM_RAS   (dram_ras),
    .DRAM_WE    (dram_w),
    .DRAM_LDQM  (dram_ldqm),
    .DRAM_UDQM  (dram_udqm)
);

endmodule
