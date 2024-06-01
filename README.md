# Buildmode c-shared code examples for Golang
This repository contains a list of examples of how to build shared libraries with golang, callable from other languages with buildmode=c-shared.

## Projects
### PHP Extension (PHP8.0++)
Compiles a new PHP extension which send data back and forth between PHP and Golang. It contains some simple functions with arguments and some examples of more advanced types (WIP, some things are broken)

#### Building and installing
```bash
# Install the prerequisites
sudo apt -y install gcc make autoconf libc-dev pkg-config php-dev php-cli
cd go-php-module

# Clean the working directory
make clean

# Build the extension
make

# Install the extension (/usr/lib/php/**/cdyne.so and /usr/local/lib/libcdyne.so)
make install

# Run the test file
make test

# Uninstall the extension
make uninstall

# Cleanup build files
make clean
```
The extension is installed into the following folders:
- /usr/lib/php/PHP_VERSION_FOLDER/cdyne.so (the PHP interface, C)
- /usr/local/lib/cdyne/libcdyne.so (shared library, compiled go code)
- /usr/local/lib/cdyne/cdyne.h (header file for the shared library)
- PHP_INI_DIR/conf.d/99-cdyne.ini (PHP configuration file for loading the extension)
- /etc/ld.so.conf.d/cdyne.conf (ldconfig configuration file for the shared library)

#### Notes
- The slice of CdyneParent does not include children when passed to Go from PHP
