package gofunc

import "fmt"

// This class/module contains pure Go code, and will communicate with PHP via the interface
// module. This can also be done in the same module if need be, but for clarity it is divided
// into two separate modules here.

// Receive a string from PHP and print it out
func ReceiveString(s string) {
	fmt.Println("[Go] Received string from PHP: ", s)
}

// Receive a string from PHP and return it back with some additions
func ReceiveAndReturnString(s string) string {
	fmt.Printf("[Go] Received string from PHP: \"%s\" now returning it back with some additions\r\n", s)
	return fmt.Sprintf("This is the string that was sent: %s", s)
}

// Receive an error from PHP and print it out
func ReceiveException(e *CdyneException) {
	fmt.Printf("[Go] Received exception from PHP: Code %d Message %s\r\n", e.Code, e.Message)
}

// Send an error from Golang to PHP
func SendException() *CdyneException {
	return &CdyneException{
		Code:    123,
		Message: "This is an error message from Golang",
	}
}

// Receive a more complex type from PHP and print it out
func ReceiveComplex(c *CdyneParent) {
	fmt.Printf("[Go] Received a struct from PHP: \r\n[Go] Example String: %s\r\n[Go] Example Int: %d\r\n[Go] Example Double: %f\r\n[Go] Example Boolean: %t\r\n", c.ExampleString, c.ExampleInt, c.ExampleDouble, c.ExampleBool)
	for _, child := range c.Children {
		fmt.Printf("[Go]    Child Key: \"%s\" has value \"%s\"\r\n", child.Key, child.Value)
	}
}

// Send a complex type from golang to PHP
func SendComplex() *CdyneParent {
	return &CdyneParent{
		ExampleString: "I have been sent from Golang to PHP",
		ExampleInt:    123,
		ExampleDouble: 123.456,
		ExampleBool:   false,
		Children: []*CdyneChild{
			{
				Key:   "I Am",
				Value: "A Child",
			},
			{
				Key:   "I Am",
				Value: "Another Child",
			},
		},
	}
}

// Receive a slice of complex types from PHP and print them out
func ReceiveComplexSlice(csl []*CdyneParent) {
	fmt.Println("[Go] Received a slice of structs from PHP")
	for _, c := range csl {
		ReceiveComplex(c)
	}
}

// Send a slice of complex types from golang to PHP
func SendComplexSlice() []*CdyneParent {
	return []*CdyneParent{
		{
			ExampleString: "I have been sent from Golang to PHP",
			ExampleInt:    123,
			ExampleDouble: 123.456,
			ExampleBool:   true,
			Children: []*CdyneChild{
				{
					Key:   "I Am",
					Value: "A Child",
				},
				{
					Key:   "I Am",
					Value: "Also A Child",
				},
			},
		},
		{
			ExampleString: "I have also been sent from Golang to PHP",
			ExampleInt:    456,
			ExampleDouble: 456.789,
			ExampleBool:   false,
			Children: []*CdyneChild{
				{
					Key:   "I Am",
					Value: "Another Child",
				},
				{
					Key:   "I Am",
					Value: "Also Another Child",
				},
			},
		},
	}
}
