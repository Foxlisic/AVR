module ga
(
    input   wire        clock,
    output  reg [3:0]   R,
    output  reg [3:0]   G,
    output  reg [3:0]   B,
    output              HS,
    output              VS,

    // Доступ к памяти
    output  reg  [11:0] A,      // Видеоданные [0000..0FFF]
    input        [ 7:0] D,      // Знакоместо
    input        [ 7:0] F,      // Шрифт

    // Внешний интерфейс
    input        [10:0] cursor, // Положение курсора от 0 до 2047
    output  reg  [ 7:0] vsync   // Количество переключений VSYNC
);

// ---------------------------------------------------------------------
// Тайминги для горизонтальной|вертикальной развертки (640x400)
// ---------------------------------------------------------------------
parameter
    hz_visible = 640, vt_visible = 400,
    hz_front   = 16,  vt_front   = 12,
    hz_sync    = 96,  vt_sync    = 2,
    hz_back    = 48,  vt_back    = 35,
    hz_whole   = 800, vt_whole   = 449;
// ---------------------------------------------------------------------
assign HS = x  < (hz_back + hz_visible + hz_front); // NEG.
assign VS = y >= (vt_back + vt_visible + vt_front); // POS.
// ---------------------------------------------------------------------
wire        xmax = (x == hz_whole - 1);
wire        ymax = (y == vt_whole - 1);
reg  [10:0] x    = 0;
reg  [10:0] y    = 0;
wire [10:0] X    = x - hz_back + 8; // X=[0..639]
wire [ 9:0] Y    = y - vt_back;     // Y=[0..399]
// ---------------------------------------------------------------------
// Текстовый видеоадаптер
// ---------------------------------------------------------------------
reg         flash;
reg  [23:0] timer;
reg  [ 7:0] char, attr, temp;
// ---------------------------------------------------------------------
wire [10:0] id = X[9:3] + (Y[8:4] * 80);
wire        maskbit = (char[~X[2:0]]) | (flash && (id == cursor+1) && Y[3:0] >= 14);
wire [ 3:0] color = maskbit ? (attr[7] & flash ? attr[6:4] : attr[3:0]) : attr[6:4];
wire [15:0] dst =
    color == 4'h0 ? 12'h111 : color == 4'h8 ? 12'h888 :
    color == 4'h1 ? 12'h008 : color == 4'h9 ? 12'h00f :
    color == 4'h2 ? 12'h080 : color == 4'hA ? 12'h0f0 :
    color == 4'h3 ? 12'h088 : color == 4'hB ? 12'h0ff :
    color == 4'h4 ? 12'h800 : color == 4'hC ? 12'hf00 :
    color == 4'h5 ? 12'h808 : color == 4'hD ? 12'hf0f :
    color == 4'h6 ? 12'h880 : color == 4'hE ? 12'hff0 :
    color == 4'h7 ? 12'hccc :                 12'hfff;

// Вывод видеосигнала
always @(posedge clock) begin

    {R, G, B} <= 12'h000;

    // Кадровая развертка
    x <= xmax ?         0 : x + 1;
    y <= xmax ? (ymax ? 0 : y + 1) : y;

    case (X[2:0])
    0: begin A    <= {id[10:0], 1'b0}; end
    1: begin temp <= D; A[0] <= 1'b1; end
    2: begin temp <= D; A    <= {temp[7:0], Y[3:0]}; end
    7: begin attr <= temp; char <= F; end
    endcase

    // Таймер для мигания курсора
    if (timer == 12500000) begin flash <= ~flash; timer <= 0; end else timer <= timer + 1;

    // Как только завершается рисование кадра, то инкрементируется vsync
    if (x == 0 && y == vt_visible + vt_back) vsync <= vsync + 1;

    // Вывод окна видеоадаптера
    if (x >= hz_back && x < hz_visible + hz_back && y >= vt_back && y < vt_visible + vt_back)
        {R, G, B} <= dst;
end

endmodule
