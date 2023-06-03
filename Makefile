# uncomment, if you want to use
USE_LIBCMINI = 1

PREFIX      = m68k-atari-mint-

CSTD        = -std=c99
#CSTD        = 

CFLAGS_2    = -x c -Werror -fno-builtin
LDFLAGS_2   = -Wl,--relax -Wl,--no-whole-archive
STARTUP     =

OPTFLAGS   = -O2

ifdef USE_LIBCMINI
CMINI_DIR   = /usr/local/lib/libcmini
CFLAGS_2   += -nostdlib -nodefaultlibs -DUSE_LIBCMINI -I$(CMINI_DIR)/include
LDFLAGS_2  += -nostdlib -nodefaultlibs -L$(CMINI_DIR)/lib -lcmini -lgcc
STARTUP     = $(CMINI_DIR)/lib/crt0.o
endif

CC	    = $(PREFIX)gcc.exe
AR	    = $(PREFIX)ar.exe
RANLIB	    = $(PREFIX)ranlib.exe
MAKE	    = make --makefile=MAKEFILE
CP          = cp -a
MV          = mv
SRCDIR      = $(UPDIR)src
PRGDIR	    = $(UPDIR)
DEPEND      = _depend.dep
CFLAGS      = -Wall -pedantic -fomit-frame-pointer $(CFLAGS_2) $(CSTD)
STRIPFLAGS  = -s -R .comment -R .gnu.version
LDFLAGS	    = -s $(LDFLAGS_2)
TOUPPER     = | tr "[:lower:]" "[:upper:]"
TOLOWER     = | tr "[:upper:]" "[:lower:]"

.PHONY:	all clean depend dep install

LDLIBS	 = -lgem

PRG	 = testbild.prg
SRC	 = $(wildcard *.c)
OBJ	 = $(SRC:%.c=%.o)

PRGU	 = $(shell echo "$(PRG)" $(TOUPPER))

all:	$(OBJ) $(PRG)

clean:
	rm -f $(OBJ) $(PRG)

depend dep:
	$(CC) $(CFLAGS) -MM $(SRC) >$(DEPEND)

$(PRG):	$(OBJ)
	$(CC) $(STARTUP) -o $(PRG) $(OBJ) $(OOBJ) $(LDLIBS) $(LDFLAGS)

%.o:	%.c
	$(CC) $(CFLAGS) $(OPTFLAGS) -c $< -o $@

-include $(DEPEND)
