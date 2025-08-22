CC = gcc
CDEBUG = -O0 -g
CCWARN = -Wall -Wextra
CFLAGS = $(CCWARN) $(CDEBUG) $(INCLUDE)
RM = del

INCLUDE = -Ilib -Isrc -Isrc/embed
LIBS = 

LDFLAGS = $(LIBS) -lopengl32 -lglfw3 -lglew32 -ltea00

SRC = $(wildcard src/*.c)
SRC_HEADERS = $(wildcard src/*.h)
SRC_OBJS = $(SRC:.c=.o)

EMBED_TEA_FILES = $(wildcard src/embed/*.tea)
EMBED_FONT_FILES = $(wildcard src/embed/*.ttf)
EMBED_C_HEADERS = $(patsubst src/embed/%.tea, src/embed/%_tea.h, $(EMBED_TEA_FILES))
EMBED_C_HEADERS += $(patsubst src/embed/%.ttf, src/embed/%_ttf.h, $(EMBED_FONT_FILES))

LIB_SRC = $(wildcard lib/*.c)
LIB_OBJS = $(LIB_SRC:.c=.o)

OBJS = $(SRC_OBJS) $(LIB_OBJS)

TARGET = micro

Q = @
E = @echo

all:	$(TARGET)

embed: $(EMBED_C_HEADERS)

$(TARGET): $(OBJS)
	$(E) "LINK         $@"
	$(Q)$(CC) -o $@ $^ $(LDFLAGS)

src/micro.o: src/micro.c $(EMBED_C_HEADERS)

src/m_init.o: src/m_init.c $(EMBED_C_HEADERS)

src/%.o: src/%.c
	$(E) "CC           $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

lib/%.o: lib/%.c
	$(E) "CC           $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

src/embed/%_tea.h: src/embed/%.tea
	$(E) "EMBED        $@"
	$(Q)python cembed.py $< > $@

src/embed/%_ttf.h: src/embed/%.ttf
	$(E) "EMBED        $@"
	$(Q)python cembed.py $< > $@

clean:
	$(RM) $(OBJS) $(TARGET)

.PHONY: all embed clean