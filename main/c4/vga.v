module vga
(
    input               clock,
    output  reg         r,
    output  reg         g,
    output  reg         b,
    output              hs,
    output              vs
);

// ---------------------------------------------------------------------
// Тайминги для горизонтальной и вертикальной развертки
parameter
//  Visible     Front       Sync        Back        Whole
    hzv =  640, hzf =   16, hzs =   96, hzb =   48, hzw =  800,
    vtv =  400, vtf =   12, vts =    2, vtb =   35, vtw =  449;
// ---------------------------------------------------------------------
assign hs = X <  (hzb + hzv + hzf);
assign vs = Y >= (vtb + vtv + vtf);
// ---------------------------------------------------------------------
// Позиция луча в кадре и максимальные позиции (x,y)
reg  [ 9:0] X = 0; wire xmax = (X == hzw - 1);
reg  [ 9:0] Y = 0; wire ymax = (Y == vtw - 1);
wire [ 9:0] x = (X - hzb);
wire [ 9:0] y = (Y - vtb);
// ---------------------------------------------------------------------

always @(posedge clock) begin

    // Черный цвет по краям
    {r, b, g} <= 3'b000;

    // Кадровая развертка
    X <= xmax ?         0 : X + 1;
    Y <= xmax ? (ymax ? 0 : Y + 1) : Y;

    // Вывод окна видеоадаптера
    if (X >= hzb && X < hzb + hzv && Y >= vtb && Y < vtb + vtv)
    begin
        {r, g, b} <= x == 0 || x == 639 || y == 0 || y == 399 ? 3'b111 : 3'b001;
    end

end

endmodule
