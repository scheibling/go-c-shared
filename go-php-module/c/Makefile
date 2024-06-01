# Base Configuration
CURRENT_DIR 	= $(shell pwd)
INSTALL_DIR 	= $(CURRENT_DIR)/build/shtool mkdir -p

EGREP 			= $(shell which grep) -E
SED 			= $(shell which sed)
AWK 			= $(shell which nawk)
LIBTOOL 		= /bin/bash $(shell which libtool)

PHP_EXTENSION_NAME = cdyne
PHP_LIB_DIR = $(PWDIR)/modules
PHP_MODULES = $(PHP_LIB_DIR)/cdyne.la
PHP_EXTENSION_DIR = $(shell php-config --extension-dir)
PHP_INCLUDE_DIR = $(shell php-config --include-dir)
PHP_EXECUTABLE = $(shell which php)
PHP_LIB_INCLUDES = -I$(PHP_INCLUDE_DIR)/main -I$(PHP_INCLUDE_DIR)/TSRM -I$(PHP_INCLUDE_DIR)/Zend -I$(PHP_INCLUDE_DIR)
CFLAGS = -g -O2 $(EXTRA_CFLAGS)
CPPFLAGS = -DHAVE_CONFIG_H
COMMON_FLAGS=-I$(PWDIR)/include -I$(PWDIR)/main -I$(PWDIR) $(PHP_LIB_INCLUDES) $(CPPFLAGS) 

%_arginfo.h: %.stub.php
	@if test -e "$(PWDIR)/build/gen_stub.php"; then \
		if test ! -z "$(PHP_EXECUTABLE)"; then \
			echo Parse $< to generate $@; \
			$(PHP_EXECUTABLE) $(PWDIR)/build/gen_stub.php $<; \
		elif type php >/dev/null 2>/dev/null; then \
			if test `php -v | head -n1 | cut -d " " -f 2 | $(SED) "s/$$/\n7.0.99/" | sort -rV | head -n1` != "7.0.99"; then \
				echo "PHP 7.1.0 or newer is required for nullable return types"; \
				exit 1; \
			fi; \
		fi; \
	fi;

%.c: %_arginfo.h
	@touch $@

.NOEXPORT:
main.lo:
	@echo "Compiling main.c $(PHP_MODULES) moddir"
	$(LIBTOOL) --tag=CC --mode=compile cc -I. -I$(PWDIR) $(COMMON_FLAGS) $(CFLAGS) -c $(PWDIR)/main.c -o main.lo
	$(LIBTOOL) --tag=CC --mode=link cc -shared $(COMMON_FLAGS) $(CFLAGS) -o cdyne.la -export-dynamic -avoid-version -prefer-pic -module -rpath $(PHP_LIB_DIR) main.lo 
	$(LIBTOOL) --tag=CC --mode=install cp ./cdyne.la $(PHP_LIB_DIR)
	@echo ""
	@echo ""

.PHONY: install
install: main.lo
	@echo "Installing the library"
	sudo cp ./$(PHP_EXTENSION_NAME).so $(PHP_EXTENSION_DIR)

.PHONY: uninstall
uninstall:
	@echo "Uninstalling the library"
	sudo rm -f $(PHP_EXTENSION_DIR)/$(PHP_EXTENSION_NAME).so

