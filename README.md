<h1 align="center">LineProcessorLib</h1>
<p align="center">Portable, one-header library to easily process lines of text from files encoded in any format.</p>
<p align="center">
<img alt="Platform" src="https://img.shields.io/badge/platform-any-33F">
<img alt="Language" src="https://img.shields.io/badge/language-C-22E">
<img alt="License"  src="https://img.shields.io/github/license/martin-rizzo/LineProcessorLib?color=11D">
<img alt="Last"     src="https://img.shields.io/github/last-commit/martin-rizzo/LineProcessorLib">
</p>

<!--
LineProcessorLib
================
-->


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
