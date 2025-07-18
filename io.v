module io
(
    input               clock,
    input       [15:0]  a,
    input       [ 7:0]  o,
    input               r,
    input               w,
    output reg  [ 2:0]  p_border,
    input               p_kdone,
    input       [ 7:0]  p_ascii,
    output reg  [ 7:0]  p               // A=20..5Fh
);

// Сохраненные значения
// -----------------------------------------------------------------------------

// Клавиатура
reg         r_ascii;
reg [ 7:0]  i_ascii;

// Таймер
reg [17:0]  t_counter;
reg [ 7:0]  i_timer;

// Чтение из порта
// -----------------------------------------------------------------------------
always @(*)
case (a)
16'h20:  p = i_ascii;           // ASCII код от клавы
16'h21:  p = i_timer;           // Таймер (100 Гц)
16'h22:  p = r_ascii;           // Статус кнопки
default: p = 8'h00;
endcase

// Чтение и запись в порт
// -----------------------------------------------------------------------------
always @(posedge clock)
begin

    // Чтение из порта сбрасывает бит
    if (r)
    case (a)
    16'h20: r_ascii <= 0;
    endcase

    // Запись в порт
    if (w)
    case (a)
    16'h20: p_border <= o[2:0];
    endcase

    // Клавиша принята с клавиатуры
    if (p_kdone) begin i_ascii <= p_ascii; r_ascii <= 1; end

    // Счетчик 100 Гц
    t_counter <= t_counter == 249999 ? 0 : t_counter + 1;
    i_timer   <= t_counter == 249999 ? (i_timer + 1) : i_timer;

end

endmodule
