// Исполнение программы
// -----------------------------------------------------------------------------

unsigned short  program[65536];
unsigned char   sram   [65536];
int             map[65536];

struct CPUFlags {
    int c, z, n, v, s, h, t, i;
};

enum AVROpcodes {

    UNDEFINED = 0,
    CPC,    SBC,    ADD,    CP,     SUB,    ADC,     AND,    EOR,    OR,
    CPI,    SBCI,   SUBI,   ORI,    ANDI,   ADIW,    SBIW,
    RJMP,   RCALL,  RET,    RETI,   BRBS,   BRBC,
    SBRC,   SBRS,   SBIC,   SBIS,   CPSE,
    ICALL,  EICALL, IJMP,   EIJMP,  CALL,   JMP,
    LDI,    MOV,    LDS,    STS,    MOVW,   BLD,     BST,    XCH,
    INC,    DEC,    LSR,    ASR,    ROR,    NEG,     COM,    SWAP,   LAC,
    LAS,    LAT,    BCLR,   BSET,   CBI,    SBI,
    LDX,    LDX_,   LD_X,   LDY_,   LD_Y,   LDYQ,    LDZ_,   LD_Z,   LDZQ,
    STX,    STX_,   ST_X,   STY_,   ST_Y,   STYQ,    STZ_,   ST_Z,   STZQ,
    LPM0Z,  LPMRZ,  LPMRZ_, ELPM0Z, ELPMRZ, ELPMRZ_, SPM,   SPM2,
    SLEEP,  WDR,    BREAK,  NOP,    INI,    OUTI,    PUSH,   POP,    DES,
    MUL,    MULS,   MULSU
};

unsigned short opcode, pc, command, cpu_halt;
struct  CPUFlags    flag;
int     last_timer, timer, fcounter, flash;
int     instr_counter, cycles;

// Объявление процедур
void assign();
void config();
void displayout();
void disassembly();
void assign_mask(const char* mask, int opcode);
void interruptcall();

// Интерфейс памяти
void put(int, unsigned char);

// Установка флагов
unsigned int neg        (unsigned int);
void set_logic_flags    (unsigned char);
void set_subcarry_flag  (int, int, int, int);
void set_subtract_flag  (int, int, int);
void set_add_flag       (int, int, int, int);
void set_adiw_flag      (int, int);
void set_sbiw_flag      (int, int);
void set_lsr_flag       (int, int);

// Извлечение операндов
int get_rd_index() { return (opcode & 0x1F0) >> 4; }
int get_rr_index() { return (opcode & 0x00F) | ((opcode & 0x200)>>5); }
int get_rd()    { return sram[ get_rd_index() ]; }
int get_rr()    { return sram[ get_rr_index() ]; }
int get_rdi()   { return sram[ get_rd_index() | 0x10 ]; }
int get_rri()   { return sram[ get_rr_index() | 0x10 ]; }
int get_imm8()  { return (opcode & 0xF) + ((opcode & 0xF00) >> 4); }
int get_ap()    { return (opcode & 0x00F) | ((opcode & 0x600) >> 5); }
int get_ka()    { return (opcode & 0x00F) | ((opcode & 0x0C0) >> 2); }
int get_qi()    { return (opcode & 0x007) | ((opcode & 0xC00) >> 7) | ((opcode & 0x2000) >> 8); }
int get_s3()    { return (opcode & 0x070) >> 4; }
int get_jmp()   { return (opcode & 1) | ((opcode & 0x1F0) >> 3); }

// 16 bit Получение
int get_S() { return sram[0x5D] + sram[0x5E]*256; }
int get_X() { return sram[0x1A] + sram[0x1B]*256; }
int get_Y() { return sram[0x1C] + sram[0x1D]*256; }
int get_Z() { return sram[0x1E] + sram[0x1F]*256; }

// 16 bit  Сохранить
void put_S(unsigned short a) { sram[0x5D] = a & 0xFF; sram[0x5E] = (a >> 8) & 0xFF; }
void put_X(unsigned short a) { sram[0x1A] = a & 0xFF; sram[0x1B] = (a >> 8) & 0xFF; }
void put_Y(unsigned short a) { sram[0x1C] = a & 0xFF; sram[0x1D] = (a >> 8) & 0xFF; }
void put_Z(unsigned short a) { sram[0x1E] = a & 0xFF; sram[0x1F] = (a >> 8) & 0xFF; }

void put_rd(unsigned char value) { sram[get_rd_index()] = value & 0xff; }
void put_rr(unsigned char value) { sram[get_rr_index()] = value & 0xff; }
void put_rdi(unsigned char value) { sram[get_rd_index() | 0x10] = value & 0xff; }
unsigned char get(int);

// Работа со стеком
void            byte_to_flag(unsigned char);
unsigned char   flag_to_byte();

// Относительные переходы
int skip_instr();
int step();

// Дизассемблер
// -----------------------------------------------------------------------------

static const char* ds_brcs[4][8] = {
    {"cc", "ne", "pl", "vc", "ge", "hc", "tc", "id"},
    {"cs", "eq", "mi", "vs", "lt", "hs", "ts", "ie"},
    {"sec","sez","sen","sev","ses","seh","set","sei"},
    {"clc","clz","cln","clv","cls","clh","clt","cli"}
};

int     ds_fetch(long& addr);
int     ds_info(long addr);
void    ds_update();
unsigned char pvsram [96];

char    ds_line[128];           // Дизассемблированная строка
int     ds_addresses[64];
int     ds_start;               // Стартовый адрес дизассемблера
int     ds_current;             // Текущий адрес
int     ds_cursor;              // Положение курсора
int     ds_debugger;            // Если =1, то находится в отладчике
int     ds_tab;                 // Где находимся
int     ds_brk[128];            // Брейкпоинты
int     ds_brk_cnt;
int     ds_dump_cursor;
int     ds_dump_start;

void    ds_keydown_spec(int key, int x, int y);
void    ds_keydown(unsigned char key, int x, int y);