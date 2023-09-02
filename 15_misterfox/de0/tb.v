`timescale 10ns / 1ns
module tb;

// ---------------------------------------------------------------------
reg clock;
reg clock_25;
reg reset_n;

always #0.5 clock    = ~clock;
always #2.0 clock_25 = ~clock_25;
// ---------------------------------------------------------------------
initial begin reset_n = 0; clock = 0; clock_25 = 0; #3 reset_n = 1; #2000 $finish; end
initial begin $dumpfile("tb.vcd"); $dumpvars(0, tb); end
// ---------------------------------------------------------------------
reg  [15:0] pgm[65536];
reg  [ 7:0] mem[128*1024];

always @(posedge clock) begin

    ir      <= pgm[pc];
    data_i  <= mem[address];

    if (we) mem[address] <= data_o;

end

// ---------------------------------------------------------------------

reg         intr = 1'b0;
reg  [ 2:0] vect = 3'h0;
wire [15:0] pc, address;
reg  [15:0] ir;
reg  [ 7:0] data_i;
wire [ 7:0] data_o;
wire        we, read;

core COREAVR
(
    .clock      (clock_25),
    .reset_n    (reset_n),
    // Программная память
    .pc         (pc),          // Программный счетчик
    .ir         (ir),          // Инструкция из памяти
    // Оперативная память
    .address    (address),     // Указатель на память RAM (sram)
    .data_i     (data_i),      // = memory[ address ]
    .data_o     (data_o),      // Запись в память по address
    .we         (we),          // Разрешение записи в память
    // Внешнее прерывание #0..7
    .intr       (intr),
    .vect       (vect),
    // Чтение из памяти
    .read       (read)
);

endmodule
