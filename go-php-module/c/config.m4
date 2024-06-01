PHP_ARG_ENABLE(cdyne, Enable the Cdyne extension, [ --enable-cdyne Enable the Cdyne extension])

if test "$CDYNE" != "no"; then
    PHP_NEW_EXTENSION(cdyne, cdyne.c, $ext_shared)
fi