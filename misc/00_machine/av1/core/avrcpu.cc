#include <avrcpu.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * I/O
 * $00 Keyboard Status 
 * $01 Keyboard Scancode
 * $02 Timer Lo
 * $03 Timer Hi
 * */

// Применение маски к опкоду
void AVRCPU::assign_mask(const char* mask, int opcode) {

    int i, j;
    int mk[16], nmask = 0, baseop = 0;

    // Расчет позиции, где находится маска
    for (i = 0; i < 16; i++) {

        int ch = mask[15-i];
        if (ch == '0' || ch == '1') 
            baseop |= (ch == '1' ? 1 << i : 0);            
        else 
            mk[nmask++] = i;
    }

    // Перебрать 2^nmask вариантов, по количеству битов и назначить всем опкод
    for (i = 0; i < (1<<nmask); i++) {

        int instn = baseop;
        for (j = 0; j < nmask; j++) if (i & (1 << j)) instn |= (1 << mk[j]);        
        map[instn] = opcode; 
    }
}

// Отметить нажатую клавишу
void AVRCPU::key_press(int xt) {

    sram[0x20] |= 0x1;
    sram[0x21]  = xt;
}

// Отметить отпущенную клавишу
void AVRCPU::key_up(int xt) {

    sram[0x20] |= 0x1;
    sram[0x21]  = 0x80 | xt;
}

// Чтение из памяти
unsigned char AVRCPU::get(int addr) {

    addr &= 0xffff;
    unsigned char dv = 0;

    // Очистка бита в порту 00 при чтении
    switch (addr) {
        
        case 0x20: dv = sram[0x20]; sram[0x20] &= ~0x01; break;
        case 0x22: dv = (timer & 0xff); break;
        case 0x23: dv = (timer >> 8) & 0xff; break;
        default:   dv = sram[addr]; break;
    }
    
    return dv;
}

// Сохранение в память
void AVRCPU::put(int addr, unsigned char value) {
    
    sram[addr & 0xffff] = value;
    
    if (addr == 0x5F) byte_to_flag(value);
    if (addr >= 0xC000) _need_update = 1;
}

// Байт во флаги
void AVRCPU::byte_to_flag(unsigned char f) {

    flag.c = (f >> 0) & 1;
    flag.z = (f >> 1) & 1;
    flag.n = (f >> 2) & 1;
    flag.v = (f >> 3) & 1;
    flag.s = (f >> 4) & 1;
    flag.h = (f >> 5) & 1;
    flag.t = (f >> 6) & 1;
    flag.i = (f >> 7) & 1;
};

// Флаги в байты
unsigned char AVRCPU::flag_to_byte() {

    unsigned char f = 
        (flag.i<<7) |
        (flag.t<<6) |
        (flag.h<<5) |
        (flag.s<<4) |
        (flag.v<<3) |
        (flag.n<<2) |
        (flag.z<<1) |
        (flag.c<<0);

    sram[0x5F] = f;
    return f;
}

// Исполнение шага процессора
void AVRCPU::step() {

    int d, a, b, A, v;

    opcode  = fetch();
    command = map[opcode];

    // Исполнение опкода
    switch (command) {

        case WDR:
        case NOP: break;

        // Остановка выполнения кода
        case SLEEP: pc = pc - 2; break;

        // Управляющие
        case RJMP:  pc = get_rjmp(); break;
        case RCALL: push16(pc); pc = get_rjmp(); break;
        case RET:   pc = pop16(); break;
        case RETI:  pc = pop16(); flag.i = 1; flag_to_byte(); break;
        case BCLR:  sram[0x5F] &= ~(1 << get_s3()); byte_to_flag(sram[0x5F]); break;
        case BSET:  sram[0x5F] |=  (1 << get_s3()); byte_to_flag(sram[0x5F]); break;
        
        // Условные перехдоды
        case BRBS: if ( (sram[0x5F] & (1<<(opcode & 7)))) pc = get_branch(); break;
        case BRBC: if (!(sram[0x5F] & (1<<(opcode & 7)))) pc = get_branch(); break;
        case CPSE: if (get_rd() == get_rr()) pc += 2; break;
        case SBRC: if (!(get_rd() & (1 << (opcode & 7)))) pc = pc + 2; break;
        case SBRS: if   (get_rd() & (1 << (opcode & 7)))  pc = pc + 2; break;

        // Пропуск, если в порту Ap есть бит
        case SBIS:
        case SBIC:

            b = (opcode & 7);
            A = (opcode & 0xF8) >> 3;
            v = get(0x20 + A) & (1 << b);
            if ((command == SBIS && v) || (command == SBIC && !v)) pc = pc + 2;
            break;

        // Ввод-вывод
        case IN:  put_rd(get(0x20 + get_ap())); break;
        case OUT: put(0x20 + get_ap(), get_rd()); break;        
        // Сброс/установка бита в I/O
        case CBI: case SBI:

            b = 1 << (opcode & 0x07);
            A = (opcode & 0xF8) >> 3;
            
            if (command == CBI)
                 put(0x20 + A, get(0x20 + A) & ~b);
            else put(0x20 + A, get(0x20 + A) |  b);
            break;

        // Стек
        case PUSH: push8(get_rd()); break;
        case POP:  put_rd(pop8()); break;

        // Срециальные
        case SWAP: d = get_rd(); put_rd(((d & 0x0F) << 4) + ((d & 0xF0) >> 4)); break;
        case BST:  flag.t = (get_rd() & (1 << (opcode & 7))) > 0 ? 1 : 0; flag_to_byte(); break;
        case BLD:  a = get_rd(); b = (1 << (opcode & 7)); put_rd( flag.t ? (a | b) : (a & ~b) ); break;

        // Арифметико-логические инструкции
        case CPC:  case SBC: case ADD:  case CP: case SUB:
        case ADC:  case AND: case EOR:  case OR: case ORI:
        case ANDI: case CPI: case SUBI: case SBCI:
        case ADIW: case SBIW:
        case COM:  case INC: case DEC:
        case LSR:  case NEG: case ROR: case ASR:
        case LAC:  case LAS: case LAT:

            command_arith(); break;        

        case LDI: 
        case LPMRZ_: case LPM0Z: case LPMRZ:
        case MOV: case MOVW:
        // Store
        case STX:  case STX_: case ST_X: 
        case STYQ: case STY_: case ST_Y:
        case STZQ: case STZ_: case ST_Z:
        // Load
        case LDX:  case LDX_: case LD_X:
        case LDYQ: case LDY_: case LD_Y:
        case LDZQ: case LDZ_: case LD_Z:
        // Direct
        case LDS: case STS: case XCH:

            command_move(); break;

        case BREAK:

            printf("\n   === %li | Отладочная инструкция BREAK ===\n\n", tstate);
            dumpreg(); dumpstat();
            break;

        default:

            dumpreg(); dumpstat();
            printf("Неизвестная инструкция $%04x в pc=$%04x\n", opcode, pc - 2);
            exit(1);
    }

    tstate++;
}

// Дамп состояния процессора
void AVRCPU::dumpstat() {

    printf("[%04X:%04X] ", pc, get_S());
    printf("%c", flag.t ? 'I' : '-');
    printf("%c", flag.i ? 'T' : '-');
    printf("%c", flag.h ? 'H' : '-');
    printf("%c", flag.s ? 'S' : '-');
    printf("%c", flag.v ? 'V' : '-');
    printf("%c", flag.n ? 'N' : '-');
    printf("%c", flag.z ? 'Z' : '-');
    printf("%c", flag.c ? 'C' : '-');
    printf(" (%02x)", sram[0x5F]);
    printf(" XYZ=(%04X, %04X, %04X)\n", get_X(), get_Y(), get_Z());
}

// Дамп регистров
void AVRCPU::dumpreg() {

    for (int i = 0; i < 32; i++) { 

        printf(i < 10 ? "r0%d=%02X " : "r%d=%02X ", i, sram[i]); 
        if ((i%8) == 7) printf("\n"); 
    } 
}

// Выполнение одного фрейма 208k инструкции для 25 мгц
void AVRCPU::frame() {

    int i;
    for (i = 0; i < 4*16384; i++) {
        step();
    }
}