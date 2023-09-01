`timescale 10ns / 1ns

module avr;

// ---------------------------------------------------------------------
// Симулятор Verilog Icarus
// ---------------------------------------------------------------------

reg clock  = 0;
reg clk50  = 0;
reg clklo  = 0;
reg locked = 0;

always #0.5 clock = ~clock;
always #1.0 clk50 = ~clk50;
always #1.5 clklo = ~clklo;

initial begin clock = 0; clk50 = 1; clklo = 0; #3.0 locked = 1; #2000 $finish; end
initial begin $dumpfile("tb.vcd"); $dumpvars(0, avr); end

// ---------------------------------------------------------------------
// Интерфейс для работы с памятью (чтение и запись)
// ---------------------------------------------------------------------

reg [15:0] ROM[ 65536]; // 128k
reg [ 7:0] RAM[131072]; // 128k

initial $readmemh("mem_core.hex", ROM, 16'h0000);
initial $readmemh("mem_sram.hex", RAM, 16'h0000);

// ---------------------------------------------------------------------
// Основной интерфейс ядра
// ---------------------------------------------------------------------

wire [15:0] pc;         // Программный счетчик
reg  [15:0] ir;         // Регистр инструкции (текущий опкод)
wire [15:0] address;    // Адрес в памяти SRAM
reg  [ 7:0] data_i;     // Прочтенные данные из SRAM
wire [ 7:0] data_o;     // Данные для записи в SRAM
wire        wren;       // Разрешение на запись в память

// ---------------------------------------------------------------------
// Контроллер внутрисхемной памяти
// ---------------------------------------------------------------------

always @(posedge clock) begin

    ir     <= ROM[pc];
    data_i <= RAM[address];
    if (wren) RAM[address] <= data_o;

end

// ---------------------------------------------------------------------
// Центральный процессорный блок
// ---------------------------------------------------------------------

avrcpu ModuleCPU
(
    // Тактовый генератор
    .clock      (clklo & locked),
    .reset_n    (1'b1),

    // Программная память
    .pc         (pc),           // Программный счетчик
    .ir         (ir),           // Инструкция из памяти

    // Оперативная память
    .address    (address),      // Указатель на память RAM (sram)
    .data_i     (data_i),       // memory[ address ]
    .data_o     (data_o),       // Запись в память по address
    .wren       (wren)          // Разрешение записи в память
);


endmodule
