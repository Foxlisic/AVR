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
    .intr       (kb_intr),          // Изменение значения вызывает прерывание
    .vect       (3'h1),             // Вектор прерывания
    .pc         (pc),
    .ir         (ir),
    .address    (address),
    .data_i     (din),              // Через роутер памяти
    .data_o     (out),
    .we         (we),
    .read       (read)
);

// -----------------------------------------------------------------------------
// Контроллер портов
// -----------------------------------------------------------------------------

reg [17:0]  address_cu;
reg         font_we;
reg         himm_we;
reg [ 3:0]  himm_in;
reg [ 7:0]  font_in;
reg [ 7:0]  font_out;
reg [11:0]  font_addr_we;
reg [17:0]  himm_address;

// Роутер памяти
wire [7:0]  din =
    address == 16'h20 ? kb_data :
    address == 16'h21 ? (vidmod ? himm_in : font_in) :
    address == 16'h22 ? spi_din :
    address == 16'h23 ? {spi_st[0], spi_st[1], 6'b000000} :
    in;

always @(posedge clock_25)
begin

    font_we  <= 1'b0;
    himm_we  <= 1'b0;
    spi_sent <= 1'b0;

    if (we) case (address)

    // Запись в порт $10 пишет по адресу font_addr_cu и делается приращение +1 после каждой записи
    16'h20: begin

        address_cu   <= address_cu + 1;
        font_we      <= (vidmod == 0);
        himm_we      <= (vidmod == 1) && (out < 8'h10);
        himm_address <= address_cu[17:0];
        font_addr_we <= address_cu[11:0];
        font_out     <= out;

    end

    // Установка курсора для font/hi
    16'h21: begin address_cu <= {address_cu[9:0], out}; end

    // SPI: Отсылка данных и команды
    16'h22: begin spi_out <= out; end
    16'h23: begin spi_cmd <= out[1:0]; spi_sent <= 1'b1; end

    // Установка позиции курсора
    16'h24: begin cursor <= {cursor[2:0], out}; end

    // Видеорежим 0-text, 1-graphics
    16'h25: begin vidmod <= out[0]; end

    endcase

end

// Зафиксировать новую клавишу
always @(posedge clock_25) if (ps2_hit) begin kb_data <= ps2_data; kb_intr <= ~kb_intr; end

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
    // Запись в память шрифтов
    .a1         (font_addr_we),
    .q1         (font_in),
    .d1         (font_out),
    .w1         (font_we)
);

// 128K видеопамять
mem_hivid M4
(
    .clock      (clock_100),
    .a0         (hvid_address),
    .q0         (hvid_data),
    // Запись в память шрифтов
    .a1         (himm_address),
    .q1         (himm_in),
    .d1         (font_out),
    .w1         (himm_we)
);

// -----------------------------------------------------------------------------
// Видеоадаптер
// -----------------------------------------------------------------------------

reg         vidmod;
wire [11:0] char_address, font_address;
reg  [10:0] cursor;
wire [ 7:0] char_data;
wire [ 7:0] font_data;
wire [17:0] hvid_address;
wire [ 3:0] hvid_data;

gpu T1
(
    // Опорная частота 25 мгц
    .clock  (clock_25),
    .mode   (vidmod),

    // Выходные данные
    .r      (VGA_R),
    .g      (VGA_G),
    .b      (VGA_B),
    .hs     (VGA_HS),
    .vs     (VGA_VS),

    // Доступ к памяти
    .char_address   (char_address),
    .font_address   (font_address),
    .himm_address   (hvid_address),
    .char_data      (char_data),
    .font_data      (font_data),
    .himm_data      (hvid_data),
    .cursor         (cursor)
);


// КЛАВИАТУРА
// -----------------------------------------------------------------------------

wire [7:0]  ps2_data;
wire        ps2_hit;
reg  [7:0]  kb_data;
reg         kb_intr;

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
    .spi_st     (spi_st)       // bit 0: busy (1); bit 1: timeout
);
// -----------------------------------------------------------------------------

endmodule

// Подключение модулей
`include "../cpu.v"
`include "../gpu.v"
`include "../ps2.v"
`include "../sdcard.v"
