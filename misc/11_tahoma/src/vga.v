/* verilator lint_off WIDTH */
/* verilator lint_off CASEINCOMPLETE */

module vga
(
    input               CLOCK,
    output  reg  [3:0]  R,
    output  reg  [3:0]  G,
    output  reg  [3:0]  B,
    output              HS,
    output              VS,
    // Считывание данных
    output  reg  [15:0] A,
    input        [ 7:0] D,
    input        [ 2:0] BORDER
);

// -----------------------------------------------------------------------------
// Тайминги для горизонтальной и вертикальной развертки
//        Visible    Front     Sync      Back      Whole
parameter hzv = 640, hzf = 16, hzs = 96, hzb = 48, hzw = 800,
          vtv = 400, vtf = 12, vts = 2,  vtb = 35, vtw = 449;
// -----------------------------------------------------------------------------
assign HS = X  < (hzb + hzv + hzf); // NEG
assign VS = Y >= (vtb + vtv + vtf); // POS
// -----------------------------------------------------------------------------
// Позиция луча в кадре и максимальные позиции (x,y)
reg  [ 9:0] X  = 0; wire xmax = (X == hzw - 1);
reg  [ 9:0] Y  = 0; wire ymax = (Y == vtw - 1);
wire [ 9:0] x  = (X - hzb); // x=[0..639]
wire [ 9:0] y  = (Y - vtb); // y=[0..399]
wire [ 7:0] xm = ((X - hzb - 64) >> 1) + 8;
wire [ 7:0] ym = ((Y - vtb - 8)  >> 1);
// -----------------------------------------------------------------------------
reg  [ 7:0] temp;
reg  [ 7:0] attr;
reg  [ 7:0] mask;
// -----------------------------------------------------------------------------
wire        cbit  = mask[ 3'h7 ^ x[3:1] ];
wire        paper = (x >= 64 && x < 64 + 512 && y >= 8 && y < 8 + 384);
wire [ 3:0] K     = paper ? ({attr[6], cbit ^ (attr[7] & flash) ? attr[2:0] : attr[5:3]}) : BORDER;
// -----------------------------------------------------------------------------

always @(posedge CLOCK) begin

    // Черный цвет по краям
    {R, G, B} <= 12'h000;

    // Кадровая развертка
    X <= xmax ?         0 : X + 1;
    Y <= xmax ? (ymax ? 0 : Y + 1) : Y;

    // Вывод окна видеоадаптера
    if (X >= hzb && X < hzb + hzv && Y >= vtb && Y < vtb + vtv)
    begin

        // Цвет или бордер
        {R, G, B} <=
        {
            {2{K[1]}}, {2{K[1] & K[3]}}, // RED
            {2{K[2]}}, {2{K[2] & K[3]}}, // GREEN
            {2{K[0]}}, {2{K[0] & K[3]}}  // BLUE
        };

        case (x[3:0])
        // Запрос CHAR $E000-$F700 (6144)
        4'h0: begin A <= {ym[7:6], ym[2:0], ym[5:3], xm[7:3]} | 16'hE000; end
        // Запрос ATTR $F800-$FAFF (768)
        4'hE: begin A <= {ym[7:3], xm[7:3]} | 16'hF800; temp <= D; end
        // Следующие 8 бит цвета готовы
        4'hF: begin mask <= temp; attr <= D; end
        endcase

    end

end

// ---------------------------------------------------------------------

reg        flash;
reg [23:0] timer;

always @(posedge CLOCK) begin

    if (timer == 12500000) begin /* полсекунды */
        timer <= 1'b0;
        flash <= flash ^ 1'b1; // мигать каждые 0.5 секунд
    end else begin
        timer <= timer + 1'b1;
    end

end

endmodule
