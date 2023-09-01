<?php
 
$data = file_get_contents($argv[1]);
$len  = strlen($data);

for ($i = 0; $i < $len; $i += 2) {
    
    $hex = ord($data[$i]) + 256*ord($data[$i+1]);
    $out .= sprintf("%04x\n", $hex);    
}

file_put_contents($argv[2], $out);