module avr
(
    input       wire        clock,
    input       wire        locked,         // =1 процессор в работе
    input       wire [15:0] pdata,          // Программные данные из "флешки"
    input       wire [ 7:0] rdata,          // RAM-data, входящие данные из памяти
    output      reg  [15:0] pc,             // Указатель на программу
    output      wire [15:0] address,        // Указатель на память
    output      reg  [ 7:0] dataw,          // Данные для записи в память
    output      reg         we              // Разрешение записи
);

`include "avr.decl.v"
`include "avr.alu.v"

always @(posedge clock) if (locked) begin

    ir <= pdata;
    pc <= pc + 1;

    // Запрос нового PC и запись старого IR
    if (tstate == 0) begin tstate <= 1; end
    // Декодирование инструкции
    else casex (ir)

        // 1T LDI r16..31, K
        16'b1110_xxxx_xxxx_xxxx: regs[ {1'b1, ir[7:4]} ] <= {ir[11:8], ir[3:0]};

        // 1T ADD Rd, Rr
        16'b0000_11xx_xxxx_xxxx: begin

            regs[ Rdi ] <= add8[7:0];
            sreg        <= add8_sreg;

        end

        // 1T RJMP k
        16'b1100_xxxx_xxxx_xxxx: begin

            pc     <= pc + {{4{ir[11]}}, ir[11:0]};
            tstate <= 0;

        end

    endcase

end

endmodule
