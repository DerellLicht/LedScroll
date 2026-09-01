USE_DEBUG = NO
USE_64BIT = NO
USE_UNICODE = YES
USE_CLANG = NO
# sadly, cygwin mingw does not support gdiplus...
USE_CYGWIN = NO
USE_WINMSGS = NO

include der_libs\tool_select.mak

ifeq ($(USE_DEBUG),YES)
CFLAGS=-Wall -O -ggdb
LFLAGS=-mwindows 
else
CFLAGS=-Wall -O2 
LFLAGS=-s -mwindows 
endif
CFLAGS += -Wno-write-strings
CFLAGS += -Wno-stringop-truncation
CFLAGS += -Weffc++

ifeq ($(USE_WINMSGS),YES)
CFLAGS += -DUSE_WINMSGS
endif

CFLAGS += -Ider_libs
IFLAGS += -Ider_libs

CSRC+=led.scroll.cpp matrixstatic.cpp lrender.cpp fontmgr.cpp wshowfont.cpp \
der_libs/tooltips.cpp \
der_libs/common_funcs.cpp \
der_libs/common_win.cpp \
der_libs/statbar.cpp \
der_libs/winmsgs.cpp 

LIBS=-lgdi32 -lcomctl32

ifeq ($(USE_STATIC),YES)
LIBS += -static
endif

OBJS = $(CSRC:.cpp=.o) rc.o

BASE=led.scroll
BINX := $(BASE).exe

# Automatically parse the latest version block
VERSION := $(shell grep -oE '\[[0-9]+\.[0-9]+\]' CHANGELOG.md | head -n 1 | tr -d '[]')
DIST_ZIP := $(BASE)V$(VERSION).zip

# Force these action-only targets to always run
.PHONY: dist release update

#************************************************************
%.o: %.cpp
	$(TOOLS)\$(GNAME) $(CFLAGS) $< -o $@

all: $(BINX)

clean:
	rm -f *.exe $(OBJS) *.zip

dist:
	rm -f *.zip
	zip -r $(DIST_ZIP) $(BINX) *.f* fntcol\* readme.txt

# Your new automated release workflow
release: dist
	@cmd /C "@echo Preparing GitHub release for v$(VERSION)..."
	sed -n '/## \['$(VERSION)'\]/,/## \[/p' CHANGELOG.md | sed '$$d' > temp_notes.md
	gh release create v$(VERSION) ./$(DIST_ZIP) ./CHANGELOG.md --notes-file temp_notes.md
	rm temp_notes.md
	@cmd /C "@echo Release v$(VERSION) successfully uploaded to GitHub!"
	
# Your new update-in-place pipeline
update: dist
	@cmd /C "@echo Updating assets for existing release v$(VERSION)..."
	@# Uploads and overwrites the .zip file and CHANGELOG.md on GitHub
	gh release upload v$(VERSION) ./$(DIST_ZIP) ./CHANGELOG.md --clobber
	@cmd /C "@echo Release v$(VERSION) assets successfully updated on GitHub!"

clint:
	cmd /C "python ..\ClaudeLint.py --exclude der_libs"
	
check:
	cmd /C "d:\llvm\bin\clang-tidy.exe $(CSRC)"

cppc:
	cmd /C "cppcheck --project=compile_commands.json --std=c++14 --suppressions-list=./.suppress.cppcheck"

depend:
	makedepend $(IFLAGS) $(CSRC)

#************************************************************
$(BINX): $(OBJS)
	$(TOOLS)/$(GNAME) $(OBJS) $(LFLAGS) -o $(BINX) $(LIBS) 

rc.o: $(BIN).rc 
	$(TOOLS)\$(WRNAME) $< -O COFF -o $@

# DO NOT DELETE

led.scroll.o: resource.h der_libs/common.h der_libs/statbar.h fontmgr.h
led.scroll.o: lrender.h matrixstatic.h
matrixstatic.o: der_libs/common.h fontmgr.h lrender.h matrixstatic.h
lrender.o: der_libs/common.h fontmgr.h lrender.h
fontmgr.o: der_libs/common.h fontmgr.h
wshowfont.o: resource.h der_libs/common.h der_libs/commonw.h
wshowfont.o: der_libs/tooltips.h der_libs/statbar.h fontmgr.h lrender.h
der_libs/tooltips.o: der_libs/iface_32_64.h der_libs/common.h
der_libs/tooltips.o: der_libs/tooltips.h
der_libs/common_funcs.o: der_libs/common.h
der_libs/common_win.o: der_libs/common.h der_libs/commonw.h
der_libs/statbar.o: der_libs/common.h der_libs/commonw.h der_libs/statbar.h
