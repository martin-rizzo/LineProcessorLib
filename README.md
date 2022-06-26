LineProcessorLib
================
Portable, one-header library to easily process lines of text from files encoded in any format.

[![Platform](https://img.shields.io/badge/platform-any-33F)](https://en.wikipedia.org/wiki/Cross-platform_software)
[![Language](https://img.shields.io/badge/language-C-22E)](https://en.wikipedia.org/wiki/C_(programming_language))
[![License](https://img.shields.io/github/license/martin-rizzo/LineProcessorLib?color=11D)](LICENSE.md)
[![Last Commit](https://img.shields.io/github/last-commit/martin-rizzo/LineProcessorLib)](https://github.com/martin-rizzo/LineProcessorLib/commits/master)


### The library is in development ###

### The library is in development ###


Example
-------

```C
#define LINEPRO_IMPLEMENTATION
#include "linepro.h"

void print_text_line(int linenum, const char* line) {
    printf("%d: %s\n", linenu, line);
}

int main (int argc, char **argv) {
    if (argc>1) { linepro(argv[1], 0, print_text_line); }
    return 0;
}

```
