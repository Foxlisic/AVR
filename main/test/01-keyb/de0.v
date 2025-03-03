module de0
(
      // Reset
      input              RESET_N,

      // Clocks
      input              CLOCK_50,
      input              CLOCK2_50,
      input              CLOCK3_50,
      inout              CLOCK4_50,

      // DRAM
      output             DRAM_CKE,
      output             DRAM_CLK,
      output      [1:0]  DRAM_BA,
      output      [12:0] DRAM_ADDR,
      inout       [15:0] DRAM_DQ,
      output             DRAM_CAS_N,
      output             DRAM_RAS_N,
      output             DRAM_WE_N,
      output             DRAM_CS_N,
      output             DRAM_LDQM,
      output             DRAM_UDQM,

      // GPIO
      inout       [35:0] GPIO_0,
      inout       [35:0] GPIO_1,

      // 7-Segment LED
      output      [6:0]  HEX0,
      output      [6:0]  HEX1,
      output      [6:0]  HEX2,
      output      [6:0]  HEX3,
      output      [6:0]  HEX4,
      output      [6:0]  HEX5,

      // Keys
      input       [3:0]  KEY,

      // LED
      output      [9:0]  LEDR,

      // PS/2
      inout              PS2_CLK,
      inout              PS2_DAT,
      inout              PS2_CLK2,
      inout              PS2_DAT2,

      // SD-Card
      output             SD_CLK,
      inout              SD_CMD,
      inout       [3:0]  SD_DATA,

      // Switch
      input       [9:0]  SW,

      // VGA
      output      [3:0]  VGA_R,
      output      [3:0]  VGA_G,
      output      [3:0]  VGA_B,
      output             VGA_HS,
      output             VGA_VS
);

// High-Impendance-State
assign DRAM_DQ = 16'hzzzz;
assign GPIO_0  = 36'hzzzzzzzz;
//assign GPIO_1  = 36'hzzzzzzzz;

assign GPIO_1 = {1'b0, PS2_CLK, PS2_DAT, tmp[0], 32'bz}; // err

// LED OFF
reg [23:0] DAT;

hex7 A1(.i(DAT[23:20]), .o(HEX5));
hex7 A2(.i(DAT[19:16]), .o(HEX4));
hex7 A3(.i(DAT[15:12]), .o(HEX3));
hex7 A4(.i(DAT[11: 8]), .o(HEX2));
hex7 A5(.i(DAT[ 7: 4]), .o(HEX1));
hex7 A6(.i(DAT[ 3: 0]), .o(HEX0));

// ---------------------------------------------------------------------
wire clock_25, clock_100, reset_n;

assign LEDR[0] = err;

wire        err;
wire [ 7:0] kbd;
reg  [ 7:0] dat;
reg         cmd;

pll u0
(
    .clkin  (CLOCK_50),
    .locked (reset_n),
    .m25    (clock_25),
    .m100   (clock_100)
);

keyboard KB
(
    .clock      (clock_25),
    .reset_n    (reset_n),
    .ps_clk     (PS2_CLK),
    .ps_dat     (PS2_DAT),
    // ----------------
    .kbd        (kbd),
    .hit        (hit),
    // ----------------
    .cmd        (cmd),
    .dat        (dat),
    .err        (err),
    .idle       (idle),
    // ----------------
    .tmp        (tmp)
);

wire        idle;
wire [7:0]  tmp;
reg  [15:0] cm;

always @(posedge clock_25)
if (reset_n && RESET_N)
begin

    cmd <= 0;
    cm  <= cm + 1;

    case (cm)

    0: cm <= KEY[0] == 0 ? 1 : 0;

    1: if (idle) begin cmd <= 1; dat <= 8'hF4; end else cm <= 1; // ENABLE SCAN
    // 1: if (idle) begin cmd <= 1; dat <= 8'hF2; end else cm <= 1; // IDENTIFY
    // 1: if (idle) begin cmd <= 1; dat <= 8'hED; end else cm <= 1; // SET LEDS
    // 3: if (idle) begin cmd <= 1; dat <= 8'h03; end else cm <= 3;

    65535: cm <= 65535;
    endcase
    if (hit) DAT <= {DAT[23:0], kbd};

    // DAT[23:16] <= cm;
    // DAT[15:8] <= tmp;

end else cm <= 0;

endmodule

`include "../../keyboard.v"
