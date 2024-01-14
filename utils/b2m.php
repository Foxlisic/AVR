<?php

/*
 * Конвертирование из Bin -> MIF файл
 * Аргумент 1: Размер памяти (256k = 262144)
 * Аргумент 2: bin-файл
 * Аргумент 3: Куда выгрузить
 */

$bin  = file_get_contents($argv[1]);
$arg  = explode(":", $argv[2]);
$size = $arg[0] * 1024;
$bits = $arg[1] ?? 8;
$incr = $bits == 16 ? 2 : 1;
$data = [];

for ($i = 0; $i < strlen($bin); $i += $incr) {
    $data[] = ord($bin[$i]) + ($bits == 16 ? 256*ord($bin[$i+1]) : 0);
}

if (empty($size)) { echo "size required\n"; exit(1); }

$len  = count($data);

$out = [
    "WIDTH=$bits;",
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
        for ($i = $a; $i < $b; $i++) $out[] = sprintf("  %X: %X;", $a++, $data[$i]);
    } else {
        $out[] = sprintf("  [%X..%X]: %X;", $a, $b - 1, $data[$a]);
        $a = $b;
    }
}

if ($len < $size) $out[] = sprintf("  [%X..%X]: 0;", $len, $size-1);
$out[] = "END;";
$pb = join("\n", $out);

// Сохранить информацию
if (isset($argv[3])) file_put_contents($argv[3], $pb); else echo $pb;
