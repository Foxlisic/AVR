<?php

/*
 * Аргумент 1: bin-файл
 * Аргумент 2: Куда выгрузить
 */

$data = [];
$temp = file_get_contents($argv[1]);

for ($i = 0; $i < strlen($temp); $i += 2) {
    $data[] = ord($temp[$i]) + ord($temp[$i+1])*256;
}

$len  = count($data);
$size = ($argv[3] ?? 64) * 1024;

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
        for ($i = $a; $i < $b; $i++) $out[] = sprintf("  %04X: %04X;", $a++, $data[$i]);
    } else {
        $out[] = sprintf("  [%04X..%04X]: %04X;", $a, $b - 1, $data[$a]);
        $a = $b;
    }
}

if ($len < $size) {
    $out[] = sprintf("  [%04X..%04X]: 0000;", $len, $size-1);
}

$out[] = "END;";
$pb = join("\n", $out);

// Сохранить информацию
if (isset($argv[2])) {
    file_put_contents($argv[2], $pb);
} else {
    echo $pb;
}
