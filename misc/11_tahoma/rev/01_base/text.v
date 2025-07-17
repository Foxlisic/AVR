/* verilator lint_off WIDTH */

module text
(
    // Опорная частота 25 мгц
    input   wire        clock,

    // Выходные данные
    output  reg  [3:0]  r,       // 4 бит на красный
    output  reg  [3:0]  g,       // 4 бит на зеленый
    output  reg  [3:0]  b,       // 4 бит на синий
    output  wire        hs,      // горизонтальная развертка
    output  wire        vs,      // вертикальная развертка

    // Доступ к памяти
    output  reg  [11:0] char_addr, // 4k Текст
    output  reg  [10:0] font_addr, // 2k Шрифт 8x8
    input   wire [ 7:0] char_data,
    input   wire [ 7:0] font_data
);

// ---------------------------------------------------------------------
// Тайминги для горизонтальной|вертикальной развертки (640x400)
// ---------------------------------------------------------------------

localparam

    hz_visible = 640, vt_visible = 400,
    hz_front   = 16,  vt_front   = 12,
    hz_sync    = 96,  vt_sync    = 2,
    hz_back    = 48,  vt_back    = 35,
    hz_whole   = 800, vt_whole   = 449;

assign hs = x  < (hz_back + hz_visible + hz_front); // NEG.
assign vs = y >= (vt_back + vt_visible + vt_front); // POS.
// ---------------------------------------------------------------------
wire        xmax = (x == hz_whole - 1);
wire        ymax = (y == vt_whole - 1);
reg  [10:0] x    = 0;
reg  [10:0] y    = 0;
reg  [ 7:0] char;
reg  [ 7:0] attr;
wire [10:0] X    = x - hz_back + 8;     // X=[0..639]
wire [ 9:0] Y    = y - vt_back;         // Y=[0..399]
// ---------------------------------------------------------------------

// Вывод видеосигнала
always @(posedge clock) begin

    // Кадровая развертка
    x <= xmax ?         0 : x + 1;
    y <= xmax ? (ymax ? 0 : y + 1) : y;

    // Вывод окна видеоадаптера
    if (x >= hz_back && x < hz_visible + hz_back &&
        y >= vt_back && y < vt_visible + vt_back)
    begin
         {r, g, b} <= char[ ~X[2:0] ] ? 12'h0A0 : 12'h111;
    end
    else {r, g, b} <= 12'h000;

    case (X[2:0])

        0: begin char_addr <= X[9:3] + (Y[8:3] * 80); end
        3: begin font_addr <= {char_data[7:0], Y[2:0]}; end
        7: begin char      <= font_data; end

    endcase

end

endmodule
