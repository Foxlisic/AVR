module io
(
    input               clock,
    input       [15:0]  a,
    input       [ 7:0]  o,
    input               r,
    input               w,
    output reg          p_vpage,        // Видеостраница 0=$8000, 1=$A000
    output reg  [ 2:0]  p_border,
    input               p_vblank,       // 60 Hz
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

// Видео
reg         r_vblank;

// Чтение из порта
// -----------------------------------------------------------------------------
always @(*)
case (a)
16'h20:  p = i_ascii;           // ASCII код от клавы
16'h21:  p = i_timer;           // Таймер (100 Гц)
16'h22:  p = r_ascii;           // Была получена кнопка? Сброс при чтении
16'h23:  p = r_vblank;          // Получен VBLank
default: p = 8'h00;
endcase

// Чтение и запись в порт
// -----------------------------------------------------------------------------
always @(posedge clock)
begin

    // Чтение из порта
    if (r)
    case (a)
    16'h22: r_ascii  <= 0;
    16'h23: r_vblank <= 0;
    endcase

    // Запись в порт
    if (w)
    case (a)
    16'h20: p_border <= o[2:0];
    16'h21: p_vpage  <= o[0];
    endcase

    // Пришел сигнал об окончании кадра
    if (p_vblank) begin r_vblank <= 1; end

    // Клавиша принята с клавиатуры
    if (p_kdone) begin i_ascii <= p_ascii; r_ascii <= 1; end

    // Счетчик 100 Гц
    if (t_counter == 249999)
         begin t_counter <= 0; i_timer <= i_timer + 1; end
    else begin t_counter <= t_counter + 1; end

end

endmodule
