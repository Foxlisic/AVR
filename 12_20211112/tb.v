`timescale 10ns / 1ns
module tb;

reg clock;
reg clock_25;
reg clock_50;
// ---------------------------------------------------------------------
always #0.5 clock    = ~clock;
always #1.0 clock_50 = ~clock_50;
always #2.0 clock_25 = ~clock_25;
// ---------------------------------------------------------------------
initial begin clock = 0; clock_25 = 0; clock_50 = 0; #2000 $finish; end
initial begin $dumpfile("tb.vcd"); $dumpvars(0, tb); end
// ---------------------------------------------------------------------
reg  [15:0] progmem[65536];
reg  [ 7:0] memdata[65536];
// ---------------------------------------------------------------------
initial begin $readmemh("tb.hex", progmem, 0); end
// ---------------------------------------------------------------------
wire [15:0] pc;
wire [15:0] address;
reg  [15:0] ir;
reg  [ 7:0] data_i;
wire [ 7:0] data_o;
wire        we;
// ---------------------------------------------------------------------

// Контроллер блочной памяти
always @(posedge clock) begin

    data_i <= memdata[ address ];
    ir     <= progmem[ pc      ];

    if (we) memdata[ address ] <= data_o;

end


// ---------------------------------------------------------------------
// Центральный процессорный блок
// ---------------------------------------------------------------------

core AVRCore
(
    // Тактовый генератор
    .clock      (clock_25),
    .locked     (locked),

    // Программная память
    .pc         (pc),           // Программный счетчик
    .ir         (ir),           // Инструкция из памяти

    // Оперативная память
    .address    (address),      // Указатель на память RAM (sram)
    .data_i     (data_i),       // memory[ address ]
    .data_o     (data_o),       // Запись в память по address
    .wren       (we)            // Разрешение записи в память
);

endmodule
