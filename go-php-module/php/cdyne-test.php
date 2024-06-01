<?php

// Send an exception to the go function ReceiveException
$x = Cdyne\exception_to_go(1, "This is an error message");

// Receive an exception from the go function SendException
$ex = Cdyne\exception_from_go();
echo "[PHP] Exception from Go: Code " . $ex->code . " Message " . $ex->message . "\r\n";

// Send a string to the go function ReceiveString
Cdyne\str_to_go("Hello, World!\r\n");

// Send and receive a string from the go function ReceiveAndReturnString
$aString = "I will be sent to a go function, modified and returned";
$returnString = Cdyne\str_to_go_return($aString);
echo "[PHP] Returned string: " . $returnString . "\r\n";

// Create a complex type to send to the go function ReceiveComplexType
// The boolean is still a little bit broken
$ctype = new Cdyne\CdyneParent("IsTrue", 1, 1.1, true, [
    new Cdyne\CdyneChild("I am", "Child 1"),
    new Cdyne\CdyneChild("I Am", "Child 2")
]);
Cdyne\complex_to_go($ctype);

// Receive a complex type from the go function SendComplexType
$ctype = Cdyne\complex_from_go();
echo "\r\n\r\n[PHP] Complex type from Go: " . 
    $ctype->exampleString . " " . 
    $ctype->exampleInt . " " . 
    $ctype->exampleDouble . " " . 
    $ctype->exampleBool . "\n";

foreach ($ctype->children as $child) {
    echo "[PHP]\tChild: (Key)" . $child->key . " (Value)" . $child->value . "\n";
}

// Create a complex array/slice and send to go
// Nested children do not currently work
$ctypeArray = [
    new Cdyne\CdyneParent("I am parent 1", 1, 1.1, true, [
        new Cdyne\CdyneChild("I am", "Child 1,1"),
        new Cdyne\CdyneChild("I am", "Child 1,2")
    ]),
    new Cdyne\CdyneParent("I am parent 2", 2, 2.2, false, [
        new Cdyne\CdyneChild("I Am", "child 2,1"),
        new Cdyne\CdyneChild("I Am", "child 2,2")
    ])
];

Cdyne\complex_slice_to_go($ctypeArray);

// Receive a complex array/slice from go	
$ctypeArray = Cdyne\complex_slice_from_go();
foreach ($ctypeArray as $ctype) {
    echo "\r\n\r\n[PHP] Complex type from Go: " . 
        $ctype->exampleString . " " . 
        $ctype->exampleInt . " " . 
        $ctype->exampleDouble . " " . 
        $ctype->exampleBool . "\n";

    foreach ($ctype->children as $child) {
        echo "[PHP]\tChild: (Key)" . $child->key . " (Value)" . $child->value . "\n";
    }
}