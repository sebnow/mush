APPNAME = mush
VERSION = 1.0
TARBALL := $(APPNAME)-$(VERSION).tar.bz2

CFLAGS = -Isrc -Os
PREFIX = /usr/local
OBJ = build/cd.o \
      build/exit.o \
      build/prompt.o \
      build/pwd.o \
      build/command.o \
      build/exec.o \
      build/parser.o \
      build/queue.o \
      build/mush_error.o

all: build/ $(APPNAME)

$(APPNAME): $(OBJ) build/main.o
	@echo "LINK $@"
	@$(LINK.cc) -o $@ $(OBJ) build/main.o

clean:
	$(RM) -r build/

install:
	-mkdir -p $(PREFIX)/bin/
	install -m755 $(APPNAME) $(PREFIX)/bin/$(APPNAME)

uninstall:
	$(RM) $(PREFIX)/bin/mush

dist:
	tar -cjf $(TARBALL) LICENSE Makefile README.markdown Rules.mk src/* wscript

distclean: clean
	$(RM) $(APPNAME)
	$(RM) $(TARBALL)

-include Rules.mk
-include Tests.mk