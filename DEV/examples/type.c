/**
 * @file      type.c
 * @date      Jun 25, 2022
 * @author    Martin Rizzo | <martinrizzo@gmail.com>
 * @copyright Copyright (c) 2022 Martin Rizzo.
 * @license   http://www.opensource.org/licenses/mit-license.html MIT License
 *//*-------------------------------------------------------------------------
            This code is an example of use of LineProcessorLib
  
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
#include <stdio.h>

/* include the LineProcessorLib implementation */
#define LINEPRO_IMPLEMENTATION
#include "linepro.h"



void process_text_line(int linenum, const char* content) {
    printf("%s\n", content);
}


int main (int argc, char **argv) {
    if (argc>1 && *argv[1]!='\0') {
        linepro(argv[1], 0, process_text_line);
    }
    return 0;
}


