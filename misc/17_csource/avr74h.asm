; DEFINED DS (DATA)   Номер бита для DS
; DEFINED SH (SHIFT)  Номер бита для SH
; DEFINED ST (STORE)  Номер бита для ST
; DEFINED PP (PORT)   Номер регистра порта

procedure out74HC595(reg r0, reg r1) ; входящие данные

        cbi PP, ST        ; 2   Перевод в режим приема данных в теневой регистр        
        ldi r16, 16       ; 1   16 бит
rpt:    cbi PP, SH        ; 2   Подготовить к clock 0->1
        cbi PP, DS        ; 2 
        add r0, r0        ; 1   Сдвиг влево
        adc r1, r1        ; 1   Перенести старший бит r0 в младший r1
        brcc sh           ; 1.5 флаг очищен?
        sbi PP, DS        ; 2   Если нет, установить его в DS
sh:     sbi PP, SH        ; 2   Загнать в регистр (clock)
        dec r16           ; 1
        brne rpt          ; 1.9 Повторять 16 раз
        sbi PP, ST        ; 2   Перенести из теневого регистра
        ret
