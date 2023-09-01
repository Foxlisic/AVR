
        
        ; X это PC. 28T на исполнение 1 инструкции

NextInstr: 

        ; 8T
        ld      Yl, X+      ; 2 Загрузка опкода
        ldi     Yh, LUT1    ; 1 Младшая часть адреса перехода
        ld      Zl, Y       ; 1
        ldi     Yh, LUT2    ; 1 Старшая часть
        ld      Zh, Y       ; 1
        ijmp                ; 2
