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
	$(RM) $(WASM_OBJS) $(WASM_TARGET) $(WASM_LIB_TARGET)

##############################################################################
# WASM target
# Requires:
#   - clang with wasm32-wasi target support
#   - wasi-sdk
# Override TEA_WASM to point to the tea.wasm WASM module file
# Override LIBTEA_WASM to point to the relocatable libtea-wasm.a WASM module file
##############################################################################

WASM_CC ?= clang --target=wasm32-wasip1 --sysroot=$(WASI_SYSROOT)
WASM_LD ?= wasm-ld
WASM_TARGET = micro.wasm
WASM_LIB_TARGET = libmicro-wasm.a
WASM_CFLAGS = -O2 -Ilib -Isrc -Isrc/embed -Istub -DMICRO_NO_AUDIO -D_WASI_EMULATED_SIGNAL -fno-stack-protector
WASM_LDFLAGS = --export-all --allow-undefined --stack-first -z stack-size=1048576
WASM_LIB_LD_FLAGS = -r --allow-undefined
WASM_SRC = $(wildcard src/*.c) $(wildcard lib/*.c)
WASM_OBJS = $(WASM_SRC:.c=.wasm.o)

wasm: embed $(WASM_TARGET)
wasm-lib: embed $(WASM_LIB_TARGET)

$(WASM_TARGET): $(WASM_OBJS)
	$(E) "WASM LD         $@"
	$(Q)$(WASM_LD) $(WASM_LDFLAGS) -o $@ $^ $(LIBTEA_WASM)

$(WASM_LIB_TARGET): $(WASM_OBJS)
	$(E) "WASM LIB LD     $@"
	$(Q)$(WASM_LD) $(WASM_LIB_LD_FLAGS) -o $@ $^ $(LIBTEA_WASM)

src/%.wasm.o: src/%.c
	$(E) "WASM CC      $<"
	$(Q)$(WASM_CC) $(WASM_CFLAGS) -c -o $@ $<

lib/%.wasm.o: lib/%.c
	$(E) "WASM CC      $<"
	$(Q)$(WASM_CC) $(WASM_CFLAGS) -c -o $@ $<

src/micro.wasm.o: $(EMBED_C_HEADERS)

.PHONY: all embed wasm wasm-lib clean
