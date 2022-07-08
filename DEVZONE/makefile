CC = cc
RELEASE_FLAGS = -Os -DNDEBUG
ANSI_C_FLAGS  = -ansi -pedantic-errors -Wall
CFLAGS = $(RELEASE_FLAGS) $(ANSI_C_FLAGS)

JOIN_SECTIONS=./bin/join-sections

SECTIONS_DIR=sections/
LINEPRO_UTF8_HEADER=../linepro.h
LINEPRO_ASCII_HEADER=../lineproa.h
UTF8_SOURCES=head-utf.h includes.h types.h iface.h internal.c iface.c
ASCII_SOURCES=head-asc.h includes.h types.h iface.h internal.c iface.c

UTF8_SECTIONS=$(addprefix $(SECTIONS_DIR),$(UTF8_SOURCES))
ASCII_SECTIONS=$(addprefix $(SECTIONS_DIR),$(ASCII_SOURCES))

.PHONY: all headers clean

all: headers

headers: $(LINEPRO_UTF8_HEADER) $(LINEPRO_ASCII_HEADER)

clean:
	rm -f $(LINEPRO_UTF8_HEADER) $(LINEPRO_ASCII_HEADER) $(JOIN_SECTIONS)


$(JOIN_SECTIONS): join-sections.c
	$(CC) $(CFLAGS) join-sections.c -o $(JOIN_SECTIONS)


$(LINEPRO_UTF8_HEADER): $(JOIN_SECTIONS) $(UTF8_SECTIONS)
	$(JOIN_SECTIONS) $(UTF8_SECTIONS) --output $(LINEPRO_UTF8_HEADER)


$(LINEPRO_ASCII_HEADER): $(JOIN_SECTIONS) $(ASCII_SECTIONS)
	$(JOIN_SECTIONS) $(ASCII_SECTIONS) --output $(LINEPRO_ASCII_HEADER)



	




