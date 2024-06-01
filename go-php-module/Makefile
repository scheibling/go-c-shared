.DEFAULT_GOAL: all

PHP_INI_DIR = $(shell php-config --ini-dir)

.PHONY: all
all: interface gcc
	@echo "Finished building. Run make install to install your module, or make clean to remove build files"

.PHONY: interface
interface:
	@echo "Building c-go interface"
	cd interface && go build -o libcdyne.so -buildmode=c-shared ./main.go
	@echo "Done"

.PHONY: gcc
gcc:
	@echo "Building c code"
	cd ./c && EXTRA_CFLAGS="-I`pwd`/../interface -L`pwd`/../interface -lcdyne" PWDIR="`pwd`" make
	@echo "Done"

.PHONY: clean
clean:
	@echo "Cleaning up"
	bash -c "rm -f interface/{libcdyne.so,libcdyne.h,main.so}"
	bash -c "rm -rf c/*.{so,lo,o,la} c/.libs"
	@echo "Done"

.PHONY: install
install:
	@echo "Installing the library"
	sudo mkdir -p /usr/local/lib/cdyne
	sudo cp interface/libcdyne.* /usr/local/lib/cdyne/
	echo "/usr/local/lib/cdyne" | sudo tee /etc/ld.so.conf.d/cdyne.conf
	echo "priority=99\r\nextension=cdyne.so" | sudo tee $(PHP_INI_DIR)/99-cdyne.ini
	sudo ldconfig
	cd c && sudo make install
	@echo "Successfully installed the library!"

.PHONY: uninstall
uninstall:
	@echo "Uninstalling the library"
	sudo rm -rf /usr/local/lib/cdyne /etc/ld.so.conf.d/cdyne.conf
	cd c && sudo make uninstall
	@echo "Successfully uninstalled the library!"

.PHONY: test
test:
	@echo "Running the PHP test file"
	php -d extension=cdyne.so php/cdyne-test.php
	@echo "Done"


.PHONY: cbtest
cbtest: clean interface gcc install test