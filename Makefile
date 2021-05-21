DESTDIR?=/usr
PREFIX?=/local

ifneq ($V,1)
Q ?= @
endif

CC	= gcc
CFLAGS	= $(DEBUG) -Wall -Wextra $(INCLUDE) -Winline -pipe 

LDFLAGS	= -L$(DESTDIR)$(PREFIX)/lib
LIBS    = -lrt -lm -lcrypt

SRC	=	src/lkit.c src/comm.c src/gpio.c

OBJ	=	$(SRC:.c=.o)

all:		lkit

lkit:	$(OBJ)
	$Q echo [Link]
	$Q $(CC) -o $@ $(OBJ) $(LDFLAGS) $(LIBS)

.c.o:
	$Q echo [Compile] $<
	$Q $(CC) -c $(CFLAGS) $< -o $@

.PHONY:	clean
clean:
	$Q echo "[Clean]"
	$Q rm -f $(OBJ) lkit *~ core tags *.bak
  
.PHONY:	install
install: lkit
	$Q echo "[Install]"
	$Q cp lkit		$(DESTDIR)$(PREFIX)/bin
ifneq ($(WIRINGPI_SUID),0)
	$Q chown root.root	$(DESTDIR)$(PREFIX)/bin/lkit
	$Q chmod 4755		$(DESTDIR)$(PREFIX)/bin/lkit
endif
#	$Q mkdir -p		$(DESTDIR)$(PREFIX)/man/man1
#	$Q cp megaio.1		$(DESTDIR)$(PREFIX)/man/man1

.PHONY:	uninstall
uninstall:
	$Q echo "[UnInstall]"
	$Q rm -f $(DESTDIR)$(PREFIX)/bin/lkit
	$Q rm -f $(DESTDIR)$(PREFIX)/man/man1/lkit.1
  