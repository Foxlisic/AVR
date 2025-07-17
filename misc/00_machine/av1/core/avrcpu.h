/**
 * Модель SRAM
 * 0000 - 001F Регистры
 * 0020 - 005F I/O, стек
 * 0060 - BFFF Общая память 48 кб
 * C000 - FFFF Видеопамять 16 кб
 */

struct CPUFlags {
    int c, z, n, v, s, h, t, i;
};

class AVRCPU {

private:

    int map[65536]; // Инструкции

    unsigned char  prog[131072];
    unsigned char  sram[65536];
    unsigned short opcode, command;
    unsigned int   timer;

    int pc;
    int _need_update;
    long tstate;

    struct CPUFlags flag;

public:

    AVRCPU();

    // Интерфейс процессора
    unsigned char* getsram() { return sram; }
    unsigned char* getprog() { return prog; }
    long get_tstate() { return tstate; }

    void assign_mask(const char*, int);
    void step();
    void frame();
    void dumpreg();
    void dumpstat();

    // Интерфейс взаимодействия
    void key_press(int);
    void key_up(int);

    // Требуется ли обновление экрана
    int need_update() { int n = _need_update; _need_update = 0; return n; }

    // Вспомогательные функции
    int fetch() { int data = prog[pc] + prog[pc+1]*256; pc += 2; return data; }

    // Установка флагов
    unsigned int neg(unsigned int);
    void set_logic_flags(unsigned char);
    void set_subcarry_flag(int, int, int, int);
    void set_subtract_flag(int, int, int);
    void set_add_flag(int, int, int, int);
    void set_adiw_flag(int, int);
    void set_sbiw_flag(int, int);
    void set_lsr_flag(int, int);

    // Исполнение блоков инструкции
    void command_arith();
    void command_misc();
    void command_move();

    // Извлечение операндов
    int get_rd_index() { return (opcode & 0x1F0) >> 4; }
    int get_rr_index() { return (opcode & 0x00F) | ((opcode & 0x200)>>5); }
    int get_rd() { return sram[ get_rd_index() ]; }
    int get_rr() { return sram[ get_rr_index() ]; }
    int get_rdi() { return sram[ get_rd_index() | 0x10 ]; }
    int get_rri() { return sram[ get_rr_index() | 0x10 ]; }
    int get_imm8() { return (opcode & 0xF) + ((opcode & 0xF00) >> 4); }
    int get_ap() { return (opcode & 0x00F) | ((opcode & 0x600) >> 5); }
    int get_ka() { return (opcode & 0x00F) | ((opcode & 0x0C0) >> 2); }
    int get_qi() { return (opcode & 0x007) | ((opcode & 0xC00) >> 7) | ((opcode & 0x2000) >> 8); }
    int get_s3() { return (opcode & 0x070) >> 4; }

    // 16 bit
    int get_S() { return sram[0x5D] + sram[0x5E]*256; }
    int get_X() { return sram[0x1A] + sram[0x1B]*256; }
    int get_Y() { return sram[0x1C] + sram[0x1D]*256; }
    int get_Z() { return sram[0x1E] + sram[0x1F]*256; }

    void put_S(unsigned short a) { sram[0x5D] = a & 0xFF; sram[0x5E] = (a >> 8) & 0xFF; }
    void put_X(unsigned short a) { sram[0x1A] = a & 0xFF; sram[0x1B] = (a >> 8) & 0xFF; }
    void put_Y(unsigned short a) { sram[0x1C] = a & 0xFF; sram[0x1D] = (a >> 8) & 0xFF; }
    void put_Z(unsigned short a) { sram[0x1E] = a & 0xFF; sram[0x1F] = (a >> 8) & 0xFF; }

    void put16(int a, unsigned short v) { sram[a] = v & 0xff; sram[a+1] = (v >> 8) & 0xff; }
    unsigned short get16(int a)  { return sram[a] + 256*sram[a+1]; }

    void put(int, unsigned char);
    void put_rd(unsigned char value) { sram[get_rd_index()] = value & 0xff; }
    void put_rr(unsigned char value) { sram[get_rr_index()] = value & 0xff; }
    void put_rdi(unsigned char value) { sram[get_rd_index() | 0x10] = value & 0xff; }
    unsigned char get(int);

    // Работа со стеком
    void push8(unsigned char v8) { unsigned short sp = get_S(); put(sp, v8); put_S((sp - 1) & 0xffff); }
    void push16(unsigned short v16) { push8(v16 >> 8); push8(v16 & 0xff); }
    unsigned char pop8() { unsigned short sp = (get_S() + 1) & 0xffff; put_S(sp); return get(sp); }
    unsigned short pop16() { int l = pop8(); int h = pop8(); return h*256 + l; }

    void byte_to_flag(unsigned char);
    unsigned char flag_to_byte();

    // Относительные переходы
    int get_rjmp()   { return pc + 2*((opcode & 0x800) > 0 ? ((opcode & 0x7FF) - 0x800) : (opcode & 0x7FF)); }
    int get_branch() { return pc + 2*((opcode & 0x200) > 0 ? ((opcode & 0x1F8)>>3) - 0x40 : ((opcode & 0x1F8)>>3) ); }

    void timer_tick() { timer++; }
};

enum AVROpcodes {

    UNDEFINED = 0,
    CPC,    SBC,    ADD,    CP,     SUB,    ADC,    AND,    EOR,    OR,
    CPI,    SBCI,   SUBI,   ORI,    ANDI,   ADIW,   SBIW,
    RJMP,   RCALL,  RET,    RETI,   BRBS,   BRBC,
    SBRC,   SBRS,   SBIC,   SBIS,   CPSE,
    ICALL,  EICALL, IJMP,   EIJMP,  CALL,   JMP,
    LDI,    MOV,    LDS,    STS,    MOVW,   BLD,    BST,    XCH,
    INC,    DEC,    LSR,    ASR,    ROR,    NEG,    COM,    SWAP,   LAC,
    LAS,    LAT,    BCLR,   BSET,   CBI,    SBI,
    LDX,    LDX_,   LD_X,   LDY_,   LD_Y,   LDYQ,   LDZ_,   LD_Z,   LDZQ,
    STX,    STX_,   ST_X,   STY_,   ST_Y,   STYQ,   STZ_,   ST_Z,   STZQ,
    LPM0Z,  LPMRZ,  LPMRZ_, ELPM0Z, ELPMRZ, ELPMRZ_, SPM,   SPM2,
    SLEEP,  WDR,    BREAK,  NOP,    IN,     OUT,    PUSH,   POP,    DES
};
