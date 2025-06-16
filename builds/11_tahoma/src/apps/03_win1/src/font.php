<?php

// Конвертировать из PNG в H

$im = imagecreatefrompng("font.png");

for ($y = 0; $y < 11; $y++) {

    echo "    ";
    for ($x = 0; $x < 896; $x += 8) {

        $v = 0;
        for ($j = 0; $j < 8; $j++) {
            $v |= (imagecolorat($im, $x + $j, $y) ? (128 >> $j) : 0);
        }

        echo sprintf("0x%02X,", $v);
    }
    echo "\n";
}
