
/*
#define LINEPRO_IMPLEMENTATION
#include "linepro.h"
*/
#include "_stdinc.h"
#include "types.h"
#include "iface.h"


void print_text_line(const char* line, LineproInfo* info) {
    printf("%3d: %s\n", info->linenum, line);
}


int main (int argc, char **argv) {
    const char* filename;
    
    filename = (argc>1 ? argv[1] : NULL);
    if (filename) {
        linepro_process_file(filename, print_text_line, "utf8", NULL, NULL);
    }
    return 0;
}
