`timescale 10ns / 1ns
module tb;

// -----------------------------------------------------------------------------
reg clock;
reg clock_25;
reg clock_50;
// -----------------------------------------------------------------------------
always #0.5 clock    = ~clock;
always #1.0 clock_50 = ~clock_50;
always #2.0 clock_25 = ~clock_25;
// -----------------------------------------------------------------------------
initial begin clock = 0; clock_25 = 0; clock_50 = 0; #2000 $finish; end
initial begin $dumpfile("tb.vcd"); $dumpvars(0, tb); end
initial begin $readmemh("tb.hex", program_data, 0); end

// Небольшой контроллер внутренней дистрибутивной памяти
// -----------------------------------------------------------------------------

reg  [15:0] program_data[65536];    // 128k памяти flash
reg  [ 7:0] ram_data    [65536];    // 64kb

// Данные программы и адрес программы во флеш-памяти
wire [15:0] data_flash = program_data[ address_flash_ff ];                 
wire [ 7:0] data_ram   = ram_data[ address_ram_ff ];

reg  [15:0] address_flash_ff;
reg  [15:0] address_ram_ff;
wire [15:0] address_flash;
wire [15:0] address_ram;
wire [ 7:0] data_write;
wire        we;

always @(posedge clock) begin

    address_flash_ff <= address_flash;
    address_ram_ff   <= address_ram;
    
    if (we) ram_data[ address_ram ] <= data_write;

end

// Интерфейс процессора
// -----------------------------------------------------------------------------

avr AVRCoreUnit
(
    .clock      (clock_25),
    .locked     (1'b1),
    .pdata      (data_flash),
    .rdata      (data_ram),
    .pc         (address_flash),
    .address    (address_ram),
    .dataw      (data_write),
    .we         (we)
);

endmodule
