/* verilator lint_off WIDTH */

module gpu
(
    // Опорная частота 25 мгц
    input   wire        clock,

    // Выходные данные
    output  reg  [3:0]  r,       // 4 бит на красный
    output  reg  [3:0]  g,       // 4 бит на зеленый
    output  reg  [3:0]  b,       // 4 бит на синий
    output              hs,      // горизонтальная развертка
    output              vs,      // вертикальная развертка
    // Видеопамять
    output  reg [15:0]  address,
    input       [ 7:0]  data
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
wire [10:0] X    = x - hz_back + 0; // X=[0..639]
wire [ 9:0] Y    = y - vt_back;     // Y=[0..399]
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

        case (X[0])
        0: address <= X[9:1] + Y[8:1]*320;
        1: {r, g, b} <= {/*R*/ data[7:5],1'b0, /*G*/ data[4:2],1'b0, /*B*/ data[1:0],2'b00};
        endcase

    end
    else {r, g, b} <= 12'h000;

end

endmodule
