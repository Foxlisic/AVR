module io
(
    input               clock,
    input       [15:0]  a,
    input       [ 7:0]  o,
    input               r,
    input               w,
    // SD CARD
    output reg          sd_command,     // Отправка команды на SD
    output reg          sd_rw,          // 0 READ; 1 WRITE
    output reg  [31:0]  sd_lba,         // Указать адрес сектора
    input       [ 1:0]  sd_card,        // Определенный тип карты
    input       [ 3:0]  sd_error,       // В случае если появилась ошибка
    input               sd_done,        // Пришел ответ о выполненной задаче от SD
    input               sd_busy,        // =1 Карта занята в работе
    // ПЕРИФЕРИЯ
    output reg          p_vpage,        // Видеостраница 0=$8000, 1=$A000
    output reg  [ 2:0]  p_border,
    input               p_vblank,       // 60 Hz
    // КЛАВИАТУРА
    input               p_kdone,
    input       [ 7:0]  p_ascii,
    // МЫШЬ
    input       [11:0]  p_msx,
    input       [11:0]  p_msy,
    input       [ 2:0]  p_btn,
    input               p_recv,
    // ВЫХОД
    output reg  [ 7:0]  p               // Информация с порта по адресу A=[20..5Fh]
);

// Сохраненные значения
// -----------------------------------------------------------------------------

reg [17:0]  t_counter;      // Таймер
reg [ 7:0]  i_ascii;
reg [ 7:0]  i_timer;
reg         r_ascii;        // Сигнал от клавиатуры
reg         r_vblank;       // Видеосинхронизация
reg         r_done;         // Есть результат от SD
reg         r_mouse;        // Есть результат от Mouse

// Чтение из порта
// -----------------------------------------------------------------------------
always @(*)
case (a)
16'h20:  p = i_ascii;           // ASCII код от клавы
16'h21:  p = i_timer;           // Таймер (100 Гц)
16'h22:  p = r_ascii;           // Была получена кнопка? Сброс при чтении
16'h23:  p = r_vblank;          // Получен VBLank
16'h24:  p = {sd_busy, r_done, sd_card, sd_error}; // Статус карточки
16'h25:  p = p_msx[7:0];        // Mouse.X
16'h26:  p = p_msy[7:0];        // Mouse.Y
16'h27:  p = {r_mouse, 4'b0000, p_btn[2:0]}; // Mouse.Btn
default: p = 8'h00;
endcase

// Чтение и запись в порт
// -----------------------------------------------------------------------------
always @(posedge clock)
begin

    sd_command <= 0;

    // Чтение из порта
    if (r)
    case (a)
    16'h22: r_ascii  <= 0;      // Данные от клавиатуры получены
    16'h23: r_vblank <= 0;      // 60 Hz VBlank
    16'h24: r_done   <= 0;      // Данные успешно получены или записаны
    16'h27: r_mouse  <= 0;      // Сброс информации о событии мыши
    endcase

    // Запись в порт
    if (w)
    case (a)
    16'h20: p_border <= o[2:0];
    16'h21: p_vpage  <= o[0];
    16'h22: sd_lba[ 7:0]  <= o;
    16'h23: sd_lba[15:8]  <= o;
    16'h24: sd_lba[23:16] <= o;
    16'h25: sd_lba[31:24] <= o;
    18'h26: begin sd_command <= 1; sd_rw <= o[0]; end
    endcase

    // Пришел сигнал об окончании кадра
    if (p_vblank) begin r_vblank <= 1; end

    // Пришел сигнал DONE от SD карты
    if (sd_done)  begin r_done <= 1; end

    // Клавиша принята с клавиатуры
    if (p_kdone)  begin r_ascii <= 1; i_ascii <= p_ascii; end

    // Приняты данные от мыши
    if (p_recv)   begin r_mouse <= 1; end

    // Счетчик 100 Гц
    if (t_counter == 249999)
         begin t_counter <= 0; i_timer <= i_timer + 1; end
    else begin t_counter <= t_counter + 1; end

end

endmodule
