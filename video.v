// verilator lint_off WIDTH
module video
(
    input               CLK,
    output reg  [3:0]   R,
    output reg  [3:0]   G,
    output reg  [3:0]   B,
    output              HS,
    output              VS,
    output reg  [15:0]  VA,
    input       [ 3:0]  VI,
    input       [ 3:0]  BRD
);
// ---------------------------------------------------------------------
// Тайминги для горизонтальной|вертикальной развертки (640x480)
parameter
    hz_back    = 48,  vt_back    = 33,  // 35  | 33
    hz_visible = 640, vt_visible = 480, // 400 | 480
    hz_front   = 16,  vt_front   = 10,  // 12  | 10
    hz_sync    = 96,  vt_sync    = 2,   // 2   | 2
    hz_whole   = 800, vt_whole   = 525; // 449 | 525
// ---------------------------------------------------------------------
assign HS = x  < (hz_back + hz_visible + hz_front); // NEG.
assign VS = y >= (vt_back + vt_visible + vt_front); // POS.
// ---------------------------------------------------------------------
wire        xmax = (x == hz_whole - 1);
wire        ymax = (y == vt_whole - 1);
reg  [ 9:0] x    = 0;
reg  [ 9:0] y    = 0;
wire [ 8:0] X    = x - hz_back - 64;
wire [ 8:0] Y    = y - vt_back;
// ---------------------------------------------------------------------
wire [ 3:0] cl = x >= (hz_back+64) && x < (hz_back+64+512) ? VI : BRD;
// ---------------------------------------------------------------------
wire [11:0] rgb =
    cl ==  0 ? 12'h111 : cl ==  1 ? 12'h008 :
    cl ==  2 ? 12'h080 : cl ==  3 ? 12'h088 :
    cl ==  4 ? 12'h800 : cl ==  5 ? 12'h808 :
    cl ==  6 ? 12'h880 : cl ==  7 ? 12'hCCC :
    cl ==  8 ? 12'h888 : cl ==  9 ? 12'h00F :
    cl == 10 ? 12'h0F0 : cl == 11 ? 12'h0FF :
    cl == 12 ? 12'hF00 : cl == 13 ? 12'hF0F :
    cl == 14 ? 12'hFF0 :            12'hFFF;
// ---------------------------------------------------------------------
reg sw;
// Вывод видеосигнала
always @(posedge CLK)
begin

    // Кадровая развертка
    x <= xmax ?         0 : x + 1;
    y <= xmax ? (ymax ? 0 : y + 1) : y;

    // Вывод окна видеоадаптера
    if (x >= hz_back && x < hz_back + hz_visible &&
        y >= vt_back && y < vt_back + vt_visible) begin

        if (x[0]) begin {R,G,B} <= rgb; end
        else      begin VA <= Y[8:1]*256 + X[8:1]; end

    end
    else {R,G,B} <= 12'h000;

end

endmodule
