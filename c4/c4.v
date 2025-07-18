module c4
(
    input           RESET_N,
    input           CLOCK,          // 50 MHZ
    input   [3:0]   KEY,
    output  [3:0]   LED,
    output          BUZZ,           // Пищалка
    input           RX,             // Прием
    output          TX,             // Отправка
    output          SCL,            // Температурный сенсор :: LM75
    inout           SDA,
    output          I2C_SCL,        // Память 1Кб :: AT24C08
    inout           I2C_SDA,
    output          PS2_CLK,
    inout           PS2_DAT,
    input           IR,             // Инфракрасный приемник
    output          VGA_R,
    output          VGA_G,
    output          VGA_B,
    output          VGA_HS,
    output          VGA_VS,
    output  [ 3:0]  DIG,            // 4x8 Семисегментный
    output  [ 7:0]  SEG,
    inout   [ 7:0]  LCD_D,          // LCD экран
    output          LCD_E,
    output          LCD_RW,
    output          LCD_RS,
    inout   [15:0]  SDRAM_DQ,
    output  [11:0]  SDRAM_A,        // Адрес
    output  [ 1:0]  SDRAM_B,        // Банк
    output          SDRAM_RAS,      // Строка
    output          SDRAM_CAS,      // Столбце
    output          SDRAM_WE,       // Разрешение записи
    output          SDRAM_L,        // LDQM
    output          SDRAM_U,        // UDQM
    output          SDRAM_CKE,      // Активация тактов
    output          SDRAM_CLK,      // Такты
    output          SDRAM_CS        // Выбор чипа (=0)
);

assign BUZZ = 1'b1;
assign DIG  = 4'b1111;
assign LED  = 4'b1111;

// Провода
// -----------------------------------------------------------------------------
wire        clock_25, clock_100, locked;
wire [15:0] a, pc, ir;
wire [ 7:0] o, p;
wire        w, r;
reg         intr;
reg  [ 2:0] vect;
wire [ 7:0] ascii;
wire        kdone;
wire [ 7:0] i8, i4, i2;
wire [ 7:0] i = pt ? p : w8 ? i8 : w4 ? i4 : w2 ? i2 : 8'h00;
wire [12:0] vga_a;
wire [ 7:0] vga_i;
reg  [ 2:0] vga_b;

// Роутер памяти
// -----------------------------------------------------------------------------
wire pt = a <= 16'h005F;
wire w8 = a >= 16'hB000 && a < 16'hD000; // 8K  VRAM
wire w4 = a >= 16'h0000 && a < 16'h1000; // 4Kb CRAM
wire w2 = a >= 16'h1000 && a < 16'h1800; // 2Kb Stack

// Частотная генерация
// -----------------------------------------------------------------------------
pll UPLL
(
    .clock      (CLOCK),
    .c0         (clock_25),
    .c1         (clock_100),
    .locked     (locked)
);

// Процессор
// -----------------------------------------------------------------------------
avr AVR
(
    .clock      (clock_25),
    .reset_n    (locked & RESET_N),
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

// -----------------------------------------------------------------------------
io IO
(
    .clock      (clock_25),
    .a          (a),
    .o          (o),
    .p          (p),
    .w          (w),
    .p_border   (vga_b),
    .p_kdone    (kdone),
    .p_ascii    (ascii)
);

// Видеоадаптер
// -----------------------------------------------------------------------------
vga UVGA
(
    .clock      (clock_25),
    .r          (VGA_R),
    .g          (VGA_G),
    .b          (VGA_B),
    .hs         (VGA_HS),
    .vs         (VGA_VS),
    .a          (vga_a),
    .i          (vga_i),
    .border     (vga_b)
);

// Клавиатура
// -----------------------------------------------------------------------------
kb KB
(
    .clock      (clock_25),
    .reset_n    (locked),
    .ps_clk     (PS2_CLK),
    .ps_dat     (PS2_DAT),
    .ascii      (ascii),
    .kdone      (kdone)
);

// Модули памяти
// -----------------------------------------------------------------------------

m32 ROM(.clock(clock_100), .a(pc[13:0]), .q(ir));
m2  STK(.clock(clock_100), .a( a[10:0]), .q(i2), .d(o), .w(w2 & w));
m4  RAM(.clock(clock_100), .a( a[11:0]), .q(i4), .d(o), .w(w4 & w));
m8  VID(.clock(clock_100), .a( a[12:0]), .q(i8), .d(o), .w(w8 & w), .ax(vga_a), .qx(vga_i));

endmodule

`include "../avr.v"
`include "../kb.v"
