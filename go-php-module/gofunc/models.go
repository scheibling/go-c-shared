package gofunc

// Define the go equivalents of the C classes defined in the interface class

type CdyneException struct {
	Code    int64
	Message string
}

type CdyneParent struct {
	ExampleString string
	ExampleInt    int
	ExampleDouble float64
	ExampleBool   bool
	Children      []*CdyneChild
}

type CdyneChild struct {
	Key   string
	Value string
}
