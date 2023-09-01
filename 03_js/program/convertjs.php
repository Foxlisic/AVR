<?php

$bin = file_get_contents($argv[1]);
for ($i = 0; $i < strlen($bin); $i += 2) {
    $out[$i>>4][] = sprintf("0x%04X", ord($bin[$i]) + 256*ord($bin[$i+1]));
}

foreach ($out as $i => $v) $out[$i] = join(", ", $v);

file_put_contents($argv[2], "const program = [\n  ".join(",\n  ", $out)."\n];");
