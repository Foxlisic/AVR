module de0(

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

// MISO: Input Port
assign SD_DATA[0] = 1'bZ;

// SDRAM Enable
assign DRAM_CKE  = 1; // ChipEnable
assign DRAM_CS_N = 0; // ChipSelect

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

// Генерация частот
wire locked;
wire clock_25;
wire clock_100;

// ---------------------------------------------------------------------

de0pll unit_pll
(
    .clkin     (CLOCK_50),
    .m25       (clock_25),
    .m100      (clock_100),
    .locked    (locked)
);

// ---------------------------------------------------------------------

wire [15:0] pc;
wire [15:0] ir;
wire [15:0] address;
wire [ 7:0] data_i;
wire [ 7:0] data_o;
wire        we;
wire        read;

wire [15:0] VGA_A;
wire [ 7:0] VGA_D;
reg  [ 2:0] VGA_BORDER;

wire [7:0]  ps2_data;
wire        ps2_hit;
reg  [7:0]  kb_data;
reg         kb_latch;

reg  [7:0]  banknum;

// ЦЕНТРАЛЬНЫЙ ПРОЦЕССОР
// ---------------------------------------------------------------------

core C1
(
    .clock      (clock_25),
    .reset_n    (locked),
    .intr       (1'b0),
    .vect       (3'h1),
    .pc         (pc),
    .ir         (ir),
    .address    (address),
    .data_i     (din),
    .data_o     (data_o),
    .we         (we),
    .read       (read),
);

// РОУТЕР ПАМЯТИ
// -----------------------------------------------------------------------------

wire [7:0] din =
    address == 8'h20 ? banknum :
    address == 8'h21 ? kb_data :
    // Бит 0: Keyboard latch (1)
    // Бит 1: SPI busy       (2)
    // Бит 2: SPI timeout    (4)
    address == 8'h22 ? {spi_st[1:0], kb_latch} :
    address == 8'h23 ? spi_din :
    data_i;

always @(posedge clock_25)
begin

    // Фиксирование нажатия на клавишу
    if (ps2_hit) begin

        kb_data  <= ps2_data;
        kb_latch <= 1;

    end

    if (we)
    case (address)
    16'h20: banknum     <= data_o;
    16'h21: VGA_BORDER  <= data_o;
    16'h22: kb_latch    <= 0;
    16'h23: spi_out     <= data_o;
    // Отсылка команды на SPI и защелка
    16'h24: {spi_sent, spi_cmd} <= {data_o[7], data_o[1:0]};
    endcase

end

// Организация памяти 128K+64K
// -----------------------------------------------------------------------------
// 32K для 320x200 видеопамять
// -----------------------------------------------------------------------------

// 128K
flash flash_unit
(
    .clock  (clock_100),
    .a0     (pc),
    .q0     (ir),
);

// 64K RAM
memory memory_unit
(
    // Процессор
    .clock  (clock_100),
    .a0     (address),
    .q0     (data_i),
    .d0     (data_o),
    .w0     (we),
    // Видеопамять
    .a1     (VGA_A),
    .q1     (VGA_D),
);

// ВИДЕОАДАПТЕР
// -----------------------------------------------------------------------------

vga unit_vga
(
    .CLOCK  (clock_25),
    .R      (VGA_R),
    .G      (VGA_G),
    .B      (VGA_B),
    .HS     (VGA_HS),
    .VS     (VGA_VS),
    .A      (VGA_A),
    .D      (VGA_D),
    .BORDER (VGA_BORDER),
);

// КЛАВИАТУРА
// -----------------------------------------------------------------------------

ps2 ps2_inst
(
    .clock      (clock_25),
    .ps_clock   (PS2_CLK),
    .ps_data    (PS2_DAT),
    .done       (ps2_hit),
    .data       (ps2_data)
);

// КОНТРОЛЛЕР SPI
// ---------------------------------------------------------------------

reg         spi_sent;
reg  [1:0]  spi_cmd;
reg  [7:0]  spi_out;
wire [1:0]  spi_st;
wire [7:0]  spi_din;

sdcard sdcard_unit
(
    // 25 Mhz
    .clock      (clock_25),
    .reset_n    (locked),

    // Физический интерфейс
    .spi_cs     (SD_DATA[3]),  // Выбор чипа
    .spi_sclk   (SD_CLK),      // Тактовая частота
    .spi_miso   (SD_DATA[0]),  // Входящие данные
    .spi_mosi   (SD_CMD),      // Исходящие

    // Интерфейс
    .spi_sent   (spi_sent),    // =1 Сообщение отослано на spi
    .spi_cmd    (spi_cmd),     // Команда
    .spi_din    (spi_din),     // Принятое сообщение
    .spi_out    (spi_out),     // Сообщение на отправку
    .spi_st     (spi_st)       // bit 0: timeout (1); bit 1: chip select 0/1
);
// -----------------------------------------------------------------------------


endmodule

`include "../core.v"
`include "../vga.v"
`include "../ps2.v"
`include "../sdcard.v"
