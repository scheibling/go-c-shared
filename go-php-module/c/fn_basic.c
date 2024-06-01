// Include a reference to the PHP API
#include <main/php.h>

// Include the Go interface library
#include "main.h"
#include "libcdyne.h"

// #include <conversion.c>
// #include <classes.c>

// Define the PHP Functions for sending data back and forth
// Go Function ReceiveString (PHP => Go)
ZEND_BEGIN_ARG_INFO(args_str_to_go, 0)
  ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()
ZEND_FUNCTION(str_to_go) {
    char *str;
    size_t slen = 256;

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
    zend_string *result = zend_string_init(rslt, strlen(rslt), 0);
    
    RETURN_STR(result);
}

// Go Function Receive Exception (PHP => Go)
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
    CdyneParent *cp = SendComplex();

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

    uint32_t count = zend_array_count(Z_ARR_P(arr));

    CdyneParent *cps = (CdyneParent*)malloc(sizeof(CdyneParent) * (count + 1));

    zend_ulong i;
    void *_key;
    zval *val;

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARR_P(arr), i, _key, val) {
        CdyneParent cp = cps[i];
        CdyneParentToC(Z_OBJ_P(val), &cp);
        cps[i] = cp;
    } ZEND_HASH_FOREACH_END();

    CdyneParent nullTerminator;
    nullTerminator.exampleString = NULL;
    cps[count] = nullTerminator;

    // Call go function interface
    ReceiveComplexSlice(cps);
    
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

    array_init(&php);

    for (int i = 0; cps[i].exampleString != NULL; i++) {
        current = cps[i];
        zval cRef;

        // Convert the C CdyneParent struct to a PHP CdyneParent object
        CdyneParentToPhp(&current, &cRef);
        add_next_index_zval(&php, &cRef);
    }

    free(cps);

    RETURN_ZVAL(&php, 1, 0);
}