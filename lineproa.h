/**
 * @file    lineproa.h
 * @date    Jun 25, 2022
 * @author  Martin Rizzo | <martinrizzo@gmail.com>
 * @license http://www.opensource.org/licenses/mit-license.html [MIT License]
 *//*-------------------------------------------------------------------------
                       LineProcessorLib (ASCII version)
                Portable, one-header library to easily process
                lines of text from files encoded in any format.

     Copyright (c) 2022 Martin Rizzo
  
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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define LINEPRO_BUFSIZE 256

typedef enum LINEPRO_ENCODING {
    LINEPRO_ENCODING_UTF8,         /* < UTF8, ASCI, Windows-1252, ...                 */
    LINEPRO_ENCODING_UTF8_BOM,     /* < UTF8+BOM [confirmed]                          */
    LINEPRO_ENCODING_UTF16_LE,     /* < UTF16 little-endian                           */
    LINEPRO_ENCODING_UTF16_BE,     /* < UTF16 big-endian                              */
    LINEPRO_ENCODING_UTF16_LE_BOM, /* < UTF16+BOM little-endian [confirmed]           */
    LINEPRO_ENCODING_UTF16_BE_BOM, /* < UTF16+BOM big-endian [confirmed]              */
    LINEPRO_ENCODING_BINARY        /* < invalid text file (it's likely a binary file) */
}   LINEPRO_ENCODING;

typedef enum LINEPRO_EOL {
    LINEPRO_EOL_WINDOWS,    /* < '\r\n'  =  MS Windows, DOS, CP/M, OS/2, Atari TOS, ...                   */
    LINEPRO_EOL_UNIX,       /* < '\n'    =  Linux, macOS, BeOS, Amiga, RISC OS, ...                       */
    LINEPRO_EOL_CLASSICMAC, /* < '\r'    =  Classic Mac OS, C64, C128, ZX Spectrum, TRS-80, Apple II, ... */
    LINEPRO_EOL_ACORNBBC,   /* < '\n\r'  =  Acorn BBC                                                     */
    LINEPRO_EOL_UNKNOWN
}   LINEPRO_EOL;

typedef struct LineproInfo {
    int              linenum;
    LINEPRO_ENCODING encoding;
    LINEPRO_EOL      eol;
    unsigned int     isEncodingSupported;
} LineproInfo;

typedef struct LineproObject {
    int              linenum;
    LINEPRO_ENCODING encoding;
    LINEPRO_EOL      eol;
    unsigned int     isEncodingSupported;
    FILE*            file;
    char*            buffer;
    int              bufferSize;
    char*            bufferEnd;
    char*            nextLine;
    unsigned int     moreDataAvailable;
    char*            expandedBuffer;
    char             initialBuffer[LINEPRO_BUFSIZE];
} LineproObject;

typedef void (*LineproFunction)(const char* line, LineproInfo* info);


void linepro_for_each_line(LineproFunction function,
                           const char*     filename,
                           int*            linenum_ptr,
                           void*           user_ptr);

void lineprof_for_each_line(LineproFunction function,
                            FILE*           file,
                            int*            linenum_ptr,
                            void*           user_ptr);


/*------------------------ INTERNAL IMPLEMENTATION -------------------------*/

static
char* _linepro_read_more_data(LineproObject* obj) {
    int bytesToKeep, bytesToLoad, bytesRead;
    char* bufferToFree=NULL;
    
    bytesToKeep = (int)(obj->bufferEnd - obj->nextLine);
    bytesToLoad = (obj->bufferSize-2) - bytesToKeep;
    /* if not enough space to load a line of text -> expand buffer!! */
    if (bytesToLoad==0) {
        bufferToFree = obj->expandedBuffer;
        obj->bufferSize    *= 2;
        obj->expandedBuffer = malloc(obj->bufferSize);
        obj->buffer         = obj->expandedBuffer;
        bytesToLoad = (obj->bufferSize-2) - bytesToKeep;
    }
    if (bytesToKeep) { memmove(obj->buffer, obj->nextLine, bytesToKeep); }
    bytesRead = (int)fread(&obj->buffer[bytesToKeep], sizeof(char), bytesToLoad, obj->file);
    obj->moreDataAvailable = (bytesRead==bytesToLoad);
    obj->nextLine          = obj->buffer;
    obj->bufferEnd         = &obj->buffer[bytesToKeep+bytesRead];
    if (bufferToFree) { free(bufferToFree); }
    return obj->nextLine;
}

static
LINEPRO_EOL _linepro_select_eol(int count_r, int count_rn, int count_n, int count_nr) {
    int max=count_r; LINEPRO_EOL eol=LINEPRO_EOL_CLASSICMAC;
    if (count_rn>max) { max=count_rn; eol=LINEPRO_EOL_WINDOWS;  }
    if (count_n >max) { max=count_n ; eol=LINEPRO_EOL_UNIX;     }
    if (count_nr>max) { max=count_nr; eol=LINEPRO_EOL_ACORNBBC; }
    if (max==0) { eol=LINEPRO_EOL_UNKNOWN; }
    return eol;
}

static
void _linepro_detect_encoding(LineproObject* obj) {
    static const unsigned char UTF8_BOM[]     = { 239, 187, 191 };
    static const unsigned char UTF16_BE_BOM[] = { 254, 255 };
    static const unsigned char UTF16_LE_BOM[] = { 255, 254 };
    int len, count, oddzeros, evenzeros, notext, isEncodingSupported;
    int eol_rn=0, eol_r=0, eol_nr=0, eol_n=0;
    unsigned char *ptr, *start;
    LINEPRO_ENCODING encoding = LINEPRO_ENCODING_BINARY;
    assert( obj!=NULL );
    
    /* detect encoding using BOM (byte order mask) */
    start = (unsigned char*)obj->nextLine;
    len   = (int)(obj->bufferEnd - obj->nextLine);
    if      (len>=3 && memcmp(start,UTF8_BOM    ,3)==0) { encoding=LINEPRO_ENCODING_UTF8_BOM    ; start+=3; }
    else if (len>=2 && memcmp(start,UTF16_BE_BOM,2)==0) { encoding=LINEPRO_ENCODING_UTF16_BE_BOM; start+=2; }
    else if (len>=2 && memcmp(start,UTF16_LE_BOM,2)==0) { encoding=LINEPRO_ENCODING_UTF16_LE_BOM; start+=2; }
    
    /* detect encoding using an heuristic algorithm */
    else {
        oddzeros = evenzeros = notext = 0;
        ptr=start; count=len/2; while (count-->0) {
            if (*ptr==0) { ++oddzeros;  } else if (*ptr<=8 || (14<=*ptr && *ptr<=31)) { ++notext; } ++ptr;
            if (*ptr==0) { ++evenzeros; } else if (*ptr<=8 || (14<=*ptr && *ptr<=31)) { ++notext; } ++ptr;
        }
        if      (oddzeros<(evenzeros/8)) { encoding=LINEPRO_ENCODING_UTF16_LE; }
        else if (evenzeros<(oddzeros/8)) { encoding=LINEPRO_ENCODING_UTF16_BE; }
        else if (notext==0)              { encoding=LINEPRO_ENCODING_UTF8;     }
    }

    /* detect end-of-line for UTF-8 & UTF-8 with BOM */
    if (encoding==LINEPRO_ENCODING_UTF8 || encoding==LINEPRO_ENCODING_UTF8_BOM) {
        ptr=start; for (count=len-2; count>0; --count,++ptr) {
            if      (ptr[0]=='\r') { if (ptr[1]=='\n') { ++eol_rn; } else { ++eol_r; } }
            else if (ptr[0]=='\n') { if (ptr[1]=='\r') { ++eol_nr; } else { ++eol_n; } }
        }
    }
    /* detect end-of-line for UTF-16BE & UTF-16BE with BOM */
    else if (encoding==LINEPRO_ENCODING_UTF16_BE || encoding==LINEPRO_ENCODING_UTF16_BE_BOM) {
        ptr=start; for (count=(len/2)-2; count>0; --count,ptr+=2) {
            if (ptr[0]==0) {
                if      (ptr[1]=='\r') { if (ptr[2]==0 && ptr[3]=='\n') { ++eol_rn; } else { ++eol_r; } }
                else if (ptr[1]=='\n') { if (ptr[2]==0 && ptr[3]=='\r') { ++eol_nr; } else { ++eol_n; } }
            }
        }
    }
    /* detect end-of-line for UTF-16LE & UTF-16LE with BOM */
    else if (encoding==LINEPRO_ENCODING_UTF16_LE || encoding==LINEPRO_ENCODING_UTF16_LE_BOM) {
        ptr=start; for (count=(len/2)-2; count>0; --count,ptr+=2) {
            if (ptr[1]==0) {
                if      (ptr[0]=='\r') { if (ptr[3]==0 && ptr[2]=='\n') { ++eol_rn; } else { ++eol_r; } }
                else if (ptr[0]=='\n') { if (ptr[3]==0 && ptr[2]=='\r') { ++eol_nr; } else { ++eol_n; } }
            }
        }
    }
    /* store results and return */
    isEncodingSupported = (encoding==LINEPRO_ENCODING_UTF8) || (encoding==LINEPRO_ENCODING_UTF8_BOM);
    obj->encoding = encoding;
    obj->eol      = _linepro_select_eol(eol_r, eol_rn, eol_n, eol_nr);
    obj->nextLine = obj->isEncodingSupported ? (char*)start : NULL;
}

/*---------------------------- PUBLIC INTERFACE ----------------------------*/

void linepro_for_each_line(LineproFunction function,
                           const char*     filename,
                           int*            linenum_ptr,
                           void*           user_ptr)
{
    FILE* file;
    file = fopen(filename, "rb");
    if (file!=NULL) {
        lineprof_for_each_line(function, file, linenum_ptr, user_ptr);
        fclose(file);
    }
}

void lineprof_for_each_line(LineproFunction function,
                            FILE*           file,
                            int*            linenum_ptr,
                            void*           user_ptr)
{
    LineproObject* obj;
    char *ptr, *line;
    assert( function!=NULL && file!=NULL );
    
    /* init object */
    obj = malloc(sizeof(LineproObject));
    obj->file              = file;
    obj->buffer            = obj->initialBuffer;
    obj->bufferSize        = LINEPRO_BUFSIZE;
    obj->nextLine          = obj->buffer;
    obj->bufferEnd         = obj->nextLine;
    obj->expandedBuffer    = NULL;
    obj->moreDataAvailable = 0;
         
    /* read first chunk of data */
    _linepro_read_more_data(obj);
    _linepro_detect_encoding(obj);

    while (obj->nextLine!=NULL) {
        ptr = line = obj->nextLine;
        while (ptr==line) {
            /* find end-of-line */
            while (*ptr!='\n' && *ptr!='\r' && ptr<(obj->bufferEnd)) { ++ptr; }
            /* if end-of-line is found -> mark it with a string terminator '\0' */
            if      (*ptr=='\r') { *ptr++='\0'; if (*ptr=='\n') { ++ptr; } }
            else if (*ptr=='\n') { *ptr++='\0'; if (*ptr=='\r') { ++ptr; } }
            /* end-of-line NOT found in buffer */
            /* if more data is available -> load more data into buffer and SEARCH AGAIN (ptr=line) */
            /* otherwise the end of file was reached -> mark it with a string terminator '\0'      */
            else if (obj->moreDataAvailable) { ptr=line=_linepro_read_more_data(obj); }
            else                             { *ptr='\0'; ptr=NULL; /* end-of-file */  }
        }
        obj->nextLine = ptr;
        function(line, (LineproInfo*)obj);
    }
    
    free(obj);
}
