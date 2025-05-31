// verilator lint_off WIDTH
module video
(
    input               CLK,
    input               RESET_N,

    // Вывод на
    output reg  [3:0]   R,
    output reg  [3:0]   G,
    output reg  [3:0]   B,
    output              HS,
    output              VS,

    // Устройство занято
    output reg          BUSY,

    // Физический интерфейс DRAM
    output              DRAM_CLK,       // Тактовая частота памяти 25 MHZ
    output reg  [ 1:0]  DRAM_BA,        // 4 банка
    output reg  [12:0]  DRAM_ADDR,      // Максимальный адрес 2^13=8192
    inout       [15:0]  DRAM_DQ,        // Ввод-вывод
    output reg          DRAM_CAS,       // CAS
    output reg          DRAM_RAS,       // RAS
    output reg          DRAM_WE,        // WE
    output reg          DRAM_LDQM,      // Маска для младшего байта
    output reg          DRAM_UDQM       // Маска для старшего байта
);

// Направление данных (in, out), если dram_we=0, то запись; иначе чтение
assign DRAM_DQ      = DRAM_WE ? 16'hZZZZ : {data_i, data_i};
assign DRAM_CLK     = CLK;

// Команды
localparam
    //                   RCW
    cmd_loadmode    = 3'b000,
    cmd_refresh     = 3'b001,
    cmd_precharge   = 3'b010,
    cmd_activate    = 3'b011,
    cmd_write       = 3'b100,
    cmd_read        = 3'b101,
    cmd_burst_term  = 3'b110,
    cmd_nop         = 3'b111;

`ifdef ICARUS
localparam INIT_TIME = 0;
`else
localparam INIT_TIME = 1250;    // 25 ns x 1250 (31 mks)
`endif

`define RCW {DRAM_RAS,DRAM_CAS,DRAM_WE}

// DRAM
// ---------------------------------------------------------------------
reg [10:0]  ic;         // 0..2047
reg [ 7:0]  data_i;
reg         reset;
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
wire [ 9:0] X    = x - hz_back;
wire [ 9:0] Y    = y - vt_back;
// ---------------------------------------------------------------------
wire        paperx = x >= hz_back && x < hz_back + hz_visible;
wire        papery = y >= vt_back && y < vt_back + vt_visible;
wire        paper  = paperx && papery;
// ---------------------------------------------------------------------

// Вывод видеосигнала
always @(negedge CLK)
if (RESET_N == 0) begin

    reset       <= 1;
    ic          <= 0;
    BUSY        <= 0;
    DRAM_ADDR   <= 0;
    DRAM_LDQM   <= 1;
    DRAM_UDQM   <= 1;
    DRAM_BA     <= 0;

end
else if (reset) begin

    ic <= ic + 1;

    case (ic)

        INIT_TIME + 0:  begin `RCW  <= cmd_precharge; end
        INIT_TIME + 3:  begin `RCW  <= cmd_refresh;   end
        INIT_TIME + 17: begin `RCW  <= cmd_loadmode; DRAM_ADDR[9:0] <= 10'b0_00_010_0_111; end
        INIT_TIME + 20: begin ic <= 0; reset <= 0; end
        default:        begin `RCW  <= cmd_nop; end

    endcase

end
else begin

    BUSY <= 0;
    {R,G,B} <= 12'h000;

    // Кадровая развертка
    x <= xmax ?         0 : x + 1;
    y <= xmax ? (ymax ? 0 : y + 1) : y;

    // Вывод окна видеоадаптера
    if (paper) {R,G,B} <= 12'hFFF;

    // Обновление видимых строк
    if (x <= 256+3 && papery) begin

        BUSY <= 1;

        case (x)
        // Запрос строки
        0: begin

            `RCW        <= cmd_activate;
            DRAM_BA     <= 2'b00;
            DRAM_ADDR   <= Y[8:2];            // Y=0..383
            DRAM_UDQM   <= 1'b0;
            DRAM_LDQM   <= 1'b0;

        end
        // Ожидание открытия строки
        1,3: begin `RCW <= cmd_nop; end
        // Выбор стартовой колонки (256 байт x 4 строки)
        2: begin

            `RCW        <= cmd_read;
            DRAM_ADDR   <= {Y[1:0], 8'h00};

        end
        // Запрос PRECHARGE
        256+3: begin

            `RCW <= cmd_precharge;
            DRAM_ADDR[10] <= 1'b1;

        end
        // Считывание 256 WORD
        default: begin

            // sram_o  <= DRAM_DQ;
            // sram_we <= 1;
            DRAM_ADDR[7:0] <= DRAM_ADDR[7:0] + 1;

        end
        endcase

    end
end

endmodule
