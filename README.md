# AVR Core

Здесь складываются процессоры на основе AVR технологии.

# Схема портов ATMEGA328
```
   0    1     2     3     4      5      6     7      8     9    A      B      C    D     E      F
20 ...  ...   ...   PINB  DDB    PORTB  PINC  DDC    PORTC PIND DDD    PORTD  ...  ...   ...    ...
30 ...  ...   ...   ...   ...    TIFR0  TIFR1 TIFR2  ...   ...  ...    PCIFR  EIFR EIMSK GPIOR0 EECR
40 EEDR EEARL EEARH GTCCR TCCR0A TCCR0B TCNT0 OCR0A  OCR0B ...  GPIOR1 GPIOR2 SPCR SPSR  SPDR   ...
50 ACSR ...   ...   SMCR  MCUSR  MCUCR  ...   SPMCSR ...   ...  ...    RAMPZ  ...  SPL   SPH    SREG
```

# Схема памяти 46К Марсоход2
```
32k ROM Память программ  $0000-$3FFF
10k RAM Общая память     $0000-$37FF
 4k RAM Видеопамять      $F000-$FFFF
```

# Клавиатура

01 PgUp
02 PgDn
03 Up
04 Right
05 Down
06 Left
07 Del
08 Backspace
09 Tab
0A Enter
0B Insert
0C Home
0D End
1B Escape
