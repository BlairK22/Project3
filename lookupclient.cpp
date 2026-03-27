/* lookupclient.cpp
 * Bible Lookup Client - CGI program that receives AJAX requests
 * from the web interface, sends them to the lookup server via
 * FIFO pipes, and returns the results to the web page.
 * CSC-3004 Introduction to Software Development - Project 3, Part 2
 *
 * Based on sslookupclient.cpp from Shakespeare demo
 * and bibleajax.cpp from Project 2
 *
 * STUDENT NAME: Blair Karamaga
 */

#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <string.h>

/* Required libraries for AJAX/CGI */
#include "/home/class/csc3004/cgicc/Cgicc.h"
#include "/home/class/csc3004/cgicc/HTTPHTMLHeader.h"
#include "/home/class/csc3004/cgicc/HTMLClasses.h"

/* Pipe communication */
#include "fifo.h"
#include "Ref.h"

using namespace std;
using namespace cgicc;

#define logging
#define LOG_FILENAME "/home/class/csc3004/tmp/blakaramaga-lookupclient.log"
#include "logfile.h"

// Pipe names - must match the server's pipe names
string receive_pipe = "reply";
string send_pipe = "request";

int main() {
   // Send required CGI header before any other output
   cout << "Content-Type: text/plain\n\n";

   Cgicc cgi;  // create object used to access CGI request data

   #ifdef logging
   logFile.open(LOG_FILENAME, ios::out);
   #endif

   // GET THE INPUT DATA from the web form
   form_iterator st = cgi.getElement("search_type");
   form_iterator book = cgi.getElement("book");
   form_iterator chapter = cgi.getElement("chapter");
   form_iterator verse = cgi.getElement("verse");
   form_iterator nv = cgi.getElement("num_verse");

   // Input validation
   bool validInput = false;
   if (chapter != cgi.getElements().end()) {
      int chapterNum = chapter->getIntegerValue();
      if (chapterNum > 150) {
         cout << "<p>The chapter number (" << chapterNum << ") is too high.</p>" << endl;
      } else if (chapterNum <= 0) {
         cout << "<p>The chapter must be a positive number.</p>" << endl;
      } else {
         validInput = true;
      }
   }

   if (validInput && verse != cgi.getElements().end()) {
      int verseNum = verse->getIntegerValue();
      if (verseNum <= 0) {
         cout << "<p>The verse must be a positive number.</p>" << endl;
         validInput = false;
      }
   } else if (validInput) {
      cout << "<p>Please enter a verse number.</p>" << endl;
      validInput = false;
   }

   if (validInput && book == cgi.getElements().end()) {
      cout << "<p>Please select a book.</p>" << endl;
      validInput = false;
   }

   if (validInput) {
      int bookNum = book->getIntegerValue();
      int chapterNum = chapter->getIntegerValue();
      int verseNum = verse->getIntegerValue();
      int numVerses = 1;
      if (nv != cgi.getElements().end()) {
         numVerses = nv->getIntegerValue();
         if (numVerses < 1) numVerses = 1;
      }

      if (bookNum < 1 || bookNum > 66) {
         cout << "<p><b>Error:</b> Invalid book number.</p>" << endl;
      } else {
         // Create the FIFO pipe objects (suppress stdout messages)
         streambuf* origBuf = cout.rdbuf();
         cout.rdbuf(NULL);
         Fifo recfifo(receive_pipe);
         Fifo sendfifo(send_pipe);
         cout.rdbuf(origBuf);

         // Send each verse request to the server one at a time
         // Start with the requested reference
         int curBook = bookNum;
         int curChap = chapterNum;
         int curVerse = verseNum;

         int currentChapter = -1;
         int currentBook = -1;

         for (int i = 0; i < numVerses; i++) {
            // Build the request string: "book:chapter:verse"
            stringstream ss;
            ss << curBook << ":" << curChap << ":" << curVerse;
            string request = ss.str();

            log("Sending request: " + request);

            // Send request to server
            sendfifo.openwrite();
            sendfifo.send(request);

            // Read reply from server
            recfifo.openread();
            string reply = "";
            string results = "";
            while (results != "$end") {
               results = recfifo.recv();
               log("Received: " + results);
               if (results != "$end") {
                  reply = results;
               }
            }
            recfifo.fifoclose();
            sendfifo.fifoclose();

            // Parse the reply: "status:BookName C:V verse_text"
            if (reply.length() > 0) {
               // Get the status code (first character before the colon)
               string::size_type colonPos = reply.find(':');
               if (colonPos != string::npos) {
                  int status = atoi(reply.substr(0, colonPos).c_str());

                  if (status == 0) {
                     // SUCCESS - parse and display the verse
                     string verseData = reply.substr(colonPos + 1);

                     // Parse book name, chapter:verse, and text from verseData
                     // Format: "BookName C:V text"
                     // Find the chapter:verse pattern to split
                     string::size_type spacePos = 0;
                     string::size_type cvPos = verseData.find(':');
                     if (cvPos != string::npos) {
                        // Find the space before the chapter number
                        spacePos = verseData.rfind(' ', cvPos);
                        string bkName = verseData.substr(0, spacePos);
                        string rest = verseData.substr(spacePos + 1);

                        // Parse chapter and verse from rest
                        string::size_type colonPos2 = rest.find(':');
                        int rChap = atoi(rest.substr(0, colonPos2).c_str());
                        string afterColon = rest.substr(colonPos2 + 1);
                        string::size_type spacePos2 = afterColon.find(' ');
                        int rVerse = atoi(afterColon.substr(0, spacePos2).c_str());
                        string text = afterColon.substr(spacePos2 + 1);

                        int rBook = curBook;

                        // Stop if we moved to a new book
                        if (i > 0 && rBook != currentBook) {
                           break;
                        }

                        // Print chapter heading if chapter changed
                        if (rChap != currentChapter) {
                           currentChapter = rChap;
                           currentBook = rBook;
                           cout << "<b>" << bkName << " " << rChap << "</b><br>" << endl;
                        }

                        // Display verse number and text
                        cout << rVerse << " " << text << "<br>" << endl;

                        // For next iteration, we need the next verse reference
                        // Increment verse for next request
                        curVerse = rVerse + 1;
                        curChap = rChap;
                     }
                  } else {
                     // Error from server
                     string errorMsg = reply.substr(colonPos + 1);
                     cout << "<p><b>Error:</b> " << errorMsg << "</p>" << endl;
                     break;
                  }
               }
            }
         }
      }
   } else if (chapter == cgi.getElements().end()) {
      cout << "<p>Please enter a chapter number.</p>" << endl;
   }

   return 0;
}
