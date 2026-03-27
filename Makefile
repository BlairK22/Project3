# Bibleajax program
# Bob Kasper, MVNU Computer Science
# updated January 2020 to use
# c++11 compiler option, current paths on cs.mvnu.edu

USER= blakaramaga

# Use GNU C++ compiler with C++11 standard
CC= g++
CFLAGS= -g -std=c++11

# Part 2: build all components
all:	testreader lookupserver bibleindex.cgi PutCGI PutHTML

# ---- Part 1: Console test program ----

testreader:	testreader.o Bible.o Ref.o Verse.o
	$(CC) $(CFLAGS) -o testreader testreader.o Bible.o Ref.o Verse.o

testreader.o:	testreader.cpp Bible.h Ref.h Verse.h
	$(CC) $(CFLAGS) -c testreader.cpp

# ---- Part 2: Lookup Server ----

lookupserver:	lookupserver.o Bible.o Ref.o Verse.o fifo.o
	$(CC) $(CFLAGS) -o lookupserver lookupserver.o Bible.o Ref.o Verse.o fifo.o

lookupserver.o:	lookupserver.cpp Bible.h Ref.h Verse.h fifo.h
	$(CC) $(CFLAGS) -c lookupserver.cpp

# ---- Part 2: Lookup Client (CGI) ----

bibleindex.cgi:	lookupclient.o Ref.o fifo.o
	$(CC) $(CFLAGS) -o bibleindex.cgi lookupclient.o Ref.o fifo.o -lcgicc

lookupclient.o:	lookupclient.cpp Ref.h fifo.h
	$(CC) $(CFLAGS) -c lookupclient.cpp

fifo.o:	fifo.cpp fifo.h
	$(CC) $(CFLAGS) -c fifo.cpp

# ---- Class file targets ----

Ref.o:	Ref.cpp Ref.h
	$(CC) $(CFLAGS) -c Ref.cpp

Verse.o:	Verse.cpp Verse.h Ref.h
	$(CC) $(CFLAGS) -c Verse.cpp

Bible.o:	Bible.cpp Bible.h Ref.h Verse.h
	$(CC) $(CFLAGS) -c Bible.cpp

# ---- Deploy to web server ----

PutCGI:	bibleindex.cgi
	chmod 755 bibleindex.cgi
	cp bibleindex.cgi /var/www/html/class/csc3004/$(USER)/cgi-bin
	echo "Current contents of your cgi-bin directory: "
	ls -l /var/www/html/class/csc3004/$(USER)/cgi-bin/

PutHTML:
	cp bibleindex.html /var/www/html/class/csc3004/$(USER)
	echo "Current contents of your HTML directory: "
	ls -l /var/www/html/class/csc3004/$(USER)

clean:
	rm -f *.o testreader lookupserver bibleindex.cgi bibleajax.cgi
