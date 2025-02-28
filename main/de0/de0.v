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

// ---------------------------------------------------------------------
wire clock_25, clock_100, reset_n;

pll u0
(
    .clkin  (CLOCK_50),
    .locked (reset_n),
    .m25    (clock_25),
    .m100   (clock_100)
);

// Центральный процессор -------------------------------------------------------

wire [15:0] address, pc, ir;
wire [ 7:0] data_i, data_o;
wire        we, read;
reg         intr;
reg  [ 2:0] vect;

avr AVR
(
    .clock      (clock_25),
    .reset_n    (reset_n),
    // Программная память
    .pc         (pc),
    .ir         (ir),
    // Оперативная память
    .address    (address),
    .data_i     (data_i),
    .data_o     (data_o),
    .we         (we),
    .read       (read),
    // Внешнее прерывание #0..7
    .intr       (intr),
    .vect       (vect)
);

// Видеопроцессор --------------------------------------------------------------

wire [15:0] video_a;
wire [ 3:0] video_i;
reg  [15:0] video_c, video_ca;
reg  [ 3:0] video_o, border;
reg         video_w;

video VIDEO
(
    .CLK        (clock_25),
    .R          (VGA_R),
    .G          (VGA_G),
    .B          (VGA_B),
    .HS         (VGA_HS),
    .VS         (VGA_VS),
    .VA         (video_a),
    .VI         (video_i),
    .BRD        (border),
);

// Контроллеры -----------------------------------------------------------------

always @(posedge clock_25)
if (reset_n == 0 || RESET_N == 0) begin

    border  <= 7;
    video_c <= 0;

end
else begin

    video_w <= 0;

    if (we)
    case (address)

    // Работа с видеопамятью
    16'h0020: video_c[ 7:0] <= data_o;
    16'h0021: video_c[15:8] <= data_o;
    16'h0022: begin

        video_ca <= video_c;
        video_c  <= video_c + 1;
        video_o  <= data_o[3:0];
        video_w  <= 1;

    end

    // Запись бордера
    16'h002D: border <= data_o[3:0];
    endcase

end

// Блоки памяти ----------------------------------------------------------------

// 128K ПЗУ
memprog M1
(
    .clock      (clock_100),
    .a          (pc),
    .q          (ir)
);

// 64K ОЗУ
memram M2
(
    .clock      (clock_100),
    .a          (address),
    .q          (data_i),
    .d          (data_o),
    .w          (we)
);

// 64K VRAM
memvideo M3
(
    // Графический контроллер
    .clock      (clock_100),
    .a          (video_a),
    .q          (video_i),

    // Запись и чтение из видеопамяти
    .ax         (video_ca),
    .dx         (video_o),
    .wx         (video_w)
);

endmodule

`include "../avr.v"
`include "../video.v"
