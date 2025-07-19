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
assign GPIO_1  = 36'hzzzzzzzz;

// LED OFF
assign HEX0 = 7'b1111111;
assign HEX1 = 7'b1111111;
assign HEX2 = 7'b1111111;
assign HEX3 = 7'b1111111;
assign HEX4 = 7'b1111111;
assign HEX5 = 7'b1111111;

// Провода
// ---------------------------------------------------------------------
wire        clock_25, clock_100, reset_n;
// Процессор
wire [15:0] a, pc, ir;
wire [ 7:0] o, p;
wire [ 2:0] vect;
wire        w, r;
wire        intr;
wire [ 7:0] i8;
wire [ 7:0] i = pt ? p : i8;
// Клавиатура
wire        hit, kdone;
wire [ 7:0] kbd, ascii;
// Видеоадаптер
wire [12:0] vga_a;
wire [ 7:0] vga_i;
wire [ 2:0] vga_b;
wire        vblank;
// Роутер памяти
wire pt = a <= 16'h005F;

// Генератор частоты
// ---------------------------------------------------------------------
pll u0
(
    .clkin      (CLOCK_50),
    .locked     (reset_n),
    .m25        (clock_25),
    .m100       (clock_100)
);

// Процессор
// -----------------------------------------------------------------------------
avr AVR
(
    .clock      (clock_25),
    .reset_n    (reset_n & RESET_N),
    .ce         (1'b1),
    // Программная память
    .pc         (pc),
    .ir         (ir),
    // Оперативная память
    .address    (a),
    .data_i     (i),
    .data_o     (o),
    .we         (w),
    .read       (r),
    // Прерывание #0..7
    .intr       (intr),
    .vect       (vect)
);

// Видеоадаптер
// ---------------------------------------------------------------------
vga A1
(
    .clock      (clock_25),
    .r          (VGA_R),
    .g          (VGA_G),
    .b          (VGA_B),
    .hs         (VGA_HS),
    .vs         (VGA_VS),
    .a          (vga_a),
    .i          (vga_i),
    .border     (vga_b),
    .vretrace   (vblank)
);

// Связь процессора с периферией
// -----------------------------------------------------------------------------
io IO
(
    .clock      (clock_25),
    .a          (a),
    .o          (o),
    .p          (p),
    .r          (r),
    .w          (w),
    .p_border   (vga_b),
    .p_kdone    (kdone),
    .p_ascii    (ascii)
);

// Клавиатура
// ---------------------------------------------------------------------
kb K1
(
    .clock      (clock_25),
    .reset_n    (reset_n),
    .ps_clk     (PS2_CLK),
    .ps_dat     (PS2_DAT),
    .hit        (hit),
    .kbd        (kbd),
    .kdone      (kdone),
    .ascii      (ascii)
);

// Модули памяти
// ---------------------------------------------------------------------

p32 ROM(.clock(clock_100), .a(pc[13:0]), .q(ir));
m64 RAM(.clock(clock_100), .a( a[15:0]), .q(i8), .d(o), .w(w), .ax({3'b100, vga_a[12:0]}), .qx(vga_i));

endmodule

`include "../avr.v"
`include "../io.v"
`include "../kb.v"
