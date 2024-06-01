// Include a reference to the PHP API
#include <main/php.h>

// Include the Go interface library
#include "main.h"
#include "libcdyne.h"

#include "classes.c"
#include "fn_basic.c"

// Define the library name and version
#define LIBCDYNE_NAME "cdyne"
#define LIBCDYNE_VERSION "0.1"

// Register our defined functions that do not belong to a class in the Zend API
zend_function_entry cdyne_functions[] = {
    // Register the basic functions in the Cdyne namespace (ex. Cdyne\str_to_go)
    ZEND_NS_FE("Cdyne", str_to_go, args_str_to_go)
    ZEND_NS_FE("Cdyne", str_to_go_return, args_str_to_go_return)
    ZEND_NS_FE("Cdyne", exception_to_go, args_exception_to_go)
    ZEND_NS_FE("Cdyne", exception_from_go, args_exception_from_go)
    ZEND_NS_FE("Cdyne", complex_to_go, args_complex_to_go)
    ZEND_NS_FE("Cdyne", complex_from_go, args_complex_from_go)
    ZEND_NS_FE("Cdyne", complex_slice_to_go, args_complex_slice_to_go)
    ZEND_NS_FE("Cdyne", complex_slice_from_go, args_complex_slice_from_go)

    // You can also register functions in the global namespace
    ZEND_FE(exception_from_go, args_exception_from_go)
    PHP_FE_END
};

// Define the module entry with all associated functions
zend_module_entry cdyne_module_entry = {
    STANDARD_MODULE_HEADER,
    LIBCDYNE_NAME,
    cdyne_functions,
    PHP_MINIT(Cdyne),
    NULL,
    NULL,
    NULL,
    NULL,
    LIBCDYNE_VERSION,
    STANDARD_MODULE_PROPERTIES
};

// Register the module
// Needs to be the prefix used for {x]_module_entry above
ZEND_GET_MODULE(cdyne);