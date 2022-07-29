#ifndef IFACE_H
#define IFACE_H

void linepro_process_file(const char*     filename,
                          LineproFunction function,
                          const char *    mode,
                          int *           linenum_ptr,
                          void *          user_ptr);

void linepro_process_stream(FILE*           stream,
                            LineproFunction function,
                            const char *    mode,
                            int *           linenum_ptr,
                            void *          user_ptr);

void linepro_process_buffer(void*           buffer,
                            size_t          bufsize,
                            LineproFunction function,
                            const char *    mode,
                            int *           linenum_ptr,
                            void *          user_ptr);

#endif
