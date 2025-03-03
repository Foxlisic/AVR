<?php

$bin = file_get_contents($argv[1] ?? "");

$hex = [
    "WIDTH=16;",
    "DEPTH=65536;",
    "ADDRESS_RADIX=HEX;",
    "DATA_RADIX=HEX;",
    "CONTENT BEGIN",
];

$len = strlen($bin);

// Заполнить программной частью
for ($i = 0; $i < $len; $i += 2) {
    $hex[] = sprintf("  %04X: %04X;", $i >> 1, ord($bin[$i]) + ord($bin[$i+1])*256);
}

// Дополнить пустыми данными
if ($len < 65536) {
    $hex[] = sprintf(" [%04X..FFFF]: 0000;", $len >> 1);
}

$hex[] = "END;\n";

file_put_contents(__DIR__ . "/../" . ($argv[2] ?? "de0/memprog.mif"), join("\n", $hex));
