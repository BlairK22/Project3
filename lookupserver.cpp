/* lookupserver.cpp
 * Bible Lookup Server - receives verse requests via FIFO pipes
 * and returns verse text using the indexed Bible class.
 * CSC-3004 Introduction to Software Development - Project 3, Part 2
 *
 * Based on sslookupserver.cpp from Shakespeare demo
 *
 * STUDENT NAME: Blair Karamaga
 */

#include <iostream>
#include <string>
#include <sstream>
#include "Bible.h"
#include "fifo.h"

#define logging
#define LOG_FILENAME "/home/class/csc3004/tmp/blakaramaga-lookupserver.log"
#include "logfile.h"

using namespace std;

// Pipe names - these will be prefixed with SIG from fifo.h
string receive_pipe = "request";
string send_pipe = "reply";

int main() {
   #ifdef logging
   logFile.open(LOG_FILENAME, ios::out);
   #endif

   // Create the Bible object - builds the reference index once
   cout << "Building Bible index..." << endl;
   Bible bible;
   cout << "Index built with " << bible.getIndexSize() << " references." << endl;
   log("Index built with " + to_string(bible.getIndexSize()) + " references.");

   // Create the communication fifos
   Fifo recfifo(receive_pipe);
   Fifo sendfifo(send_pipe);

   cout << "Lookup server ready. Waiting for requests..." << endl;
   log("Server ready.");

   recfifo.openread();

   // Server loop: repeat each time a request is received
   while (true) {
      // Get a request from the client
      // Request format: "book:chapter:verse" or "book:chapter:verse:numVerses"
      string request = recfifo.recv();
      cout << "Received request: " << request << endl;
      log("Request: " + request);

      // Parse numVerses if present (4th field after third colon)
      int numVerses = 1;
      string refPart = request;
      // Count colons to check for numVerses field
      int colonCount = 0;
      for (size_t i = 0; i < request.length(); i++) {
         if (request[i] == ':') colonCount++;
      }
      if (colonCount >= 3) {
         // Find the third colon to split off numVerses
         size_t pos = 0;
         for (int c = 0; c < 3; c++) {
            pos = request.find(':', pos) + 1;
         }
         numVerses = atoi(request.substr(pos).c_str());
         refPart = request.substr(0, pos - 1);
         if (numVerses < 1) numVerses = 1;
      }

      // Parse the reference from the request string
      Ref ref(refPart);
      cout << "Parsed reference: ";
      ref.display();
      cout << " (" << numVerses << " verses)" << endl;

      sendfifo.openwrite();

      // Look up and send each verse
      for (int i = 0; i < numVerses; i++) {
         LookupResult result;
         Verse verse = bible.lookup(ref, result);

         string message;
         if (result == SUCCESS) {
            stringstream ss;
            ss << ref.getBookName() << " " << ref.getChapter() << ":" << ref.getVerse() << " " << verse.getVerse();
            message = to_string(result) + ":" + ss.str();
            cout << "Sending: " << message << endl;
            log("Reply: " + message);
            sendfifo.send(message);

            // Get next verse reference for multi-verse lookup
            if (i < numVerses - 1) {
               ref = bible.next(ref, result);
               if (result != SUCCESS) {
                  message = to_string(result) + ":" + bible.error(result);
                  sendfifo.send(message);
                  break;
               }
            }
         } else {
            message = to_string(result) + ":" + bible.error(result);
            cout << "Sending: " << message << endl;
            log("Reply: " + message);
            sendfifo.send(message);
            break;
         }
      }

      // Send end-of-response marker
      sendfifo.send("$end");
      sendfifo.fifoclose();
      cout << "Response complete." << endl << endl;
   }

   return 0;
}
