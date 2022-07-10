#ifndef IFACE_H
#define IFACE_H

void linepro_for_each_line(LineproFunction function,
                           const char*     filename,
                           int*            linenum_ptr,
                           void*           user_ptr);

void lineprof_for_each_line(LineproFunction function,
                            FILE*           file,
                            int*            linenum_ptr,
                            void*           user_ptr);

#endif
