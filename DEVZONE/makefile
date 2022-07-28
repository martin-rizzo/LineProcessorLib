DEBUG_FLAGS   =
RELEASE_FLAGS = -Os -DNDEBUG

# ansi c compiler
CC     = cc
CFLAGS = $(RELEASE_FLAGS) -ansi -pedantic-errors -Wall

# directories
MAIN_DIR     = ..
BIN_DIR      = ./bin
SECTIONS_DIR = ./sections

# targets
LINEPRO_HEADER = $(MAIN_DIR)/linepro.h
JOIN_SECTIONS  = $(BIN_DIR)/join-sections

# code sections
SECTIONS_FILES= _heading.h _stdinc.h types.h iface.h internal.c iface.c
SECTIONS = $(addprefix $(SECTIONS_DIR)/,$(SECTIONS_FILES))


.PHONY: all header clean

all: header

header: $(LINEPRO_HEADER)

clean:
	rm -f $(LINEPRO_HEADER) $(JOIN_SECTIONS)


$(LINEPRO_HEADER): $(JOIN_SECTIONS) $(SECTIONS)
	$(JOIN_SECTIONS) $(SECTIONS) --output $(LINEPRO_HEADER)


$(JOIN_SECTIONS): join-sections.c
	$(CC) $(CFLAGS) join-sections.c -o $(JOIN_SECTIONS)


	
