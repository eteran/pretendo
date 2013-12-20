
SHELL=/bin/sh

CC      := gcc -ansi
LD      := gcc
MKDIR   := mkdir -p
OBJ_DIR := .obj

.SUFFIXES:

$(OBJ_DIR)/%.o: %.c
	@echo compiling $< ...
	@$(CC) $(CFLAGS) -MMD -c $< -o $@

TARGET = libines.a

CFLAGS  := -pedantic -W -Wall -O2 -g3
LDFLAGS := 
C_FILES := ines_crc32.c lib_ines.c load_ines.c
H_FILES := ines_crc32.h lib_ines.h load_ines.h ines_types.h 

O_FILES := $(patsubst %.c, $(OBJ_DIR)/%.o, $(C_FILES))
D_FILES := $(O_FILES:.o=.d)

SOURCEFILES :=	$(H_FILES) $(C_FILES)
.PRECIOUS:	$(SOURCEFILES)
.PHONY:     all clean

all: $(TARGET)

$(O_FILES): | $(OBJ_DIR) 

$(D_FILES): | $(OBJ_DIR)

$(OBJ_DIR) :
	@$(MKDIR) $@

$(TARGET): $(O_FILES)
	@echo "linking ..."
	@$(AR) rcs $@ $^

clean:
	$(RM) $(O_FILES) $(D_FILES) $(TARGET)

-include $(D_FILES)
