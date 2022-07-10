/**
 * @file    join-sections.c
 * @date    Jul 21, 2019
 * @author  Martin Rizzo | <martinrizzo@gmail.com>
 * @license http://www.opensource.org/licenses/mit-license.html [MIT License]
 *//*-------------------------------------------------------------------------
                Join multiple code sections in one only header
  
     Copyright (c) 2019-2022 Martin Rizzo
  
     Permission is hereby granted, free of charge, to any person obtaining
     a copy of this software and associated documentation files (the
     "Software"), to deal in the Software without restriction, including
     without limitation the rights to use, copy, modify, merge, publish,
     distribute, sublicense, and/or sell copies of the Software, and to
     permit persons to whom the Software is furnished to do so, subject to
     the following conditions:
  
     The above copyright notice and this permission notice shall be
     included in all copies or substantial portions of the Software.
  
     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
     EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
     MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
     IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
     CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
     TORT OR OTHERWISE, ARISING FROM,OUT OF OR IN CONNECTION WITH THE
     SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define APP_NAME "join-sections"
#define APP_VERSION "1.0"
#if !defined(MAX_FILE_COUNT)
#    define  MAX_FILE_COUNT 256
#endif

static const char help[]="\n\
Usage: "APP_NAME" [OPTIONS] sour1.c sour2.c ...\n\
\n\
Joins multiple sections of code in one only header.\n\
\n\
Options:\n\
  -o, --output <file>  Write output to <file>\n\
  -h, --help           Print this help\n\
  -v, --version        Print version\n\
\n\
Examples:\n\
  "APP_NAME" sections/types.h sections/iface.c sections/internal.c -o output.h\n\
";

typedef enum Command {
    CMD_JOIN_SECTIONS,
    CMD_SHOW_HELP,
    CMD_PRINT_VERSION,
    CMD_PRINT_ERROR
} Command;

enum { FALSE=0, TRUE=1 };

/*--------------------------------- ERRORS ---------------------------------*/

typedef enum Error {
    NO_ERROR,
    ERR_UNKNOWN,
    ERR_TOO_MUCH_FILES,
    ERR_UNKNOWN_OPTION,
    ERR_CANT_OPEN_INPUT_FILE,
    ERR_CANT_OPEN_OUTPUT_FILE,
    ERR_LIMIT /* <- must be the last constant */
} Error;

const char* error_msg[] = {
    "No error",
    "Unknown error",
    "Too much files to join",
    "Unknown option '%s'",
    "Can not open file '%s' for reading",
    "Can not open file '%s' for writing"
};

/*---------------------------- ??????????????? ----------------------------*/

#define is_newline(x) ((x)=='\n' || (x)=='\r')
#define is_blank(x)   ((x)==' '  || (x)=='\t')

char* get_next_line(char* ptr, const char* end) {
    while (ptr<end && !is_newline(*ptr)) { ++ptr; }
    if (ptr<end) { ptr += (is_newline(ptr[1]) && ptr[1]!=ptr[0]) ? 2 : 1; }
    return ptr;
}

/*---------------------------- SECTION HANDLING ----------------------------*/

typedef struct Section {
    char* begin;
    char* end;
    char  buffer[1];
} Section;

Section* load_new_section(const char* filename) {
    FILE* file; Section* section; long length;
    assert( filename!=NULL && filename[0]!='\0' );
    
    file = fopen(filename, "rb");
    if (!file) { return NULL; }
    
    /* calculate file length */
    fseek(file, 0, SEEK_END);
    length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    /* load the entire file into memory and adjust length */
    section = malloc(sizeof(Section)+length+1);
    if (!section) { fclose(file); return NULL; }
    length = fread(section->buffer, 1, length, file);
    section->buffer[length]='\0';
    length = strlen(section->buffer);

    /* close file & return section pointers begin/end */
    section->begin = &section->buffer[0];
    section->end   = &section->buffer[length];
    assert( (*section->end)=='\0' );
    fclose(file);
    return section;
}

void append_section_to_file(FILE* out_file, const Section* section, const char* section_name) {
    long length;
    assert( out_file!=NULL && section!=NULL && section_name!=NULL );
    length = (section->end - section->begin);
    if (out_file!=stdout) { printf(" # appending '%s'\n", section_name); }
    fwrite(section->begin, 1, length, out_file);
}

void trim_section(Section* section) {
    char* ptr; const char* end; int preprocesor;
    assert( section!=NULL && section->begin!=NULL && section->end!=NULL );
    
    ptr = section->begin;
    end = section->end;
    preprocesor=TRUE; while (ptr<end && preprocesor) {
        /* skip spaces/tabs and verify preprocesor directive */
        while (ptr<end && is_blank(*ptr)) { ++ptr; }
        preprocesor = (*ptr=='#');
        if (preprocesor) { section->begin = ptr = get_next_line(ptr,end); }
    }
}

void resolve_macros_in_section(Section* section) {
    static const char macro_name[]="HEADER_CODE";
    static const int  macro_name_len=11;
    char* dest; const char *sour, *end;
    int inside, outside;
    assert( section!=NULL && section->begin!=NULL && section->end!=NULL );
    
    sour = dest = section->begin;
    end  = (section->end - macro_name_len);
    while (sour<end) {
        if (0!=memcmp(sour, macro_name, macro_name_len)) { *dest++=*sour++; }
        else {
            sour+=macro_name_len;
            inside=outside=0;
            while (sour<end && !outside) {
                if (*sour==')') { outside=(--inside<=0); }
                if (inside) { *dest++=*sour; }
                if (*sour=='(') { ++inside; }
                ++sour;
            }
        }
    }
    end = section->end;
    while (sour<end) { *dest++=*sour++; }
    section->end=dest;
    (*section->end)='\0';
}

/*-------------------------------- COMMANDS --------------------------------*/

Error show_help(void) {
    printf("%s",help);
    return NO_ERROR;
}

Error print_version(void) {
    printf("%s v%s\n", APP_NAME, APP_VERSION);
    return NO_ERROR;
}

Error print_error(Error error, const char* param) {
    if (error<0 || ERR_LIMIT<=error) { error=ERR_UNKNOWN; }
    if (error) {
        fprintf(stderr,"ERROR: ");
        fprintf(stderr,error_msg[error],(param!=NULL?param:""));
        fprintf(stderr,"\n");
    }
    return error;
}

Error join_sections(const char*  out_filename,
                    const char** filenames,
                    int          filenames_cnt)
{
    FILE* out_file; Section* section; const char* section_name;
    Error error=NO_ERROR; const char* errparam=0;
    int i;

    out_file = out_filename ? fopen(out_filename, "wb") : stdout;
    if (!out_file) { error=ERR_CANT_OPEN_OUTPUT_FILE; errparam=out_filename; }
    
    for (i=0; i<filenames_cnt && !error; ++i) {
        section_name = filenames[i];
        section      = load_new_section( section_name );
        if (section) {
            trim_section(section);
            resolve_macros_in_section(section);
            append_section_to_file(out_file, section, section_name);
            free(section);
        }
        else { error=ERR_CANT_OPEN_INPUT_FILE; errparam=filenames[i]; }
        
    }
    if (out_file!=NULL && out_file!=stdout) { fclose(out_file); }
    return print_error(error,errparam);
}

/*--------------------------------------------------------------------------*/

#define OPT_EQ(arg,short,long) (0==strcmp(arg,short) || 0==strcmp(arg,long))
#define IS_OPT(arg) (arg[0]=='-')

int main(int argc, char **argv) {
    Command command=CMD_JOIN_SECTIONS;
    Error error=NO_ERROR; const char* errparam=0;
    const char *opt;
    const char* out_filename=NULL;
    const char* filenames[MAX_FILE_COUNT]; int filenames_cnt=0;
    int i, ignore_opts;
    
    ignore_opts=FALSE;
    i=1; while (i<argc) {
        opt=argv[i++];
        if (ignore_opts || !IS_OPT(opt)) {
            if (filenames_cnt<MAX_FILE_COUNT) { filenames[filenames_cnt++]=opt; }
            else { command=CMD_PRINT_ERROR; error=ERR_TOO_MUCH_FILES; errparam=0; }
        }
        else if(OPT_EQ(opt,"-o","--output" )) { if (i<argc) out_filename=argv[i++]; }
        else if(OPT_EQ(opt,"-h","--help"   )) { command=CMD_SHOW_HELP;     }
        else if(OPT_EQ(opt,"-v","--version")) { command=CMD_PRINT_VERSION; }
        else if(OPT_EQ(opt,"--","--")       ) { ignore_opts=TRUE;          }
        else { command=CMD_PRINT_ERROR; error=ERR_UNKNOWN_OPTION; errparam=opt; }
    }
    switch (command) {
        case CMD_SHOW_HELP:     return show_help();
        case CMD_PRINT_VERSION: return print_version();
        case CMD_PRINT_ERROR:   return print_error(error, errparam);
        case CMD_JOIN_SECTIONS: return join_sections(out_filename, filenames, filenames_cnt);
        default:                return NO_ERROR;
    }
}
