<?php
    $querry = $_SERVER["QUERY_STRING"];
    parse_str($querry, $keysAndValues);
    $name = $keysAndValues["session-id"];
    $randomString = bin2hex(random_bytes(10));
    $data = $name . $randomString;
    $token = password_hash($data, PASSWORD_BCRYPT);
    echo "Set-Cookie: session_Id=".$name.";Expires=Wed, 21 Oct 2025 07:28:00 GMT; Path=/";
?>