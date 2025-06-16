
            org     0000h               ; F000:0000

            mov     ax, $b800
            mov     es, ax
            mov     ax, $0801
m1:         xor     di, di
            mov     cx, 2000
@@:         stosw
            dec     al
            loop    @b
            jmp     m1
            hlt
