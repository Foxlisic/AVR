<?php

$lookup =
[
    [ 0, 32,  8, 40,  2, 34, 10, 42],
    [48, 16, 56, 24, 50, 18, 58, 26],
    [12, 44,  4, 36, 14, 46,  6, 38],
    [60, 28, 52, 20, 62, 30, 54, 22],
    [ 3, 35, 11, 43,  1, 33,  9, 41],
    [51, 19, 59, 27, 49, 17, 57, 25],
    [15, 47,  7, 39, 13, 45,  5, 37],
    [63, 31, 55, 23, 61, 29, 53, 21],
];

$im = imagecreatefrompng("compbw.png");
$dt = imagecreate(32, 32);

imagecolorset($dt, 0, 0, 0, 0, 0);
imagecolorset($dt, 0, 255, 255, 255, 0);

for ($y = 0; $y < 32; $y++) {
    
    $dword = 0;
    for ($x = 0; $x < 32; $x++) {
        
        $at = imagecolorat($im, $x, $y); // + ($lookup[$y & 7][$x & 7] - 32)*0.65;
        $cl = ($at > 128) ? 1 : 0;
        
        imagesetpixel($dt, $x, $y, $cl);
        
        $dword = 2*$dword + $cl;
    }
    echo sprintf("0x%04X, 0x%04X, ", $dword & 65535, $dword >> 16);
    if ($y % 4 == 3) echo "\n";
}

imagegif($dt, "comp.gif");