/* lookupclient.cpp
 * Bible Lookup Client - CGI program that receives AJAX requests
 * from the web interface, sends them to the lookup server via
 * FIFO pipes, and returns the results to the web page.
 * CSC-3004 Introduction to Software Development - Project 3, Part 2
 *
 * This program is invoked by the Apache web server each time the
 * user submits a search from bibleindex.html. It extracts the
 * form data (book, chapter, verse, numVerses), validates the input,
 * builds a request string, sends it to the lookup server via a
 * FIFO pipe, reads the reply verses from the reply pipe, and
 * outputs HTML to display on the web page.
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

/* Required libraries for AJAX/CGI - handles HTTP form data */
#include "/home/class/csc3004/cgicc/Cgicc.h"
#include "/home/class/csc3004/cgicc/HTTPHTMLHeader.h"
#include "/home/class/csc3004/cgicc/HTMLClasses.h"

/* Pipe communication with the lookup server */
#include "fifo.h"
#include "Ref.h"

using namespace std;
using namespace cgicc;

// Enable logging to track client activity for debugging
#define logging
#define LOG_FILENAME "/home/class/csc3004/tmp/blakaramaga-lookupclient.log"
#include "logfile.h"

// Pipe names - must match the server's pipe names
// Combined with SIG in fifo.h: blakaramaga_reply, blakaramaga_request
string receive_pipe = "reply";
string send_pipe = "request";

/* parseVerseReply - Parse a single verse reply from the server
 * Parameters:
 *   verseData - the verse data string after the status code
 *               format: "BookName C:V verse_text"
 *   bkName    - output: the book name (e.g., "Matthew")
 *   rChap     - output: the chapter number
 *   rVerse    - output: the verse number
 *   text      - output: the verse text
 * Returns: true if parsing succeeded, false otherwise
 */
bool parseVerseReply(const string& verseData, string& bkName,
                     int& rChap, int& rVerse, string& text) {
   // Find the colon in "C:V" pattern to locate chapter:verse
   string::size_type cvPos = verseData.find(':');
   if (cvPos == string::npos) return false;

   // Book name is everything before the chapter number
   string::size_type spaceBeforeChap = verseData.rfind(' ', cvPos);
   if (spaceBeforeChap == string::npos) return false;

   bkName = verseData.substr(0, spaceBeforeChap);
   string rest = verseData.substr(spaceBeforeChap + 1);

   // Parse chapter number before the colon
   string::size_type colonPos2 = rest.find(':');
   if (colonPos2 == string::npos) return false;
   rChap = atoi(rest.substr(0, colonPos2).c_str());

   // Parse verse number and text after the colon
   string afterColon = rest.substr(colonPos2 + 1);
   string::size_type spacePos2 = afterColon.find(' ');
   if (spacePos2 == string::npos) return false;
   rVerse = atoi(afterColon.substr(0, spacePos2).c_str());
   text = afterColon.substr(spacePos2 + 1);

   return true;
}

/* main - CGI program entry point
 * Receives form data from the web page, validates input,
 * sends request to the lookup server, receives and displays results.
 */
int main() {
   // A CGI program must send a content type header first
   cout << "Content-Type: text/plain\n\n";

   Cgicc cgi;  // cgicc object to access CGI form data

   // Open the log file for debugging
   #ifdef logging
   logFile.open(LOG_FILENAME, ios::out);
   #endif
   log("Client started - processing new request");

   // Extract form fields from the AJAX request
   form_iterator st = cgi.getElement("search_type");     // search type radio button
   form_iterator book = cgi.getElement("book");           // book dropdown (1-66)
   form_iterator chapter = cgi.getElement("chapter");     // chapter text field
   form_iterator verse = cgi.getElement("verse");         // verse text field
   form_iterator nv = cgi.getElement("num_verse");        // number of verses field

   // --- Input Validation ---
   // Validate all input before sending to the server
   bool validInput = false;

   // Validate chapter number: must be present and between 1-150
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

   // Validate verse number: must be present and positive
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

   // Validate book selection: must be present
   if (validInput && book == cgi.getElements().end()) {
      cout << "<p>Please select a book.</p>" << endl;
      validInput = false;
   }

   // --- Send Request and Display Results ---
   if (validInput) {
      // Extract integer values from form data
      int bookNum = book->getIntegerValue();
      int chapterNum = chapter->getIntegerValue();
      int verseNum = verse->getIntegerValue();
      int numVerses = 1;  // default to single verse
      if (nv != cgi.getElements().end()) {
         numVerses = nv->getIntegerValue();
         if (numVerses < 1) numVerses = 1;
         if (numVerses > 100) numVerses = 100; // safety limit
      }

      // Validate book number range (1=Genesis through 66=Revelation)
      if (bookNum < 1 || bookNum > 66) {
         cout << "<p><b>Error:</b> Invalid book number.</p>" << endl;
      } else {
         // Build the request string: "book:chapter:verse:numVerses"
         stringstream ss;
         ss << bookNum << ":" << chapterNum << ":" << verseNum << ":" << numVerses;
         string request = ss.str();
         log("Sending request: " + request);

         // Create FIFO pipe objects
         // Suppress stdout from Fifo constructor (it prints "Success creating pipe")
         // which would appear on the web page as unwanted text
         streambuf* origBuf = cout.rdbuf();
         cout.rdbuf(NULL);
         Fifo recfifo(receive_pipe);
         Fifo sendfifo(send_pipe);
         cout.rdbuf(origBuf);

         // Send the request to the lookup server via the request pipe
         sendfifo.openwrite();
         sendfifo.send(request);
         log("Request sent to server");

         // Open the reply pipe and read responses
         recfifo.openread();
         log("Waiting for reply from server...");

         // Track current chapter to print chapter headings when it changes
         int currentChapter = -1;

         // Read replies until the server sends "$end"
         string results = "";
         while (results != "$end") {
            results = recfifo.recv();
            log("Received: " + results);

            if (results != "$end") {
               // Parse the status code from the beginning of the reply
               string::size_type colonPos = results.find(':');
               if (colonPos != string::npos) {
                  int status = atoi(results.substr(0, colonPos).c_str());

                  if (status == 0) {
                     // SUCCESS - extract and display the verse
                     string verseData = results.substr(colonPos + 1);
                     string bkName;
                     int rChap, rVerse;
                     string text;

                     if (parseVerseReply(verseData, bkName, rChap, rVerse, text)) {
                        // Print chapter heading when chapter changes
                        if (rChap != currentChapter) {
                           currentChapter = rChap;
                           cout << "<b>" << bkName << " " << rChap << "</b><br>" << endl;
                        }
                        // Display the verse number and text
                        cout << rVerse << " " << text << "<br>" << endl;
                     }
                  } else {
                     // Error from server - display error message
                     string errorMsg = results.substr(colonPos + 1);
                     cout << "<p><b>Error:</b> " << errorMsg << "</p>" << endl;
                  }
               }
            }
         }

         // Close both pipes after the response is complete
         recfifo.fifoclose();
         sendfifo.fifoclose();
         log("Pipes closed. Request complete.");
      }
   } else if (chapter == cgi.getElements().end()) {
      cout << "<p>Please enter a chapter number.</p>" << endl;
   }

   return 0;
}
