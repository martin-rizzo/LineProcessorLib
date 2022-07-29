#include "_stdinc.h"
#include "types.h"
#include "iface.h"
#include "internal.h"

/*---------------------------- PUBLIC INTERFACE ----------------------------*/

void linepro_process_file(const char*     filename,
                          LineproFunction function,
                          const char *    mode,
                          int *           linenum_ptr,
                          void *          user_ptr)
{
    FILE* stream;
    assert( filename!=NULL && filename[0]!='\0' && function!=NULL );
    stream = fopen(filename, "rb");
    if (stream) {
        linepro_process_stream(stream, function, mode, linenum_ptr, user_ptr);
        fclose(stream);
    }
}

void linepro_process_stream(FILE*           stream,
                            LineproFunction function,
                            const char *    mode,
                            int *           linenum_ptr,
                            void *          user_ptr)
{
    LineproObject* obj; char *ptr, *line;
    assert( stream!=NULL && function!=NULL );
    
    /* init object */
    obj = malloc(sizeof(LineproObject));
    obj->linenum           = linenum_ptr!=NULL ? (*linenum_ptr) : 1;
    obj->stream            = stream;
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
        function(line, (LineproInfo*)obj); ++obj->linenum;
    }
    
    if (linenum_ptr!=NULL) { (*linenum_ptr)=obj->linenum; }
    free(obj);
}

void linepro_process_buffer(void*           buffer,
                            size_t          bufsize,
                            LineproFunction function,
                            const char *    mode,
                            int *           linenum_ptr,
                            void *          user_ptr)
{
    /* not implemented, yet */
    assert( 0 );
}
