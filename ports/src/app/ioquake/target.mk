TARGET      = ioquake
LIBS        = libc libc_fs libc_log sdl \
              ioq3_bot ioq3_common ioq3_renderer gallium config_args
SRC_C       = con_log.c con_tty.c sys_main.c sys_unix.c
CC_OPT      = -Wimplicit -pipe -DUSE_ICON -DNO_GZIP \
              -DUSE_INTERNAL_JPEG  -DUSE_LOCAL_HEADERS -DSTANDALONE \
              -DPRODUCT_VERSION=\"1.36-GIT\" -Wformat=2 -Wno-format-zero-length \
              -Wformat-security -Wno-format-nonliteral -Wstrict-aliasing=2 \
              -Wmissing-format-attribute -Wdisabled-optimization \
              -Werror-implicit-function-declaration -DNDEBUG -funroll-loops \
              -falign-loops=2 -falign-jumps=2 -falign-functions=2 -fstrength-reduce \
              -ffast-math

vpath %.c $(REP_DIR)/contrib/ioq3/code/sys