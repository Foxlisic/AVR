/* verilator lint_off WIDTH */
/* verilator lint_off CASEINCOMPLETE */

module gpu
(
    // 53 Mhz
    input               clock,
    output  reg  [3:0]  R,
    output  reg  [3:0]  G,
    output  reg  [3:0]  B,
    output              HS,
    output              VS,
    // Подключено к 4K (charmap) 4K (font)
    input        [10:0] cursor,
    input        [ 7:0] char_data,
    input        [ 7:0] font_data,
    output  reg  [11:0] char_address,
    output  reg  [11:0] font_address
);

// ---------------------------------------------------------------------
// Тайминги для горизонтальной и вертикальной развертки
//        Visible     Front     Sync      Back      Whole
parameter hzv = 720,  hzf = 40, hzs = 76,  hzb = 116, hzw = 952,
          vtv = 900,  vtf = 1,  vts = 3,   vtb = 28,  vtw = 932;
// ---------------------------------------------------------------------
assign HS = X  < (hzb + hzv + hzf); // NEG
assign VS = Y >= (vtb + vtv + vtf); // POS
// ---------------------------------------------------------------------
// Позиция луча в кадре и максимальные позиции (x,y)
reg  [10:0] X = 0; wire xmax = (X == hzw - 1);
reg  [ 9:0] Y = 0; wire ymax = (Y == vtw - 1);
// Область рисования
wire [10:0] x = (X - hzb);
wire [ 9:0] y = (Y - vtb);
// Центрирование
wire [10:0] xb = (X - hzb - 32);
wire [10:0] yb = (Y - vtb - 50);
// ---------------------------------------------------------------------

// Временные значения маски и атрибутов
reg [ 7:0] char;
reg [ 7:0] data;
reg [ 7:0] attr;
reg [24:0] timer;
reg        flash;

// Положение символа
wire [10:0] pos = 80*yb[9:5] + xb[9:3];

// Рисуемый цвет
wire        maskbit = (data[ ~xb[2:0] ]) | (flash && (pos == cursor + 1) && yb[4:1] >= 14);
wire [ 3:0] selclr  = (maskbit ? (attr[7] & flash ? attr[6:4] : attr[3:0]) : attr[6:4]);

// Цвет символа
wire [11:0] color =

    // R(5 бит) G(6 бит) B(5 бит(
    selclr == 4'd0  ? {4'h0, 4'h0, 4'h0} : // 0 Черный
    selclr == 4'd1  ? {4'h0, 4'h0, 4'h5} : // 1 Синий (темный)
    selclr == 4'd2  ? {4'h0, 4'h7, 4'h0} : // 2 Зеленый (темный)
    selclr == 4'd3  ? {4'h0, 4'h7, 4'h5} : // 3 Бирюзовый (темный)
    selclr == 4'd4  ? {4'h7, 4'h0, 4'h0} : // 4 Красный (темный)
    selclr == 4'd5  ? {4'h7, 4'h0, 4'h5} : // 5 Фиолетовый (темный)
    selclr == 4'd6  ? {4'h7, 4'h7, 4'h0} : // 6 Коричневый
    selclr == 4'd7  ? {4'hA, 4'hA, 4'hA} : // 7 Светло-серый
    selclr == 4'd8  ? {4'h5, 4'h5, 4'h5} : // 8 Темно-серый
    selclr == 4'd9  ? {4'h0, 4'h0, 4'hF} : // 9 Синий
    selclr == 4'd10 ? {4'h0, 4'hF, 4'h0} : // 10 Зеленый
    selclr == 4'd11 ? {4'h0, 4'hF, 4'hF} : // 11 Бирюзовый
    selclr == 4'd12 ? {4'hF, 4'h0, 4'h0} : // 12 Красный
    selclr == 4'd13 ? {4'hF, 4'h0, 4'hF} : // 13 Фиолетовый
    selclr == 4'd14 ? {4'hF, 4'hF, 4'h0} : // 14 Желтый
                      {4'hF, 4'hF, 4'hF};  // 15 Белый

always @(posedge clock) begin

    // Черный цвет по краям
    {R, G, B} <= 12'h000;

    // Кадровая развертка
    X <= xmax ?         0 : X + 1;
    Y <= xmax ? (ymax ? 0 : Y + 1) : Y;

    // Вывод окна видеоадаптера
    if (X >= hzb && X < hzb + hzv && Y >= vtb && Y < vtb + vtv)
    begin
        {R, G, B} <= (x >= 40 && x < 680 && y >= 50 && y < 850) ? color : 12'h111;
    end

    // Знакогенератор
    case (x[2:0])
    3'd0: begin char_address <= {pos,        1'b0}; end
    3'd2: begin font_address <= {char_data,  yb[4:1]}; end
    3'd4: begin char_address <= {pos,        1'b1};
                char         <= font_data; end
    3'd7: begin data         <= char;
                attr         <= char_data; end
    endcase

    // Каждые 0,5 секунды перебрасывается регистр flash
    if (timer == 25000000) begin
        timer <= 0;
        flash <= ~flash;
    end else
        timer <= timer + 1;

end

endmodule
