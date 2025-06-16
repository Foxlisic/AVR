<?php

/*
 * Конвертирование из Bin -> MIF файл
 * Аргумент 1: Размер памяти (256k = 262144)
 * Аргумент 2: bin-файл
 * Аргумент 3: Куда выгрузить
 */

$size = (int) $argv[1] * 1024;
$in   = file_get_contents($argv[2]);
$len  = strlen($in);
$data = [];

// Преобразование в 16-битные
for ($i = 0; $i < $len; $i += 2) {
    $data[] = ord($in[$i]) + 256*ord($in[$i+1]);
}

$len = count($data);

if (empty($size)) { echo "size required\n"; exit(1); }

$out = [
    "WIDTH=16;",
    "DEPTH=$size;",
    "ADDRESS_RADIX=HEX;",
    "DATA_RADIX=HEX;",
    "CONTENT BEGIN",
];

$a = 0;

// RLE-кодирование
while ($a < $len) {

    // Поиск однотонных блоков
    for ($b = $a + 1; $b < $len && $data[$a] == $data[$b]; $b++);

    // Если найденный блок длиной до 0 до 2 одинаковых символов
    if ($b - $a < 3) {
        for ($i = $a; $i < $b; $i++) $out[] = sprintf("  %X: %04X;", $a++, $data[$i]);
    } else {
        $out[] = sprintf("  [%X..%X]: %04X;", $a, $b - 1, $data[$a]);
        $a = $b;
    }
}

if ($len < $size) $out[] = sprintf("  [%X..%X]: 0000;", $len, $size-1);
$out[] = "END;";
$pb = join("\n", $out);

// Сохранить информацию
if (isset($argv[3])) file_put_contents($argv[3], $pb); else echo $pb;
