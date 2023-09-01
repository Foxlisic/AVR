<?php

$bios = file_get_contents($argv[1]);
$size = strlen($bios);

$map = [];
$out = "const byte DATASET[] PROGMEM = \n";

for ($i = 0; $i < $size; $i++) {
    $map[] = sprintf("0x%02x", ord($bios[$i])) . ($i%16 == 15 ? "\n" : "");
}

$out .= '{'.join(", ", $map).'};';
echo $out;
