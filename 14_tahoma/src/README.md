# AVR

AVR это простой процессор для того, чтобы выпендриться

# PORTS

```
00 RW   БАНК ПАМЯТИ
------------------------------------------------------------------------
01 R    Последний код клавиши с клавиатуры
01 W    Цвет бордера
------------------------------------------------------------------------
02 R    Бит 0: Триггер нажатия на клавиатуру
        Бит 1: SPI Busy
        Бит 2: SPI Timeout

02 W    Сброс триггера клавиатуры
------------------------------------------------------------------------
03 RW   Данные для SPI
04 W    Бит 0: Выполнение команды, срабатывает на 0->1
------------------------------------------------------------------------
0E W    Время истечения таймера
0E R    ТАЙМЕР (мс) LO
0F R    ТАЙМЕР (мс) HI
```