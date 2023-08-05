<?php

if ($_SERVER["REQUEST_METHOD"] === "POST") {
    $handle = fopen("php://stdin", "r");
    $body = trim(fread($handle, 1024));
    echo $body;
    fclose($handle);
    $pattern = '/name="([^"]+)"\s*\r?\n\r?\n([^-\s]+)/';
    preg_match_all($pattern, $body, $matches, PREG_SET_ORDER);

    $keysAndValues = [];


    foreach ($matches as $match) {
        $key = $match[1];
        $value = $match[2];
        $keysAndValues[$key] = $value;
    }
    print_r($keysAndValues);
}
if ($_SERVER["REQUEST_METHOD"] === "GET" ) {
    $querry = $_SERVER["QUERY_STRING"];
    // Parse the query string into an associative array
    parse_str($querry, $keysAndValues);
    // Output the extracted key-value pairs
    print_r($keysAndValues);
}
?>


