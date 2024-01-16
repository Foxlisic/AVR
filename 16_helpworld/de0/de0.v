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
// Объявление проводов
// -----------------------------------------------------------------------------

// Провода
wire        clock_25, clock_100, locked;
wire [15:0] pc, ir;
wire [15:0] address;
wire [11:0] vga_a;
wire [ 7:0] mem_in, data_o, vga_d, vga_f, kb_data;
wire        we, read, kb_done;

// Регистры
reg         intr;
reg  [ 2:0] vect;
reg  [ 7:0] kb, cursor_x, cursor_y;

// -----------------------------------------------------------------------------
// Генерация частот
// -----------------------------------------------------------------------------

pll PLL
(
    .clkin      (CLOCK_50),
    .m25        (clock_25),
    .m100       (clock_100),
    .locked     (locked)
);

// -----------------------------------------------------------------------------
// Центральный процессор
// -----------------------------------------------------------------------------

avr CORE
(
    .clock      (clock_25),
    .reset_n    (locked),
    // Память программ
    .pc         (pc),
    .ir         (ir),
    // Память ОЗУ
    .address    (address),
    .data_i     (data_i),
    .data_o     (data_o),
    .we         (we),
    .read       (read),
    // Внешнее прерывание #0..7
    .intr       (intr),
    .vect       (vect)
);

// -----------------------------------------------------------------------------
// Адаптер
// -----------------------------------------------------------------------------

wire [11:0] cursor = cursor_x + cursor_y*80;

// Видеоадаптер
ga VIDEO
(
    .clock  (clock_25),
    // Вывод на экране
    .R      (VGA_R),
    .G      (VGA_G),
    .B      (VGA_B),
    .HS     (VGA_HS),
    .VS     (VGA_VS),
    // Связь с памятью
    .A      (vga_a),
    .D      (vga_d),
    .F      (vga_f),
    .cursor (cursor),
);

// -----------------------------------------------------------------------------
// Клавиатура
// -----------------------------------------------------------------------------

kb KBD
(
    .clock      (clock_25),
    .ps_clock   (PS2_CLK),
    .ps_data    (PS2_DAT),
    .done       (kb_done),
    .data       (kb_data)
);

// -----------------------------------------------------------------------------
// Контроллеры и роутер
// -----------------------------------------------------------------------------

// Роутер портов в памяти
wire [7:0] data_i =
    16'h0020 == address ? kb :
    16'h0021 == address ? cursor_x :
    16'h0022 == address ? cursor_y :
    mem_in;

// Запись в порты или прием данных от устройств
always @(posedge clock_25) begin

    // Получение скан-кода с клавиатуры
    if (kb_done) begin kb <= kb_data; vect <= 1'b0; intr <= ~intr; end

    // Запись в порты
    if (we) case (address)
    16'h0021: cursor_x <= data_o;
    16'h0022: cursor_y <= data_o;
    endcase

end

// Память
// -----------------------------------------------------------------------------
prog M0(.clock(clock_100), .a(pc[13:0]), .q(ir)); // 32K
comm M1(.clock(clock_100), .a({4'hF, vga_a}), .q(vga_d), .ax(address), .qx(mem_in), .dx(data_o), .wx(we)); // 64K
font M2(.clock(clock_100), .a(vga_a), .q(vga_f)); // 4K
// -----------------------------------------------------------------------------

endmodule

`include "../kb.v"
`include "../ga.v"
`include "../avr.v"
