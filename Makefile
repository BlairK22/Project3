# Bibleajax program
# Bob Kasper, MVNU Computer Science
# updated January 2020 to use
# c++11 compiler option, current paths on cs.mvnu.edu

# This is a Makefile for the Bible web app demo program.
# Copy this directory to a location within your home directory.
# Change the USER name value below to your own user name.
# Then use "make" to build the server program,
# and deploy it to the live web server directory.
# To test the program, go to http://cs.mvnu.edu/class/csc3004/USER/
# and open the bibleajax.html link.

USER= blakaramaga

# Use GNU C++ compiler with C++11 standard
CC= g++
CFLAGS= -g -std=c++11

all:	bibleajax.cgi PutCGI PutHTML

# Build the CGI executable, linking Project 1 class files
bibleajax.cgi:	bibleajax.o Bible.o Ref.o Verse.o
	$(CC) $(CFLAGS) -o bibleajax.cgi bibleajax.o Bible.o Ref.o Verse.o -lcgicc
	# -l option is necessary to link with cgicc library

# main program to handle AJAX/CGI requests for Bible references
bibleajax.o:	bibleajax.cpp
	$(CC) $(CFLAGS) -c bibleajax.cpp

# Project 1 class targets:
Ref.o:	Ref.cpp Ref.h
	$(CC) $(CFLAGS) -c Ref.cpp

Verse.o:	Verse.cpp Verse.h Ref.h
	$(CC) $(CFLAGS) -c Verse.cpp

Bible.o:	Bible.cpp Bible.h Ref.h Verse.h
	$(CC) $(CFLAGS) -c Bible.cpp

PutCGI:	bibleajax.cgi
	chmod 755 bibleajax.cgi
	cp bibleajax.cgi /var/www/html/class/csc3004/$(USER)/cgi-bin

	echo "Current contents of your cgi-bin directory: "
	ls -l /var/www/html/class/csc3004/$(USER)/cgi-bin/

PutHTML:
	cp bibleajax.html /var/www/html/class/csc3004/$(USER)

	echo "Current contents of your HTML directory: "
	ls -l /var/www/html/class/csc3004/$(USER)

clean:
	rm *.o core bibleajax.cgi
