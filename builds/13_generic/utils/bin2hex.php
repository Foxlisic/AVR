<?php

$file = isset($argv[1]) ? $argv[1] : stdin;
$file = file_get_contents($file);
$size = (int)(strlen($file) >> 1);

$out = '';
for ($i = 0; $i < $size; $i++) {

    $val = ord($file[2*$i]) + 256*ord($file[2*$i + 1]);
    $out .= sprintf("%04X\n", $val);
}

file_put_contents($argv[2], $out);