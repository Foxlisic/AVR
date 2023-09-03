module c4
(
    // Core Board
    input           CLOCK_50,
    input   [1:0]   KEY,
    output  [1:0]   LED,

    // Адаптер VGA 16 BIT
    output  [4:0]   VGA_R,
    output  [5:0]   VGA_G,
    output  [4:0]   VGA_B,
    output          VGA_HS,
    output          VGA_VS,

    // Кнопки, индикаторы дочерней платы
    input   [4:0]   DB_KEY,
    output  [4:0]   DB_LED,
    output  [7:0]   SEMI,
    output  [2:0]   SEN,

    // SDRAM 32 MB
    output  [12:0]  DRAM_A,         // Address
    output  [ 1:0]  DRAM_B,         // Bank
    inout   [15:0]  DRAM_D,         // Data
    output          DRAM_CS,        // Chip Select
    output          DRAM_CKE,       // Clock Enable
    output          DRAM_CLK,       // DRAM Clock
    output          DRAM_LDQM,      // Low Data Mask
    output          DRAM_HDQM,      // High Data Mask
    output          DRAM_CAS,       // Column Access
    output          DRAM_RAS,       // Row Access
    output          DRAM_WE,        // Write Enable

    // Относительно чипа CYCLONE
    output          TX,
    input           RX
);

assign LED          = 2'b11;        // Основная
assign DB_LED       = 5'b11111;     // Дочерняя
assign SEN          = 3'b000;       // Все выключены
assign SEMI         = 8'b11111111;  // Семисегментный

// SDRAM Configuration
assign DRAM_D       = 16'hz;
assign DRAM_CKE     = 1'b0;     // 1=Enable [0=Disable]
assign DRAM_CS      = 1'b1;     // 0=Enable [1=Disable]
assign DRAM_LDQM    = 1'b1;     // Disable
assign DRAM_HDQM    = 1'b1;     // Disable
assign DRAM_CAS     = 1'b1;     // NOP
assign DRAM_RAS     = 1'b1;
assign DRAM_WE      = 1'b1;

// Генератор частот
// --------------------------------------------------------------

wire locked;
wire clock_25;
wire clock_50;
wire clock_100;

pll PLL0
(
    .clock      (CLOCK_50),
    .clock_25   (clock_25),
    .clock_50   (clock_50),
    .clock_100  (clock_100),
    .locked     (locked)
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
    .reset_n    (locked),       // Возможность сброса по кнопке
    .intr       (1'b0),         // Изменение значения вызывает прерывание
    .vect       (3'h1),         // Вектор прерывания
    .pc         (pc),
    .ir         (ir),
    .address    (address),
    .data_i     (in),
    .data_o     (out),
    .we         (we),
    .read       (read)
);

// -----------------------------------------------------------------------------
// Контроллер памяти
// -----------------------------------------------------------------------------

reg [15:0]  gpu_address;
reg [15:0]  gpu_address_w;
reg [ 7:0]  gpu_out;
reg         gpu_we;

always @(posedge clock_25)
begin

    gpu_we <= 1'b0;

    if (we)
    case (address)
    16'h20: gpu_address  <= {gpu_address[7:0], out};
    16'h21: video_bank_r <= out[0];
    16'h22: video_bank_w <= out[0];
    16'h23: begin gpu_address_w <= gpu_address; gpu_address <= gpu_address + 1'b1; gpu_we <= 1'b1; gpu_out <= out; end
    endcase

end

// -----------------------------------------------------------------------------
// Модули памяти ROM/RAM
// -----------------------------------------------------------------------------

// Память программ mem_flash.mif (32k x 16)
mem_flash M1
(
    .clock      (clock_100),
    .address_a  (pc),
    .q_a        (ir)
);

// Память программ mem_ram.mif (64k x 8)
mem_ram M2
(
    .clock      (clock_100),
    .address_a  (address),
    .data_a     (out),
    .q_a        (in),
    .wren_a     (we)
);

// Видеопамять mem_vid.mif (64k x 2)
mem_vid M3
(
    .clock      (clock_100),
    // Чтение
    .address_a  ({video_bank_r, video_address}),
    .q_a        (video_data),
    // Запись
    .address_b  ({video_bank_w, gpu_address_w}),
    .data_b     (gpu_out),
    .wren_b     (gpu_we)
);

// -----------------------------------------------------------------------------
// Видеоадаптер
// -----------------------------------------------------------------------------

reg         video_bank_r;
reg         video_bank_w;
wire [15:0] video_address;
wire [ 7:0] video_data;

gpu T1
(
    // Опорная частота 25 мгц
    .clock      (clock_25),

    // Выходные данные
    .r          (VGA_R[4:1]),
    .g          (VGA_G[5:2]),
    .b          (VGA_B[4:1]),
    .hs         (VGA_HS),
    .vs         (VGA_VS),

    // Входящие данные
    .address    (video_address),
    .data       (video_data)
);

endmodule

// Подключение модулей
`include "../cpu.v"
`include "../gpu.v"
