#include <avrcpu.h>
#include <stdio.h>
#include <stdlib.h>

void AVRCPU::command_move() {

    unsigned short p;
    int r, d;

    switch (command) {

        case LDI: put_rdi(get_imm8()); break;

        // Загрузка из памяти в регистры
        case LPM0Z:  sram[0] = prog[get_Z()]; break;
        case LPMRZ:  put_rd(prog[get_Z()]); break;
        case LPMRZ_: p = get_Z(); put_rd(prog[p]); put_Z(p+1); break;

        // Store X
        case STX:   put(get_X(), get_rd()); break;
        case STX_:  p = get_X();     put(p, get_rd()); put_X(p+1); break;
        case ST_X:  p = get_X() + 1; put(p, get_rd()); put_X(p); break;

        // Store Y
        case STYQ:  put((get_Y() + get_qi()), get_rd()); break;
        case STY_:  p = get_Y();     put(p, get_rd()); put_Y(p+1); break;
        case ST_Y:  p = get_Y() + 1; put(p, get_rd()); put_Y(p); break;

        // Store Z
        case STZQ:  put((get_Z() + get_qi()), get_rd()); break;
        case STZ_:  p = get_Z();     put(p, get_rd()); put_Z(p+1); break;
        case ST_Z:  p = get_Z() + 1; put(p, get_rd()); put_Z(p); break;

        // Load X
        case LDX:   put_rd(get(get_X())); break;
        case LDX_:  p = get_X();     put_rd(get(p)); put_X(p+1); break;
        case LD_X:  p = get_X() + 1; put_rd(get(p)); put_X(p); break;

        // Load Y
        case LDYQ:  put_rd(get((get_Y() + get_qi()))); break;
        case LDY_:  p = get_Y();     put_rd(get(p)); put_Y(p+1); break;
        case LD_Y:  p = get_Y() + 1; put_rd(get(p)); put_Y(p); break;

        // Load Z
        case LDZQ:  put_rd(get((get_Z() + get_qi()))); break;
        case LDZ_:  p = get_Z();     put_rd(get(p)); put_Z(p+1); break;
        case LD_Z:  p = get_Z() + 1; put_rd(get(p)); put_Z(p); break;

        case MOV:   put_rd(get_rr()); break;
        case MOVW:

            r = (get_rr_index() & 0xF) << 1;
            d = (get_rd_index() & 0xF) << 1;

            put16(d, get16(r));
            break;

        case LDS: d = fetch(); put_rd( get(d) ); break;
        case STS: d = fetch(); put(d, get_rd()); break;
        // Обмен (Z) и Rd
        case XCH: p = get_Z(); r = get(p); put(p, get_rd()); put_rd(r); break;

        default:

            printf("Неизвестная команда #2\n");
            exit(2);
    }

}
