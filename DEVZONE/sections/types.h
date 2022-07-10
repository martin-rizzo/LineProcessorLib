#ifndef TYPES_H
#define TYPES_H

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

#endif
