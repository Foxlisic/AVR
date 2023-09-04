module sdram
(
    // Физический интерфейс
    input           reset_n,    
    input           clk_in,     // 100 mhz
    output          clk_out,    // --> 100 mhz
    
    // Сигналы от процессора
    input           clk_cpu,    // Процессорный CLK
    input           read,       // От процессора
    input           write,      // От процессора
    input    [25:0] address,    // Запрошенный адрес
    
    // К процессору
    output  reg     ce          // Удержания работы CPU
);

assign clk_out = clk_in;

// ---------------------------------------------------------------------
reg  [1:0] flop  = 2'b00;
wire [1:0] flopn = {flop[0], clk_cpu};
// ---------------------------------------------------------------------

always @(negedge clk_in) 
if (reset_n == 1'b0) begin
    ce <= 1'b0;
end else begin

    if (flopn == 2'b10) begin
        ce <= 1'b1;
    end

    flop <= flopn;

end

endmodule
