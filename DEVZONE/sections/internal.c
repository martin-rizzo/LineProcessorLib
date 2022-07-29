#include "_stdinc.h"
#include "types.h"
#include "internal.h"
#define HEADER_CODE(x)

/*------------------------ INTERNAL IMPLEMENTATION -------------------------*/

HEADER_CODE(static)
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
    bytesRead = (int)fread(&obj->buffer[bytesToKeep], sizeof(char), bytesToLoad, obj->stream);
    obj->moreDataAvailable = (bytesRead==bytesToLoad);
    obj->nextLine          = obj->buffer;
    obj->bufferEnd         = &obj->buffer[bytesToKeep+bytesRead];
    if (bufferToFree) { free(bufferToFree); }
    return obj->nextLine;
}

HEADER_CODE(static)
LINEPRO_EOL _linepro_select_eol(int count_r, int count_rn, int count_n, int count_nr) {
    int max=count_r; LINEPRO_EOL eol=LINEPRO_EOL_CLASSICMAC;
    if (count_rn>max) { max=count_rn; eol=LINEPRO_EOL_WINDOWS;  }
    if (count_n >max) { max=count_n ; eol=LINEPRO_EOL_UNIX;     }
    if (count_nr>max) { max=count_nr; eol=LINEPRO_EOL_ACORNBBC; }
    if (max==0) { eol=LINEPRO_EOL_UNKNOWN; }
    return eol;
}

HEADER_CODE(static)
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
