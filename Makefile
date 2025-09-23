#SHELL=cmd.exe
USE_DEBUG = NO
USE_STATIC = NO
USE_WINMSGS = NO

<<<<<<< HEAD
=======
#BASE_PATH=c:/mingw.tdm461/bin/
#BASE_PATH=c:/mingw.v4.8.1/bin/
>>>>>>> 0b0b9773be1f4a7a25efe03d2e3635d0b67a9c32
BASE_PATH=d:/tdm32/bin/

ifeq ($(USE_DEBUG),YES)
CFLAGS=-Wall -O -ggdb -mwindows 
LFLAGS=
else
CFLAGS=-Wall -O2 -mwindows 
LFLAGS=-s
endif
CFLAGS += -Wno-write-strings
CFLAGS += -Wno-stringop-truncation
CFLAGS += -Weffc++
<<<<<<< HEAD
#LiFLAGS += -DWINVER=0x0500
#LiFLAGS += -D_WIN32_IE=0x0501
=======
CFLAGS += -Wno-stringop-truncation
LiFLAGS += -DWINVER=0x0500
LiFLAGS += -D_WIN32_IE=0x0501
>>>>>>> 0b0b9773be1f4a7a25efe03d2e3635d0b67a9c32
#LiFLAGS += _WIN32_WINNT=0x0501
CFLAGS += -DUSE_WINMSGS

LiFLAGS += -Ider_libs
CFLAGS += -Ider_libs
IFLAGS += -Ider_libs

CSRC+=led.scroll.cpp matrixstatic.cpp lrender.cpp fontmgr.cpp wshowfont.cpp \
der_libs\tooltips.cpp \
der_libs\common_funcs.cpp \
der_libs\common_win.cpp \
der_libs\statbar.cpp \
der_libs\winmsgs.cpp 

ifeq ($(USE_STATIC),YES)
LIBS=-lgdi32 -lcomctl32 -static
else
LIBS=-lgdi32 -lcomctl32
endif

OBJS = $(CSRC:.cpp=.o) rc.o

BIN=led.scroll

#************************************************************
%.o: %.cpp
	$(BASE_PATH)g++ $(CFLAGS) -c $< -o $@

all: $(BIN).exe

clean:
	rm -f $(BIN).exe $(OBJS) *.zip *.bak *~

dist:
	rm -f $(BIN).zip
	zip -r $(BIN).zip $(BIN).exe *.f* fntcol\* readme.txt

source:
	rm -f $(BIN).src.zip
	zip $(BIN).src.zip *

lint:
	cmd /C "c:\lint9\lint-nt +v -width(160,4) $(LiFLAGS) -ic:\lint9 mingw.lnt -os(_lint.tmp) lintdefs.cpp $(CSRC)"

depend:
	makedepend $(IFLAGS) $(CSRC)

#************************************************************
$(BIN).exe: $(OBJS)
	$(BASE_PATH)g++ $(CFLAGS) $(LFLAGS) $(OBJS) -o $@ $(LIBS)

rc.o: $(BIN).rc 
	windres $< -O COFF -o $@

# DO NOT DELETE

led.scroll.o: resource.h der_libs/common.h der_libs/statbar.h fontmgr.h
led.scroll.o: lrender.h matrixstatic.h
matrixstatic.o: der_libs/common.h fontmgr.h lrender.h MatrixStatic.h
lrender.o: der_libs/common.h fontmgr.h lrender.h
fontmgr.o: der_libs/common.h fontmgr.h
wshowfont.o: targetver.h resource.h der_libs/common.h der_libs/tooltips.h
wshowfont.o: der_libs/statbar.h fontmgr.h lrender.h
der_libs\tooltips.o: der_libs/iface_32_64.h der_libs/common.h
der_libs\tooltips.o: der_libs/tooltips.h
der_libs\common_funcs.o: der_libs/common.h
der_libs\common_win.o: der_libs/common.h der_libs/commonw.h
der_libs\statbar.o: der_libs/common.h der_libs/commonw.h der_libs/statbar.h
