// Include a reference to the PHP API
#include <main/php.h>

// Include the Go interface library
#include "main.h"
#include "libcdyne.h"

// #include <conversion.c>
// #include <classes.c>

// Define the PHP Functions equivalents of the functions defined in the interface module

// Go Function ReceiveString (PHP => Go)
// Define the arguments for the function
// ZEND_BEGIN_ARG_INFO(arguments_name, allow_null)
ZEND_BEGIN_ARG_INFO(args_str_to_go, 0)
  ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()
// Define the function interface
ZEND_FUNCTION(str_to_go) {
    // Define the variables for the function
    char *str;
    size_t slen = 256;

    // Parse the parameters passed to the function
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(str, slen)
    ZEND_PARSE_PARAMETERS_END();

    // Call go function interface
    ReceiveString(str);
}

// Go Function Receive And Return String (PHP => Go => PHP)
ZEND_BEGIN_ARG_INFO(args_str_to_go_return, 0)
  ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()
ZEND_FUNCTION(str_to_go_return) {
    char *str;
    size_t slen = 256;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(str, slen)
    ZEND_PARSE_PARAMETERS_END();

    // Call go function interface
    char *rslt = ReceiveAndReturnString(str);
    // Parse the resulting char* to a zend_string to pass to a PHP function
    zend_string *result = zend_string_init(rslt, strlen(rslt), 0);
    
    RETURN_STR(result);
}

// Go Function Receive Exception (PHP => Go)
// Pass the parameters, initialize the CdyneException struct, and call the Go function interface
// You could also take the Exception class ass a parameter, like in the CdyneParent example below
ZEND_BEGIN_ARG_INFO(args_exception_to_go, 0)
    ZEND_ARG_INFO(0, code)
    ZEND_ARG_INFO(0, message)
ZEND_END_ARG_INFO()
ZEND_FUNCTION(exception_to_go) {
    long code;
    char *message;
    size_t slen = 256;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_LONG(code)
        Z_PARAM_STRING(message, slen)
    ZEND_PARSE_PARAMETERS_END();

    // Create the CdyneException struct
    CdyneException ce;
    ce.code = code;
    ce.message = message;

    // Call go function interface
    ReceiveException(&ce);
}

// Go Send Error (Go => PHP)
ZEND_BEGIN_ARG_INFO(args_exception_from_go, 0)
ZEND_END_ARG_INFO()
ZEND_FUNCTION(exception_from_go) {
    // Call go function interface
    CdyneException *ce = SendException();

    // Convert the C CdyneException struct to a PHP CdyneException object
    zval php;
    CdyneExceptionToPhp(ce, &php);

    // Return an arbitrary type
    RETURN_ZVAL(&php, 1, 0);
}

// Go Function Receive Complex Struct (CdyneParent with CdyneChild, PHP => Go)
ZEND_BEGIN_ARG_INFO(args_complex_to_go, 0)
    ZEND_ARG_INFO(0, struct)
ZEND_END_ARG_INFO()
ZEND_FUNCTION(complex_to_go) {
    zend_object *pcom;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJ_OF_CLASS(pcom, cdyne_parent_ce)
    ZEND_PARSE_PARAMETERS_END();

    // Create the CdyneParent struct
    CdyneParent cp;

    // Convert the PHP CdyneParent object to a C CdyneParent struct
    CdyneParentToC(pcom, &cp);

    // Call go function interface
    ReceiveComplex(&cp);
}


// Go Function Send Complex Struct (CdyneParent with CdyneChild, Go => PHP)
ZEND_BEGIN_ARG_INFO(args_complex_from_go, 0)
ZEND_END_ARG_INFO()
ZEND_FUNCTION(complex_from_go) {
    // Call go function interface
    CdyneParent* cp = SendComplex();

    // Convert the C CdyneParent struct to a PHP CdyneParent object
    zval php;
    CdyneParentToPhp(cp, &php);

    RETURN_ZVAL(&php, 1, 0);
}

// Go Function Receive Complex Slice ([]CdyneParent, PHP => Go)
ZEND_BEGIN_ARG_INFO(args_complex_slice_to_go, 0)
    ZEND_ARG_INFO(0, arr)
ZEND_END_ARG_INFO()
ZEND_FUNCTION(complex_slice_to_go) {
    zval *arr;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(arr)
    ZEND_PARSE_PARAMETERS_END();

    // Check the length of the source array
    uint32_t count = zend_array_count(Z_ARR_P(arr));

    // Create an array for the classes with the length of the source array + 1 null terminator
    CdyneParent *cps = (CdyneParent*)malloc(sizeof(CdyneParent) * (count + 1));

    // Define variables for the iterator
    zend_ulong i;
    void *_key;
    zval *val;

    // Iterate over the source array and convert each element to a C CdyneParent struct
    ZEND_HASH_FOREACH_KEY_VAL(Z_ARR_P(arr), i, _key, val) {
        CdyneParent cp = cps[i];
        CdyneParentToC(Z_OBJ_P(val), &cp);
        cps[i] = cp;
    } ZEND_HASH_FOREACH_END();

    // Add the null terminator to let the Go function know when the array ends
    CdyneParent nullTerminator;
    nullTerminator.exampleString = NULL;
    cps[count] = nullTerminator;

    // Call go function interface
    ReceiveComplexSlice(cps);
    
    // Free the memory used by the array to avoid leaks
    free(cps);
}


// Go Function Send Complex Slice ([]CdyneParent, Go => PHP)
ZEND_BEGIN_ARG_INFO(args_complex_slice_from_go, 0)
ZEND_END_ARG_INFO()
ZEND_FUNCTION(complex_slice_from_go) {
    // Call go function interface
    CdyneParent *cps = SendComplexSlice();
    if (cps == NULL) {
        RETURN_NULL();
    }

    CdyneParent current;
    zval php;

    // Initialize an empty array
    array_init(&php);

    // Contrary to the previous example, the array is automatically extended on-add instead
    // of being pre-allocated. This is handled by the add_next_index_zval function.
    for (int i = 0; cps[i].exampleString != NULL; i++) {
        current = cps[i];
        zval cRef;

        // Convert the C CdyneParent struct to a PHP CdyneParent object
        CdyneParentToPhp(&current, &cRef);
        add_next_index_zval(&php, &cRef);
    }

    // Free the memory used by the array to avoid leaks
    free(cps);

    RETURN_ZVAL(&php, 1, 0);
}