module de0
(
    // Reset
    input              RESET_N,

    // Clocks
    input              CLOCK_50,
    input              CLOCK2_50,
    input              CLOCK3_50,
    input              CLOCK4_50,

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

// MISO: Input Port
assign SD_DATA[0] = 1'bZ;

// SDRAM Enable
assign DRAM_CKE  = 1;   // ChipEnable
assign DRAM_CS_N = 0;   // ChipSelect

// Z-state
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

// -----------------------------------------------------------------------------
// Генератор частот, 25, 50, 100 Mhz
// -----------------------------------------------------------------------------

wire locked;
wire clock_25;

pll PLL0
(
    .clkin     (CLOCK_50),
    .m25       (clock_25),
    .m50       (clock_50),
    .m100      (clock_100),
    .locked    (locked)
);

// -----------------------------------------------------------------------------
// Центральный процессор
// -----------------------------------------------------------------------------

wire [15:0] pc;                 // Адрес в памяти программ
wire [15:0] ir;                 // Прочитанные данные
wire [15:0] address;            // Адрес в общей памяти
wire [ 7:0] in;                 // Данные на вход в процессор
wire [ 7:0] out;                // Данные на выход из процессора
wire        we;                 // Запись в память
wire        read;               // Сигнал что данные были только что прочитаны

cpu C1
(
    .clock      (clock_25),
    .reset_n    (locked & RESET_N), // Возможность сброса по кнопке
    .intr       (1'b0),             // Изменение значения вызывает прерывание
    .vect       (3'h1),             // Вектор прерывания
    .pc         (pc),
    .ir         (ir),
    .address    (address),
    .data_i     (in),
    .data_o     (out),
    .we         (we),
    .read       (read)
);

// -----------------------------------------------------------------------------
// Модули памяти ROM/RAM
// -----------------------------------------------------------------------------

// Память программ mem_flash.mif (32k x 16)
mem_flash M1
(
    .clock      (clock_100),
    .address0   (pc),
    .out0       (ir)
);

// Память программ mem_ram.mif (64k x 8)
mem_ram M2
(
    .clock      (clock_100),
    // Обращение к процессору
    .address0   (address),
    .out0       (in),
    .in0        (out),
    .we0        (we),
    // Обращение к видеоадаптеру
    .address1   ({4'b1111, char_address}),
    .out1       (char_data)
);

// Шрифты 2K
mem_font M3
(
    .clock      (clock_100),
    .a0         (font_address),
    .q0         (font_data),
);

// -----------------------------------------------------------------------------
// Видеоадаптер
// -----------------------------------------------------------------------------

wire [11:0] char_address;
wire [11:0] font_address;
wire [ 7:0] char_data;
wire [ 7:0] font_data;

gpu T1
(
    // Опорная частота 25 мгц
    .clock  (clock_25),

    // Выходные данные
    .r      (VGA_R),
    .g      (VGA_G),
    .b      (VGA_B),
    .hs     (VGA_HS),
    .vs     (VGA_VS),

    // Доступ к памяти
    .char_address   (char_address),
    .font_address   (font_address),
    .char_data      (char_data),
    .font_data      (font_data)
);

endmodule

// Подключение модулей
`include "../cpu.v"
`include "../gpu.v"
