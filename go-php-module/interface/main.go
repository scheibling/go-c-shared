package main

// #include <stdlib.h>
// #include <main.h>
import "C"

import (
	"fmt"
	gofunc "scheiblingco/gocshared"
	"unsafe"
)

//export ReceiveString
func ReceiveString(s *C.char) {
	gofunc.ReceiveString(C.GoString(s))
}

//export ReceiveAndReturnString
func ReceiveAndReturnString(s *C.char) *C.char {
	return C.CString(gofunc.ReceiveAndReturnString(C.GoString(s)))
}

//export ReceiveException
func ReceiveException(e *C.CdyneException) {
	pErr := &gofunc.CdyneException{}
	CdyneErrorToGo(e, pErr)
	gofunc.ReceiveException(pErr)
}

//export SendException
func SendException() *C.CdyneException {
	gce := gofunc.SendException()
	cce := cNewCdyneException()

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

	CdyneParentToC(ccp, gcp)

	return ccp
}

//export ReceiveComplexSlice
func ReceiveComplexSlice(csl *C.CdyneParent) {
	slRefs := (*[1 << 32]C.CdyneParent)(unsafe.Pointer(csl))
	goSl := []*gofunc.CdyneParent{}

	for i := 0; slRefs[i].exampleString != nil; i++ {
		new := &gofunc.CdyneParent{}
		CdyneParentToGo(&slRefs[i], new)
		goSl = append(goSl, new)
	}

	gofunc.ReceiveComplexSlice(goSl)
}

//export SendComplexSlice
func SendComplexSlice() *C.CdyneParent {
	goSl := gofunc.SendComplexSlice()

	cSl := cNewCdyneParentArray(len(goSl) + 1)
	slRefs := (*[1 << 32]C.CdyneParent)(unsafe.Pointer(cSl))

	for i, parent := range goSl {
		CdyneParentToC(&slRefs[i], parent)
	}

	slRefs[len(goSl)].exampleString = nil

	return cSl
}

func CdyneErrorToGo(cce *C.CdyneException, gce *gofunc.CdyneException) {
	gce.Code = int64(cce.code)
	gce.Message = C.GoString(cce.message)
}

func CdyneErrorToC(cce *C.CdyneException, gce *gofunc.CdyneException) {
	cce.code = C.long(gce.Code)
	cce.message = C.CString(gce.Message)
}

func CdyneChildToGo(ccc *C.CdyneChild, gcc *gofunc.CdyneChild) {
	gcc.Key = C.GoString(ccc.key)
	gcc.Value = C.GoString(ccc.value)
}

func CdyneChildToC(ccc *C.CdyneChild, gcc *gofunc.CdyneChild) {
	ccc.key = C.CString(gcc.Key)
	ccc.value = C.CString(gcc.Value)
}

func CdyneParentToGo(ccp *C.CdyneParent, gcp *gofunc.CdyneParent) {
	gcp.ExampleString = C.GoString(ccp.exampleString)
	gcp.ExampleInt = int(ccp.exampleInt)
	gcp.ExampleDouble = float64(ccp.exampleDouble)
	gcp.ExampleBool = bool(ccp.exampleBool == C.int(1))
	fmt.Println("Received integer ", ccp.exampleBool)
	fmt.Println("Parsing bool as ", gcp.ExampleBool)

	if gcp.Children == nil {
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

func CdyneParentToC(ccp *C.CdyneParent, gcp *gofunc.CdyneParent) {
	ccp.exampleString = C.CString(gcp.ExampleString)
	ccp.exampleInt = C.int(gcp.ExampleInt)
	ccp.exampleDouble = C.double(gcp.ExampleDouble)

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
		CdyneChildToC(&cChildRefs[i], child)
	}
}

func cNewCdyneException() *C.CdyneException {
	return (*C.CdyneException)(
		C.malloc(
			C.size_t(
				unsafe.Sizeof(C.CdyneException{}),
			),
		),
	)
}

func cNewCdyneChild() *C.CdyneChild {
	return (*C.CdyneChild)(
		C.malloc(
			C.size_t(
				unsafe.Sizeof(C.CdyneChild{}),
			),
		),
	)
}

func cNewCdyneParent() *C.CdyneParent {
	return (*C.CdyneParent)(
		C.malloc(
			C.size_t(
				unsafe.Sizeof(C.CdyneParent{}),
			),
		),
	)
}

func cNewCdyneChildArray(length int) *C.CdyneChild {
	return (*C.CdyneChild)(
		C.malloc(
			C.size_t(
				unsafe.Sizeof(C.CdyneChild{}),
			) * C.ulong(length),
		),
	)
}

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
