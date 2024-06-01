<?php

function assert_ok($ok, $message, $end = false) {
    
    if ($ok) {
        echo "[PHP] Test passed\r\n";
    } else {
        echo "[PHP] Test failed: {$message}\r\n";
    }

    if ($end) {
        echo "---------------------------------------------------------\r\n\r\n";
    }
}

// Send an exception to the go function ReceiveException
$x = Cdyne\exception_to_go(1, "This is an error message");

// Receive an exception from the go function SendException
$ex = Cdyne\exception_from_go();
echo "[PHP] Exception from Go: Code " . $ex->code . " Message " . $ex->message . "\r\n";
assert_ok($ex->code == 123, "Invalid return code");
assert_ok($ex->message == "This is an error message", "Invalid return message", true);


// Try the method we put in the global namespace
$exf = exception_from_go();
echo "[PHP] Exception from Go: Code " . $exf->code . " Message " . $exf->message . "\r\n";
assert_ok($exf->code == 123, "Invalid return code");
assert_ok($exf->message == "This is an error message from Golang", "Invalid return message", true);

// Create an exception to test the formatting function
$ex = new Cdyne\CdyneException(123, "I have failed spectacularily");
echo "[PHP] " . $ex->AsString() . "\r\n";
assert_ok($ex->AsString() == "Code: 123 Message: I have failed spectacularily", true);

// Send a string to the go function ReceiveString
Cdyne\str_to_go("Hello, World!");

// Send and receive a string from the go function ReceiveAndReturnString
$aString = "I will be sent to a go function, modified and returned";
$returnString = Cdyne\str_to_go_return($aString);
echo "[PHP] Returned string: " . $returnString . "\r\n";
assert_ok($returnString == "This is the string that was sent: I will be sent to a go function, modified and returned", "Invalid return string", true);

// Create a complex type to send to the go function ReceiveComplexType
// The boolean is still a little bit broken
$ctype = new Cdyne\CdyneParent("IsTrue", 1, 1.1, true, [
    new Cdyne\CdyneChild("I am", "Child 1"),
    new Cdyne\CdyneChild("I Am", "Child 2")
]);
Cdyne\complex_to_go($ctype);

$ctype = new Cdyne\CdyneParent("IsFalse", 1, 1.1, false, [
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

assert_ok($ctype->exampleString === "I have been sent from Golang to PHP", "Invalid exampleString");
assert_ok($ctype->exampleInt === 123, "Invalid exampleInt");
assert_ok($ctype->exampleDouble === 123.456, "Invalid exampleDouble");
assert_ok($ctype->exampleBool === false, "Invalid exampleBool");
assert_ok(count($ctype->children) == 2, "Invalid number of children", true);

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

assert_ok(count($ctypeArray) == 2, "Invalid number of elements in the array", true);
assert_ok($ctypeArray[0]->exampleString === "I have been sent from Golang to PHP", "Invalid exampleString");
assert_ok($ctypeArray[1]->exampleString === "I have also been sent from Golang to PHP", "Invalid exampleString");
assert_ok($ctypeArray[0]->exampleInt === 123, "Invalid exampleInt");
assert_ok($ctypeArray[1]->exampleInt === 456, "Invalid exampleInt", true);