package main

// #include <stdlib.h>
// #include <main.h>
import "C"

import (
	gofunc "scheiblingco/gocshared"
	"unsafe"
)

// This file contains the functions that will be used to communicate via C with
// the PHP interface. The types are defined in the main.h file, and correspond
// to the types defined in the gofunc package.

// We receive the string as a C.char, and convert it to a Go string
//
//export ReceiveString
func ReceiveString(s *C.char) {
	gofunc.ReceiveString(C.GoString(s))
}

// We receive the string as a C.char, and convert it to a Go string
// We then return the string back to PHP, so we convert it back to a C.char
//
//export ReceiveAndReturnString
func ReceiveAndReturnString(s *C.char) *C.char {
	return C.CString(gofunc.ReceiveAndReturnString(C.GoString(s)))
}

//export ReceiveException
func ReceiveException(e *C.CdyneException) {
	pErr := &gofunc.CdyneException{}

	// Call the conversion function to convert the C struct to a Go struct
	// This is done for all the functions that receive a struct from PHP
	CdyneErrorToGo(e, pErr)

	// Call the gofunc package function with the converted struct
	gofunc.ReceiveException(pErr)
}

//export SendException
func SendException() *C.CdyneException {
	gce := gofunc.SendException()

	// Allocate memory for the C struct
	cce := cNewCdyneException()

	// Call the conversion function to convert the Go struct to a C struct
	CdyneErrorToC(cce, gce)

	return cce
}

//export ReceiveComplex
func ReceiveComplex(c *C.CdyneParent) {
	pParent := &gofunc.CdyneParent{}
	CdyneParentToGo(c, pParent)
	gofunc.ReceiveComplex(pParent)
}

//export SendComplex
func SendComplex() *C.CdyneParent {
	gcp := gofunc.SendComplex()
	ccp := cNewCdyneParent()

	CdyneParentToC(gcp, ccp)

	return ccp
}

// Receives a slice of complex types from PHP
// Since there is no way to know the length of the slice, C only stores it
// in memory as a sequence of structs, we add a null terminator to the end
// of the sequence to know when to terminate the loop

//export ReceiveComplexSlice
func ReceiveComplexSlice(csl *C.CdyneParent) {
	// Create a Go slice with an unsafe pointer to the C array
	// When we iterate, we will keep inching forward until
	// we hit the null terminator
	slRefs := (*[1 << 32]C.CdyneParent)(unsafe.Pointer(csl))
	goSl := []*gofunc.CdyneParent{}

	// Iterate through the C array and convert each struct to a Go struct
	// Check for the null terminator to know when to stop
	for i := 0; slRefs[i].exampleString != nil; i++ {
		new := &gofunc.CdyneParent{}
		CdyneParentToGo(&slRefs[i], new)
		goSl = append(goSl, new)
	}

	gofunc.ReceiveComplexSlice(goSl)
}

// Here, we do the opposite
// We receive a slice of complex types from Go, and convert them to a list of C structs
// We then return a sequence of structs, terminated by a null entry as a way of telling
// the interface on the other side when to stop.

//export SendComplexSlice
func SendComplexSlice() *C.CdyneParent {
	goSl := gofunc.SendComplexSlice()

	cSl := cNewCdyneParentArray(len(goSl) + 1)
	slRefs := (*[1 << 32]C.CdyneParent)(unsafe.Pointer(cSl))

	for i, parent := range goSl {
		CdyneParentToC(parent, &slRefs[i])
	}

	slRefs[len(goSl)].exampleString = nil

	return cSl
}

// Simple conversion function for the CdyneException type
func CdyneErrorToGo(cce *C.CdyneException, gce *gofunc.CdyneException) {
	gce.Code = int64(cce.code)
	gce.Message = C.GoString(cce.message)
}

// Simple conversion function for the CdyneException type
func CdyneErrorToC(cce *C.CdyneException, gce *gofunc.CdyneException) {
	cce.code = C.long(gce.Code)
	cce.message = C.CString(gce.Message)
}

// Conversion functions for the CdyneChild type
func CdyneChildToGo(ccc *C.CdyneChild, gcc *gofunc.CdyneChild) {
	gcc.Key = C.GoString(ccc.key)
	gcc.Value = C.GoString(ccc.value)
}

// Conversion functions for the CdyneChild type
func CdyneChildToC(gcc *gofunc.CdyneChild, ccc *C.CdyneChild) {
	ccc.key = C.CString(gcc.Key)
	ccc.value = C.CString(gcc.Value)
}

// Conversion function for the CdyneParent type
func CdyneParentToGo(ccp *C.CdyneParent, gcp *gofunc.CdyneParent) {
	gcp.ExampleString = C.GoString(ccp.exampleString)
	gcp.ExampleInt = int(ccp.exampleInt)
	gcp.ExampleDouble = float64(ccp.exampleDouble)

	// Convert the bool that we converted to an integer back to a bool
	gcp.ExampleBool = bool(ccp.exampleBool == C.int(1))

	if ccp.children == nil {
		return
	}

	gcp.Children = []*gofunc.CdyneChild{}
	cChildRefs := (*[1 << 32]C.CdyneChild)(unsafe.Pointer(ccp.children))

	for i := 0; cChildRefs[i].key != nil; i++ {
		gChild := gofunc.CdyneChild{}
		CdyneChildToGo(&cChildRefs[i], &gChild)
		gcp.Children = append(gcp.Children, &gChild)
	}
}

// Conversion function for the CdyneParent type
func CdyneParentToC(gcp *gofunc.CdyneParent, ccp *C.CdyneParent) {
	ccp.exampleString = C.CString(gcp.ExampleString)
	ccp.exampleInt = C.int(gcp.ExampleInt)
	ccp.exampleDouble = C.double(gcp.ExampleDouble)

	// Convert the bool to an integer
	ival := 0
	if gcp.ExampleBool {
		ival = 1
	}

	ccp.exampleBool = C.int(ival)

	if gcp.Children == nil {
		return
	}

	ccp.children = cNewCdyneChildArray(len(gcp.Children) + 1)
	cChildRefs := (*[1 << 32]C.CdyneChild)(unsafe.Pointer(ccp.children))

	for i, child := range gcp.Children {
		cChildRefs[i].key = C.CString(child.Key)
		cChildRefs[i].value = C.CString(child.Value)
		// cChildRefs[i] = *cNewCdyneChild()
		// CdyneChildToC(child, &cChildRefs[i])
	}

	cChildRefs[len(gcp.Children)].key = nil
}

// Create a new struct allocation for the CdyneException type
// by checking the length of the struct type
func cNewCdyneException() *C.CdyneException {
	return (*C.CdyneException)(
		C.malloc(
			C.size_t(
				unsafe.Sizeof(C.CdyneException{}),
			),
		),
	)
}

// Create a new struct allocation for the CdyneChild type
func cNewCdyneChild() *C.CdyneChild {
	return (*C.CdyneChild)(
		C.malloc(
			C.size_t(
				unsafe.Sizeof(C.CdyneChild{}),
			),
		),
	)
}

// Create a new struct allocation for the CdyneParent type
func cNewCdyneParent() *C.CdyneParent {
	return (*C.CdyneParent)(
		C.malloc(
			C.size_t(
				unsafe.Sizeof(C.CdyneParent{}),
			),
		),
	)
}

// Create a new struct allocation for the CdyneChild type
func cNewCdyneChildArray(length int) *C.CdyneChild {
	return (*C.CdyneChild)(
		C.malloc(
			C.size_t(
				unsafe.Sizeof(C.CdyneChild{}),
			) * C.ulong(length),
		),
	)
}

// Create a new struct allocation for the CdyneParent type
func cNewCdyneParentArray(length int) *C.CdyneParent {
	return (*C.CdyneParent)(
		C.malloc(
			C.size_t(
				unsafe.Sizeof(C.CdyneParent{}),
			) * C.ulong(length),
		),
	)
}

func main() {}
