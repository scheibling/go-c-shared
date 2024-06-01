// Include a reference to the PHP API
#include <main/php.h>

// Include the Go interface library
#include "main.h"
#include "libcdyne.h"

static zend_class_entry *cdyne_parent_ce = NULL;
static zend_class_entry *cdyne_child_ce = NULL;
static zend_class_entry *cdyne_exception_ce = NULL;

// Convert the C CdyneException struct to a PHP CdyneException object
void CdyneExceptionToPhp(CdyneException* ce, zval* php) {
    // Create and initialize the PHP object
    zend_class_entry *zce = cdyne_exception_ce;
    object_init_ex(php, zce);

    // Set the properties of the PHP object
    zend_update_property_double(zce, Z_OBJ_P(php), "code", sizeof("code")-1, ce->code);
    zend_update_property_string(zce, Z_OBJ_P(php), "message", sizeof("message")-1, ce->message);
}

// Convert the PHP CdyneException object to a C CdyneException struct
void CdyneExceptionToC(zend_object* php, CdyneException* ce) {
    zval _;
    
    // Read the properties of the PHP object
    // zend_read_property(class_entry, object, property_name, property_name_length, silent, return_value)
    zval *pcode = zend_read_property(cdyne_exception_ce, php, "code", sizeof("code")-1, 0, &_);
    zval *pmsg = zend_read_property(cdyne_exception_ce, php, "message", sizeof("message")-1, 0, &_);
    
    // Convert the properties to C types, and set them on the struct
    // Since the PHP objects are loosely typed, the zval struct has data fields for multiple
    // types of values. The Z_XVAL macros are used to extract the required value from the struct.
    // Z_LVAL extracts a long value from pcode.value.lval
    // Z_STRVAL extracts a string value from pmsg.value.str.val
    ce->code = Z_LVAL(*pcode);
    ce->message = Z_STRVAL(*pmsg);
}

// Convert the C CdyneChild struct to a PHP CdyneChild object
void CdyneChildToPhp(CdyneChild* cc, zval* php) {
    // Reference the zend class entry for the CdyneChild class
    zend_class_entry *zce = cdyne_child_ce;
    
    // Initialize the PHP object with a reference to the correct class entry
    // object_init_ex(zval, zend_class_entry)
    object_init_ex(php, zce);

    // Set the properties of the PHP object from the C struct
    // zend_update_property_string(zend_class_entry, object, property_name, property_name_length, value)
    zend_update_property_string(zce, Z_OBJ_P(php), "key", sizeof("key")-1, cc->key);
    zend_update_property_string(zce, Z_OBJ_P(php), "value", sizeof("value")-1, cc->value);
}

// Convert the PHP CdyneChild object to a C CdyneChild struct
void CdyneChildToC(zend_object* php, CdyneChild* cc) {
    // Read the properties of the PHP object
    zval _;
    
    zval *pkey = zend_read_property(cdyne_child_ce, php, "key", sizeof("key")-1, 0, &_);
    zval *pval = zend_read_property(cdyne_child_ce, php, "value", sizeof("value")-1, 0, &_);
    
    // Convert the properties to C types, and set them on the struct
    cc->key = Z_STRVAL(*pkey);
    cc->value = Z_STRVAL(*pval);
}

// Convert the C CdyneParent struct to a PHP CdyneParent object
void CdyneParentToPhp(CdyneParent* cp, zval* php) {
    // Create and initialize the PHP object
    zend_class_entry *zce = cdyne_parent_ce;
    object_init_ex(php, zce);
    
    // Fetch the properties from the C struct and set them on the PHP object
    // zend_update_property_x(zend_class_entry, object, property_name, property_name_length, value)
    zend_update_property_string(zce, Z_OBJ_P(php), "exampleString", sizeof("exampleString")-1, cp->exampleString);
    zend_update_property_long(zce, Z_OBJ_P(php), "exampleInt", sizeof("exampleInt")-1, cp->exampleInt);
    zend_update_property_double(zce, Z_OBJ_P(php), "exampleDouble", sizeof("exampleDouble")-1, cp->exampleDouble);
    zend_update_property_bool(zce, Z_OBJ_P(php), "exampleBool", sizeof("exampleBool")-1, cp->exampleBool == 1);
    
    // Define and initialize an array to hold the children
    zval children;
    array_init(&children);

    // Set the children property on the PHP object
    // the zend_update_property function is a generic version of the zend_update_property_x functions
    zend_update_property(zce, Z_OBJ_P(php), "children", sizeof("children") - 1, &children);

    // Loop through the list of children, and add them to the previously created zend object
    if (cp->children != NULL) {
        CdyneChild child;

        // Watch for the null terminator, it tells us when the list is finished
        for (int x = 0; cp->children[x].key != NULL; x++) {
            // Create a copy of the item at the current index
            child = cp->children[x];
            
            // Create a PHP object from the C struct
            zval childRef;
            CdyneChildToPhp(&child, &childRef);

            // Append the child to the children array and set the value
            add_next_index_zval(&children, &childRef);
        }
    }
}

// Convert the CdyneParent PHP class to a C CdyneParent struct
void CdyneParentToC(zend_object* php, CdyneParent* cp) {
    zval _;

    // Fetch all properties from the PHP object
    zval *pExString = zend_read_property(cdyne_parent_ce, php, "exampleString", sizeof("exampleString")-1, 0, &_);
    zval *pExInt = zend_read_property(cdyne_parent_ce, php, "exampleInt", sizeof("exampleInt")-1, 0, &_);
    zval *pExDouble = zend_read_property(cdyne_parent_ce, php, "exampleDouble", sizeof("exampleDouble")-1, 0, &_);
    zval *pExBool = zend_read_property(cdyne_parent_ce, php, "exampleBool", sizeof("exampleBool")-1, 0, &_);
    zval *pChildren = zend_read_property(cdyne_parent_ce, php, "children", sizeof("children")-1, 0, &_);

    // Convert the PHP properties to their C counterparts
    cp->exampleString = Z_STRVAL(*pExString);
    cp->exampleInt = Z_LVAL(*pExInt);
    cp->exampleDouble = Z_DVAL(*pExDouble);

    // Convert the PHP boolean to an integer, as the ZEND api for my current version does not seem to have 
    // the Z_BVAL/Z_BVAL_P functions available. The go code does the same on the other side
    cp->exampleBool = Z_TYPE(*pExBool) == IS_TRUE ? 1 : 0;

    // Allocate memory for the children array
    uint32_t count = zend_array_count(Z_ARR_P(pChildren));
    cp->children = (CdyneChild*)malloc(sizeof(CdyneChild) * (count + 1));

    zend_ulong i;
    void *_key;
    zval *val;

    // Loop through the list of children, converting one by one to C structs and putting them back in the list
    ZEND_HASH_FOREACH_KEY_VAL(Z_ARR_P(pChildren), i, _key, val) {
        CdyneChild cElem = cp->children[i];
        CdyneChildToC(Z_OBJ_P(val), &cElem);
        cp->children[i] = cElem;
    } ZEND_HASH_FOREACH_END();

    // Insert null terminator at the end of the list so the Go code knows when to stop reading the list
    CdyneChild nullTerminator;
    nullTerminator.key = NULL;
    nullTerminator.value = NULL;
    cp->children[count] = nullTerminator;
}

// Define the PHP Class for CdyneException, together with the constructor and required arguments
PHP_METHOD(CdyneException, __construct)
{
    // Define the arguments for the constructor
    long code;
    char *message;
    size_t stlen = 256;
    
    // Parse the arguments into the class
    // ZEND_PARSE_PARAMETERS_START(min_num_args, max_num_args)
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_LONG(code)
        Z_PARAM_STRING(message, stlen)
    ZEND_PARSE_PARAMETERS_END();

    // Set the properties on the PHP object
    zend_update_property_long(cdyne_exception_ce, Z_OBJ_P(ZEND_THIS), "code", sizeof("code")-1, code);
    zend_update_property_string(cdyne_exception_ce, Z_OBJ_P(ZEND_THIS), "message", sizeof("message")-1, message);
} ZEND_BEGIN_ARG_INFO(arginfo_cdyne_exception_construct, 0)
    // Define the accepted arguments for this function
    ZEND_ARG_INFO(0, code)
    ZEND_ARG_INFO(0, message)
ZEND_END_ARG_INFO()

// Define a specific PHP class for CdyneException that formats and returns the exception
// in a specific format
PHP_METHOD(CdyneException, AsString) {
    // Fetch the properties from the PHP object
    zval _;
    zval *pcode = zend_read_property(cdyne_exception_ce, Z_OBJ_P(ZEND_THIS), "code", sizeof("code")-1, 0, &_);
    zval *pmsg = zend_read_property(cdyne_exception_ce, Z_OBJ_P(ZEND_THIS), "message", sizeof("message")-1, 0, &_);

    // Create a string buffer to hold the formatted message
    char *buffer = (char*)malloc(256);

    // Format the message
    snprintf(buffer, 256, "CdyneException: %s (code: %ld)", Z_STRVAL_P(pmsg), Z_LVAL_P(pcode));

    // Return the formatted message
    RETURN_STRING(buffer);
} ZEND_BEGIN_ARG_INFO(arginfo_cdyne_exception_AsString, 0)
// No arguments to this function
ZEND_END_ARG_INFO()

// Define the functions that are available in the PHP class for CdyneException
const zend_function_entry cdyne_exception_functions[] = {
    PHP_ME(CdyneException, __construct, arginfo_cdyne_exception_construct, ZEND_ACC_PUBLIC)
    PHP_ME(CdyneException, AsString, arginfo_cdyne_exception_AsString, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

// Define the PHP class for CdyneChild, together with the constructor and required arguments
PHP_METHOD(CdyneChild, __construct)
{
    char *key;
    char *value;
    size_t slen = 256;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(key, slen)
        Z_PARAM_STRING(value, slen)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property_string(cdyne_child_ce, Z_OBJ_P(ZEND_THIS), "key", sizeof("key")-1, key);
    zend_update_property_string(cdyne_child_ce, Z_OBJ_P(ZEND_THIS), "value", sizeof("value")-1, value);
} ZEND_BEGIN_ARG_INFO(arginfo_cdyne_child_construct, 0)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
const zend_function_entry cdyne_child_functions[] = {
    PHP_ME(CdyneChild, __construct, arginfo_cdyne_child_construct, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

// Define the PHP class for CdyneParent, together with the constructor and required arguments
PHP_METHOD(CdyneParent, __construct)
{
    char *exampleString;
    long exampleInt;
    double exampleDouble;
    bool exampleBool;
    zval *children;

    size_t slen = 256;

    ZEND_PARSE_PARAMETERS_START(5, 5)
        Z_PARAM_STRING(exampleString, slen)
        Z_PARAM_LONG(exampleInt)
        Z_PARAM_DOUBLE(exampleDouble)
        Z_PARAM_BOOL(exampleBool)
        Z_PARAM_ARRAY(children)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property_string(cdyne_parent_ce, Z_OBJ_P(ZEND_THIS), "exampleString", sizeof("exampleString")-1, exampleString);
    zend_update_property_long(cdyne_parent_ce, Z_OBJ_P(ZEND_THIS), "exampleInt", sizeof("exampleInt")-1, exampleInt);
    zend_update_property_double(cdyne_parent_ce, Z_OBJ_P(ZEND_THIS), "exampleDouble", sizeof("exampleDouble")-1, exampleDouble);
    zend_update_property_bool(cdyne_parent_ce, Z_OBJ_P(ZEND_THIS), "exampleBool", sizeof("exampleBool")-1, exampleBool);
    zend_update_property(cdyne_parent_ce, Z_OBJ_P(ZEND_THIS), "children", sizeof("children")-1, children);
} ZEND_BEGIN_ARG_INFO(arginfo_cdyne_parent_construct, 0)
    ZEND_ARG_INFO(0, exampleString)
    ZEND_ARG_INFO(0, exampleInt)
    ZEND_ARG_INFO(0, exampleDouble)
    ZEND_ARG_INFO(0, exampleBool)
    ZEND_ARG_INFO(0, children)
ZEND_END_ARG_INFO()
const zend_function_entry cdyne_parent_functions[] = {
    PHP_ME(CdyneParent, __construct, arginfo_cdyne_parent_construct, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

// Define the library main namespace
PHP_MINIT_FUNCTION(Cdyne)
{
    // Initialize the namespace entry for the CdyneException class
    zend_class_entry cee;
    // INIT_NS_CLASS_ENTRY(zend_class_entry, namespace, class_name, function_list)
    INIT_NS_CLASS_ENTRY(cee, "Cdyne", "CdyneException", cdyne_exception_functions)
    // Register the class entry with the PHP engine
    cdyne_exception_ce = zend_register_internal_class(&cee);    
    // Define the accessible properties of the class and whether they are public or private
    zend_declare_property_long(cdyne_exception_ce, "code", strlen("code"), 0, ZEND_ACC_PUBLIC);
    zend_declare_property_string(cdyne_exception_ce, "message", strlen("message"), "", ZEND_ACC_PUBLIC);

    zend_class_entry cce;
    INIT_NS_CLASS_ENTRY(cce, "Cdyne", "CdyneChild", cdyne_child_functions)
    cdyne_child_ce = zend_register_internal_class(&cce);
    zend_declare_property_string(cdyne_child_ce, "key", strlen("key"), "", ZEND_ACC_PUBLIC);
    zend_declare_property_string(cdyne_child_ce, "value", strlen("value"), "", ZEND_ACC_PUBLIC);

    zval *cRef = (zval*)malloc(sizeof(zval));
    zend_class_entry cpe;
    INIT_NS_CLASS_ENTRY(cpe, "Cdyne", "CdyneParent", cdyne_parent_functions)
    cdyne_parent_ce = zend_register_internal_class(&cpe);
    zend_declare_property_string(cdyne_parent_ce, "exampleString", strlen("exampleString"), "", ZEND_ACC_PUBLIC);
    zend_declare_property_long(cdyne_parent_ce, "exampleInt", strlen("exampleInt"), 0, ZEND_ACC_PUBLIC);
    zend_declare_property_double(cdyne_parent_ce, "exampleDouble", strlen("exampleDouble"), 0.0, ZEND_ACC_PUBLIC);
    zend_declare_property_bool(cdyne_parent_ce, "exampleBool", strlen("exampleBool"), 0, ZEND_ACC_PUBLIC);
    zend_declare_property(cdyne_parent_ce, "children", strlen("children"), cRef, ZEND_ACC_PUBLIC);

    return SUCCESS;
}

