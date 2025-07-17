# AVR Core

Здесь складываются процессоры на основе AVR технологии.

# Схема портов ATMEGA328

```
     0/8    1/9     2/A     3/B     4/C     5/D     6/E     7/F
======================================================================
00  --      --      --      PINB    DDRB    PORTB   PINC    DDRC
08  PORTC   PIND    DDRD    PORTD   --      --      --      --
10  --      --      --      --      --      TIFR0   TIFR1   TIFR2
18  --      --      --      PCIFR   EIFR    EIMSK   GPIOR0  EECR
20  EEDR    EEARL   EEARH   GTCCR   TCCR0A  TCCR0B  TCNT0   OCR0A
28  OCR0B   --      GPIOR1  GPIOR2  SPCR    SPSR    SPDR    --
30  ACSR    --      --      SMCR    MCUSR   MCUCR   --      SPMCSR
38  --      --      --      --      --      SPL     SPH     SREG
40  WDTCSR  CLKPR   --      --      PRR     --      OSCCAL  --
48  PCICR   EICRA   --      PCMSK0  PCMSK1  PCMSK2  TIMSK0  TIMSK1
50  TIMSK2  --      --      --      --      --      --      --
58  ADCL    ADCH    ADCSRA  ADCSRB  ADMUX   --      DIDR0   DIDR1
60  TCCR1A  TCCR1B  TCCR1C  --      TCNT1L  TCNT1H  ICR1L   ICR1H
68  OCR1AL  OCR1AH  OCR1BL  OCR1BH  --      --      --      --
70  --      --      --      --      --      --      --      --
78  --      --      --      --      --      --      --      --
80  --      --      --      --      --      --      --      --
88  --      --      --      --      --      --      --      --
90  TCCR2A  TCCR2B  TCNT2   OCR2A   OCR2B   --      ASSR    --
98  TWBR    TWSR    TWAR    TWDR    TWCR    TWAMR   --      --
A0  UCSR0A  UCSR0B  UCSR0C  --      UBRR0L  UBRR0H  UDR0    --
```

# Схема памяти 46К Марсоход2
```
32k ROM Память программ  $0000-$3FFF
10k RAM Общая память     $0000-$37FF
 4k RAM Видеопамять      $F000-$FFFF
```

# Клавиатура

00 NULL
01 Up
02 Right
03 Down
04 Left
05 Del
06 PgUp
07 PgDn
08 Backspace
09 Tab
0A Enter
0B Insert
0C Home
0D End
1B Escape
