<?php

$core = file_get_contents($argv[1]);
$core = preg_replace('~\s+$~m', '', $core);
file_put_contents($argv[2] ?? $argv[1], $core);
