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

assign SD_DATA[0] = 1'bZ;

// Провода
// ---------------------------------------------------------------------
wire        clock_25, clock_100, reset_n;
// Процессор
wire [15:0] a, pc, ir;
wire [ 7:0] o, p;
wire [ 2:0] vect;
wire        w, r;
wire        intr;
// Клавиатура
wire        hit, kdone;
wire [ 7:0] kbd, ascii;
// Видеоадаптер
wire [12:0] vga_a;
wire [ 7:0] vga_i;
wire [ 2:0] vga_b;
wire        vblank;
wire        vpage;
// SD Card
wire [31:0] sd_lba;
wire [ 3:0] sd_error;
wire [ 1:0] sd_card;
wire [ 8:0] sd_a;
wire [ 7:0] sd_i, sd_o;
wire        sd_w, sd_rw, sd_busy, sd_done, sd_command;
// Роутер памяти
wire [ 7:0] i8, s8;
wire        m_sd = a >= 16'hFC00;
wire        m_pt = a <= 16'h005F;
wire [ 7:0] i = m_pt ? p : (m_sd ? s8 : i8);

// Генератор частоты
// -----------------------------------------------------------------------------

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
    // -- Периферия
    .p_vpage    (vpage),
    .p_border   (vga_b),
    .p_kdone    (kdone),
    .p_ascii    (ascii),
    .p_vblank   (vblank),
    // -- SD
    .sd_command (sd_command),
    .sd_rw      (sd_rw),
    .sd_lba     (sd_lba),
    .sd_card    (sd_card),
    .sd_error   (sd_error),
    .sd_done    (sd_done),
    .sd_busy    (sd_busy)
);

// Видеоадаптер
// -----------------------------------------------------------------------------

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
    .vblank     (vblank)
);

// Клавиатура
// -----------------------------------------------------------------------------

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

// Карта SDRAM
// -----------------------------------------------------------------------------

sd SD
(
    .clock      (clock_25),
    .reset_n    (reset_n),
    // Физический доступ
    .cs         (SD_DATA[3]),   // Выбор чипа
    .sclk       (SD_CLK),       // Тактовая частота
    .miso       (SD_DATA[0]),   // Входящие данные
    .mosi       (SD_CMD),       // Исходящие
    // Управление устройством
    .command    (sd_command),   // =1 Сигнал запуска
    .rw         (sd_rw),        // =0 Чтение =1 Запись
    .lba        (sd_lba),       // Запрошенный номер сектора с 0
    .busy       (sd_busy),      // =1 Устройство занято
    .done       (sd_done),      // Строб-сигнал DONE (1 такт)
    .error      (sd_error),     // =1 Ошибка доступа к SD
    .card       (sd_card),      // Тип карты 0,1,2,3=SDHC
    // Чтение или запись в память данных
    .a          (sd_a),
    .i          (sd_i),
    .o          (sd_o),
    .w          (sd_w)
);

// Модули памяти
// ---------------------------------------------------------------------

p32 ROM(.clock(clock_100), .a(pc[13:0]), .q(ir));
m64 RAM(.clock(clock_100), .a( a[15:0]), .q(i8), .d(o), .w(w),        .ax({2'b10, vpage, vga_a[12:0]}), .qx(vga_i));
m1  SDC(.clock(clock_100), .a( a[ 9:0]), .q(s8), .d(o), .w(w & m_sd), .ax(sd_a), .qx(sd_i), .wx(sd_w), .dx(sd_o));

endmodule

`include "../avr.v"
`include "../io.v"
`include "../kb.v"
`include "../sd.v"
