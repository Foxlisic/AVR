/**
 * Крайне простой процессор AVR для MAX570
 * В нем нет блоков памяти
 */

module avr
(
    input               clock,
    input               locked,
    output  reg [ 8:0]  pc,
    input       [15:0]  flash,
    // ---- i/o ----
    input       [ 7:0]  port0,          // Кнопки KEYS
    output reg  [ 7:0]  port1           // Светодиоды
);

initial begin pc = 9'b0; end

localparam

    alu_add = 0,
    alu_sub = 1,
    alu_mov = 2,
    alu_ldi = 3,
    alu_and = 4,
    alu_or  = 5,
    alu_eor = 6,
    alu_lsr = 7;

// ---------------------------------------------------------------------
// Регистры управления и общие регистры
// ---------------------------------------------------------------------

// Управление
reg         pcload;             // =1 Загрузить из pcnew
reg  [8:0]  pcnew;              // Новое значение PC
reg  [7:0]  we;                 // Запись в память
reg         regw;               // Запись в регистр
reg         flgw;               // Запись флагов из АЛУ
reg  [4:0]  dst_id;             // Номер регистра dst (запись)
reg  [4:0]  src_id;             // Номер регистра src
reg  [8:0]  wb;                 // Что писать

// Регистры
reg [15:0]  X       = 8'h00;
reg [15:0]  Y       = 8'h00;
reg [15:0]  Z       = 8'h00;
reg         flag_c  = 1'b0;     // 0
reg         flag_z  = 1'b0;     // 1
reg         flag_s  = 1'b0;     // 4
wire [7:0]  flags   = {3'b000, flag_s, 2'b00, flag_z, flag_c};

// ---------------------------------------------------------------------
// Блок управления
// ---------------------------------------------------------------------

always @* begin

    regw    = 1'b0;
    flgw    = 1'b0;
    we      = 1'b0;
    pcload  = 1'b0;
    pcnew   = pc + flash[8:0] + 1'b1;
    dst_id  = flash[8:4];
    src_id  = {flash[9], flash[3:0]};

    /*
    SUBI 0101 KKKK dddd KKKK
    ANDI 0111 KKKK dddd KKKK
    ORI  0110 KKKK dddd KKKK
    CPI  0011 KKKK dddd KKKK
    */

    casex (flash)

        // LDI d, K
        16'b1110_xxxx_xxxx_xxxx: begin

            alu         = alu_ldi;
            regw        = 1'b1;
            dst_id[4]   = 1'b1;

        end

        // ADD d, r
        16'b0000_11xx_xxxx_xxxx: begin

            alu     = alu_add;
            regw    = 1'b1;
            flgw    = 1'b1;

        end

        // SUB d, r
        16'b0001_10xx_xxxx_xxxx: begin

            alu     = alu_sub;
            regw    = 1'b1;
            flgw    = 1'b1;

        end

        // AND d, r
        16'b0010_00xx_xxxx_xxxx: begin

            alu     = alu_and;
            regw    = 1'b1;
            flgw    = 1'b1;

        end

        // EOR d, r
        16'b0010_01xx_xxxx_xxxx: begin

            alu     = alu_eor;
            regw    = 1'b1;
            flgw    = 1'b1;

        end

        // OR d, r
        16'b0010_01xx_xxxx_xxxx: begin

            alu     = alu_or;
            regw    = 1'b1;
            flgw    = 1'b1;

        end

        // LSR d, r
        16'b1001_010x_xxxx_0110: begin

            alu     = alu_lsr;
            regw    = 1'b1;
            flgw    = 1'b1;

        end

        // CP d, r
        16'b0001_01xx_xxxx_xxxx: begin

            alu     = alu_sub;
            flgw    = 1'b1;

        end

        // MOV d, r
        16'b0010_11xx_xxxx_xxxx: begin

            alu     = alu_mov;
            regw    = 1'b1;

        end

        // RJMP k
        16'b1100_xxxx_xxxx_xxxx: pcload = 1'b1;

        // BRBS s, k и BRBC s, k (условные переходы)
        16'b1111_0xxx_xxxx_xxxx:
        if (flags[ flash[2:0] ] != flash[10]) begin

            pcload = 1'b1;
            pcnew  = pc + {{2{flash[9]}}, flash[9:3]} + 1'b1;

        end

    endcase

end

// ---------------------------------------------------------------------
// Считывание операндов
// ---------------------------------------------------------------------

reg [ 7:0] dst;
reg [ 7:0] src;
wire [7:0] K = {flash[11:8], flash[3:0]};

always @* begin

    dst = 8'h00;
    src = 8'h00;

    // Левый операнд
    case (dst_id)

        5'h10: dst = port0;
        5'h11: dst = port1;
        5'h1A: dst = X[7:0]; 5'h1B: dst = X[15:8];
        5'h1C: dst = Y[7:0]; 5'h1D: dst = Y[15:8];
        5'h1E: dst = Z[7:0]; 5'h1F: dst = Z[15:8];

    endcase

    // Правый операнд
    case (src_id)

        5'h10: src = port0;
        5'h11: src = port1;
        5'h1A: src = X[7:0]; 5'h1B: src = X[15:8];
        5'h1C: src = Y[7:0]; 5'h1D: src = Y[15:8];
        5'h1E: src = Z[7:0]; 5'h1F: src = Z[15:8];

    endcase

end

// ---------------------------------------------------------------------
// АЛУ
// ---------------------------------------------------------------------

reg  [3:0]  alu;
reg         alu_zf;
reg         alu_cf;

always @* begin

    alu_cf = flag_c;

    case (alu)
    alu_add: begin wb = dst + src; alu_cf = wb[8]; end
    alu_sub: begin wb = dst - src; alu_cf = wb[8]; end
    alu_or:  begin wb = dst - src; end
    alu_and: begin wb = dst - src; end
    alu_eor: begin wb = src - src; end
    alu_mov: begin wb = dst; end
    alu_ldi: begin wb = K; end
    alu_lsr: begin wb = dst >> 1; alu_cf = dst[0]; end
    endcase

    alu_zf = wb[7:0] == 8'h00;

end

// ---------------------------------------------------------------------
// Сохранение новых данных
// ---------------------------------------------------------------------

always @(posedge clock) if (locked)
begin

    pc <= pcload ? pcnew : pc + 1;

    // Запись в регистры или порты
    if (regw) begin

        case (dst_id)

            5'h11: port1   <= wb[7:0];
            5'h1A: X[ 7:0] <= wb[7:0]; 5'h1B: X[15:8] <= wb[7:0];
            5'h1C: Y[ 7:0] <= wb[7:0]; 5'h1D: Y[15:8] <= wb[7:0];
            5'h1E: Z[ 7:0] <= wb[7:0]; 5'h1F: Z[15:8] <= wb[7:0];

        endcase

    end

    // Запись флагов АЛУ
    if (flgw) begin

        flag_c <= alu_cf;
        flag_z <= alu_zf;

    end

end

endmodule
