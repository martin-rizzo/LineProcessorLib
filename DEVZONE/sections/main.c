
/*
#define LINEPRO_IMPLEMENTATION
#include "linepro.h"
*/
#include "_stdinc.h"
#include "types.h"
#include "iface.h"


void print_line(const char* line, LineproInfo* info) {
    printf("%d: %s\n", info->linenum, line);
}


int main (int argc, char **argv) {
    if (argc>1) { linepro_for_each_line(print_line, argv[1], NULL, NULL); }
    return 0;
}
