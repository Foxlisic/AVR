КАРТА ПАМЯТИ DE0 :: 308K :: Текстовый и графический режим
------------------------------------------------------------------------
64K     ROM 32x16
64K     RAM 64x8
64Kx2   VIDEO   [320x200 2 буфера или 640x400 один]
32K     TEXTURE [256x128x8 или 256x256x4]
4K      CHARMAP
16K     FREE

КАРТА ПАМЯТИ EP4C10 :: 46K :: Только текстовый
------------------------------------------------------------------------
32K     ROM
6K      RAM
8K      VIDEO [80x25]

Доступ к видеоданным через запись или чтение из портов.
Команды к видеоускорителю для рисования 2D или 3D.
Аналогично с доступом к SDRAM как внешнему устройству.

Порты Arduino

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
======================================================================

Порты эмулятора

00  VideoMode:
    0 80x25
    2 320x200 Bank 0
    3 320x200 Bank 1
    4 640x400 Bank 0
    5 640x400 Bank 1
01  Положение текстового курсора. Сначала X, потом Y.
02  Запись или чтение в видеопамять, инкрементирование при каждом доступе
0C  X[ 7:0] Позиционирование X
0D  X[15:8]
0E  Y[ 7:0] Позиционирование Y
0F  Y[15:8]

Модель      Память      PROG    RAM     VRAM    TEXTURE FREE
------------------------------------------------------------------------
de0         308K        64K     64K     128K    32K     16K
ep4c10      46K         32K     6K      4K      0       0
ep4ce150    720K        128K    64K     256K    256K    16K
------------------------------------------------------------------------
