/***************************************************************************
* fifo.h - code to allow interprocess communication via a fifo,
* or "named pipe"
* copyright : (C) 2009 by Jim Skon
*
* This code permits the creation and use of FIFOs for communication
* between processes.  
* 
* Revised by Bob Kasper (2/12/2022):
*     use special class directory instead of /tmp for improved web security
* the named piped is created and used in /home/class/csc3004/tmp
***************************************************************************/
#ifndef FIFO_H
#define FIFO_H

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <string>
using namespace std;

#define MaxMess 800

// PATH changed from /tmp to special class directory for improved web security
const string PATH  = "/home/class/csc3004/tmp/";

// SIGniture assures the pipe is unique amoung users
const string SIG = "blakaramaga_";
#define MODE 0777		// allow pipe to be read and written by any user
#define MESSTERM '\n'

class Fifo {

 private: 
  int fd;   // File descriptor for pipes
  string pipename;

 public:
  // create a named pipe (FIFO)
  Fifo();
  Fifo(string);

  void openread();    // Start a new read transaction
  void openwrite();   // Start a new write transaction
  void fifoclose();   // Finish a transaction

  string recv();      // Get the next record (after openread)
  void send(string);  // Send a record (after openwrite)
};
#endif
