USE_DEBUG = NO
USE_STATIC = NO
USE_WINMSGS = NO

#BASE_PATH=c:/mingw.tdm461/bin/
#BASE_PATH=c:/mingw.v4.8.1/bin/
BASE_PATH=c:/mingw/bin/

ifeq ($(USE_DEBUG),YES)
CFLAGS=-Wall -O -ggdb -mwindows 
LFLAGS=
else
CFLAGS=-Wall -O2 -mwindows 
LFLAGS=-s
endif
CFLAGS += -Wno-write-strings
CFLAGS += -Weffc++
CFLAGS += -D_WIN32_IE=0x0501
CFLAGS += -DWINVER=0x0500
LiFLAGS += -DWINVER=0x0500
LiFLAGS += -D_WIN32_IE=0x0501
#LiFLAGS += _WIN32_WINNT=0x0501
ifeq ($(USE_STATIC),YES)
LIBS=-lgdi32 -lcomctl32 -static
else
LIBS=-lgdi32 -lcomctl32
endif

CSRC+=led.scroll.cpp matrixstatic.cpp lrender.cpp fontmgr.cpp \
wshowfont.cpp tooltips.cpp \
common_funcs.cpp statbar.cpp winmsgs.cpp
ifeq ($(USE_WINMSGS),YES)
CSRC+=winmsgs.cpp 
CFLAGS += -DUSE_WINMSGS
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
	zip $(BIN).zip $(BIN).exe *.f* readme.txt

source:
	rm -f $(BIN).src.zip
	zip $(BIN).src.zip *

lint:
	cmd /C "c:\lint9\lint-nt +v -width(160,4) $(LiFLAGS) -ic:\lint9 -i../der_libs mingw.lnt -os(_lint.tmp) lintdefs.cpp $(CSRC)"

depend:
	makedepend $(CFLAGS) $(CSRC)

#************************************************************
$(BIN).exe: $(OBJS)
	$(BASE_PATH)g++ $(CFLAGS) $(LFLAGS) $(OBJS) -o $@ $(LIBS)

rc.o: $(BIN).rc 
	$(BASE_PATH)windres $< -O coff -o $@

# DO NOT DELETE

led.scroll.o: resource.h common.h statbar.h fontmgr.h lrender.h
led.scroll.o: matrixstatic.h
matrixstatic.o: common.h fontmgr.h lrender.h MatrixStatic.h
lrender.o: common.h fontmgr.h lrender.h
fontmgr.o: common.h fontmgr.h
wshowfont.o: resource.h common.h statbar.h fontmgr.h lrender.h
tooltips.o: resource.h common.h
common_funcs.o: common.h
statbar.o: common.h statbar.h
