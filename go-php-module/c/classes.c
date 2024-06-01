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
    zend_class_entry *zce = cdyne_exception_ce;
    object_init_ex(php, zce);

    zend_update_property_double(zce, Z_OBJ_P(php), "code", sizeof("code")-1, ce->code);
    zend_update_property_string(zce, Z_OBJ_P(php), "message", sizeof("message")-1, ce->message);
}

// Convert the PHP CdyneException object to a C CdyneException struct
void CdyneExceptionToC(zend_object* php, CdyneException* ce) {
    zval _;
    
    zval *pcode = zend_read_property(cdyne_exception_ce, php, "code", sizeof("code")-1, 0, &_);
    zval *pmsg = zend_read_property(cdyne_exception_ce, php, "message", sizeof("message")-1, 0, &_);
    
    ce->code = Z_LVAL(*pcode);
    ce->message = Z_STRVAL(*pmsg);
}


// Convert the C CdyneChild struct to a PHP CdyneChild object
void CdyneChildToPhp(CdyneChild* cc, zval* php) {
    zend_class_entry *zce = cdyne_child_ce;
    object_init_ex(php, zce);

    zend_update_property_string(zce, Z_OBJ_P(php), "key", sizeof("key")-1, cc->key);
    zend_update_property_string(zce, Z_OBJ_P(php), "value", sizeof("value")-1, cc->value);
}

// Convert the PHP CdyneChild object to a C CdyneChild struct
void CdyneChildToC(zend_object* php, CdyneChild* cc) {
    zval _;
    
    zval *pkey = zend_read_property(cdyne_child_ce, php, "key", sizeof("key")-1, 0, &_);
    zval *pval = zend_read_property(cdyne_child_ce, php, "value", sizeof("value")-1, 0, &_);
    
    cc->key = Z_STRVAL(*pkey);
    cc->value = Z_STRVAL(*pval);
}

// Convert the C CdyneParent struct to a PHP CdyneParent object
void CdyneParentToPhp(CdyneParent* cp, zval* php) {
    zend_class_entry *zce = cdyne_parent_ce;
    object_init_ex(php, zce);
    
    zend_update_property_string(zce, Z_OBJ_P(php), "exampleString", sizeof("exampleString")-1, cp->exampleString);
    zend_update_property_long(zce, Z_OBJ_P(php), "exampleInt", sizeof("exampleInt")-1, cp->exampleInt);
    zend_update_property_double(zce, Z_OBJ_P(php), "exampleDouble", sizeof("exampleDouble")-1, cp->exampleDouble);
    zend_update_property_bool(zce, Z_OBJ_P(php), "exampleBool", sizeof("exampleBool")-1, cp->exampleBool == 1);
    
    // Define the children array
    zval children;
    array_init(&children);
    zend_update_property(zce, Z_OBJ_P(php), "children", sizeof("children") - 1, &children);

    if (cp->children != NULL) {
        CdyneChild child;
        for (int x = 0; ; x++) {
            child = cp->children[x];
            
            if (child.key == NULL) {
                break;
            }

            zval childRef;
            CdyneChildToPhp(&child, &childRef);
            add_next_index_zval(&children, &childRef);
        }
    }
}

// Convert the CdyneParent PHP class to a C CdyneParent struct
void CdyneParentToC(zend_object* php, CdyneParent* cp) {
    zval _;

    zval *pExString = zend_read_property(cdyne_parent_ce, php, "exampleString", sizeof("exampleString")-1, 0, &_);
    zval *pExInt = zend_read_property(cdyne_parent_ce, php, "exampleInt", sizeof("exampleInt")-1, 0, &_);
    zval *pExDouble = zend_read_property(cdyne_parent_ce, php, "exampleDouble", sizeof("exampleDouble")-1, 0, &_);
    zval *pExBool = zend_read_property(cdyne_parent_ce, php, "exampleBool", sizeof("exampleBool")-1, 0, &_);
    zval *pChildren = zend_read_property(cdyne_parent_ce, php, "children", sizeof("children")-1, 0, &_);

    cp->exampleString = Z_STRVAL(*pExString);
    cp->exampleInt = Z_LVAL(*pExInt);
    cp->exampleDouble = Z_DVAL(*pExDouble);
    cp->exampleBool = 1;

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

    // Insert null terminator at the end of the list so the Go code knows when to stop reading
    CdyneChild nullTerminator;
    nullTerminator.key = NULL;
    nullTerminator.value = NULL;
    cp->children[count] = nullTerminator;
}

// Define the PHP Classes
PHP_METHOD(CdyneException, __construct)
{
    long code;
    char *message;
    size_t stlen = 256;
    
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_LONG(code)
        Z_PARAM_STRING(message, stlen)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property_long(cdyne_exception_ce, Z_OBJ_P(ZEND_THIS), "code", sizeof("code")-1, code);
    zend_update_property_string(cdyne_exception_ce, Z_OBJ_P(ZEND_THIS), "message", sizeof("message")-1, message);
} ZEND_BEGIN_ARG_INFO(arginfo_cdyne_exception_construct, 0)
    ZEND_ARG_INFO(0, code)
    ZEND_ARG_INFO(0, message)
ZEND_END_ARG_INFO()
const zend_function_entry cdyne_exception_functions[] = {
    PHP_ME(CdyneException, __construct, arginfo_cdyne_exception_construct, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

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
    zend_class_entry cee;
    INIT_NS_CLASS_ENTRY(cee, "Cdyne", "CdyneException", cdyne_exception_functions)
    cdyne_exception_ce = zend_register_internal_class(&cee);    
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

