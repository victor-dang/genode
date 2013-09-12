CONTRIB_DIR = $(REP_DIR)/contrib/ioq3
INC_DIR    += $(CONTRIB_DIR)/code/jpeg-8c \
              $(CONTRIB_DIR)/code/zlib
CC_OPT      = -Wimplicit -pipe -DUSE_ICON -DNO_GZIP \
              -DUSE_INTERNAL_JPEG  -DUSE_LOCAL_HEADERS -DSTANDALONE \
              -DPRODUCT_VERSION=\"1.36-GIT\" -Wformat=2 -Wno-format-zero-length -Wformat-security \
              -Wno-format-nonliteral -Wstrict-aliasing=2 -Wmissing-format-attribute -Wdisabled-optimization \
              -Werror-implicit-function-declaration -DNDEBUG -funroll-loops -falign-loops=2 \
              -falign-jumps=2 -falign-functions=2 -fstrength-reduce -ffast-math
CC_MARCH    = -march=core2 -m32
LIBS        = sdl libc libm

vpath %.c $(CONTRIB_DIR)
