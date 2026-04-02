/* lookupserver.cpp
 * Bible Lookup Server - receives verse requests via FIFO pipes
 * and returns verse text using the indexed Bible class.
 * CSC-3004 Introduction to Software Development - Project 3, Part 2
 *
 * This program builds the Bible reference index once at startup,
 * then runs as a background server, repeatedly receiving requests
 * from the lookup client via a FIFO request pipe, looking up the
 * requested verse(s) using the indexed Bible class, and sending
 * the results back through a FIFO reply pipe.
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

// Pipe names - combined with SIG in fifo.h to create unique pipe files
// e.g., /home/class/csc3004/tmp/blakaramaga_request
string receive_pipe = "request";
string send_pipe = "reply";

/* parseRequest - Parse a request string into book, chapter, verse, numVerses
 * Parameters:
 *   request   - the raw request string from the pipe (e.g., "43:3:16:1")
 *   refPart   - output: the reference portion (e.g., "43:3:16")
 *   numVerses - output: number of verses to retrieve (default 1)
 * Assumptions: request is in format "book:chapter:verse" or "book:chapter:verse:numVerses"
 */
void parseRequest(const string& request, string& refPart, int& numVerses) {
   numVerses = 1;
   refPart = request;

   // Guard against empty or malformed requests
   if (request.empty()) {
      return;
   }

   // Count colons to check if numVerses field is present
   int colonCount = 0;
   for (size_t i = 0; i < request.length(); i++) {
      if (request[i] == ':') colonCount++;
   }

   // If there are 3+ colons, the 4th field is numVerses
   if (colonCount >= 3) {
      // Find the third colon position
      size_t pos = 0;
      for (int c = 0; c < 3; c++) {
         pos = request.find(':', pos);
         if (pos == string::npos) return; // malformed, use defaults
         pos++;
      }
      // Extract numVerses from after the third colon
      string nvStr = request.substr(pos);
      if (!nvStr.empty()) {
         numVerses = atoi(nvStr.c_str());
      }
      // Extract the reference portion (everything before the third colon)
      refPart = request.substr(0, pos - 1);
      if (numVerses < 1) numVerses = 1;
   }
}

/* main - Server entry point
 * Builds the Bible index once, creates FIFO pipes, then enters
 * an infinite loop waiting for client requests and sending replies.
 */
int main() {
   #ifdef logging
   logFile.open(LOG_FILENAME, ios::out);
   #endif

   // Build the Bible reference index once at startup
   cout << "Building Bible index..." << endl;
   Bible bible;
   cout << "Index built with " << bible.getIndexSize() << " references." << endl;
   log("Index built with " + to_string(bible.getIndexSize()) + " references.");

   // Create the communication FIFO pipes
   Fifo recfifo(receive_pipe);
   Fifo sendfifo(send_pipe);

   cout << "Lookup server ready. Waiting for requests..." << endl;
   log("Server ready.");

   // Open the receive pipe once - it stays open for the life of the server
   recfifo.openread();

   // Infinite server loop: wait for requests, process, send replies
   while (true) {
      // Block until a request arrives from the client
      string request = recfifo.recv();
      cout << "Received request: " << request << endl;
      log("Request: " + request);

      // Parse the request into reference string and number of verses
      int numVerses = 1;
      string refPart;
      parseRequest(request, refPart, numVerses);

      // Create a Ref from the parsed reference string
      // The Ref parse constructor handles "book:chapter:verse" format
      Ref ref(refPart);
      cout << "Parsed reference: ";
      ref.display();
      cout << " (" << numVerses << " verses)" << endl;

      // Validate that the parsed reference has valid values
      if (ref.getBook() <= 0 || ref.getChapter() <= 0 || ref.getVerse() <= 0) {
         sendfifo.openwrite();
         string message = "4:Invalid reference format";
         cout << "Sending: " << message << endl;
         log("Reply: " + message);
         sendfifo.send(message);
         sendfifo.send("$end");
         sendfifo.fifoclose();
         cout << "Response complete." << endl << endl;
         continue; // Skip to next request
      }

      // Open the send pipe to transmit the reply
      sendfifo.openwrite();

      // Look up and send each requested verse
      for (int i = 0; i < numVerses; i++) {
         LookupResult result;
         Verse verse = bible.lookup(ref, result);

         string message;
         if (result == SUCCESS) {
            // Build reply: "0:BookName C:V verse_text"
            stringstream ss;
            ss << ref.getBookName() << " " << ref.getChapter()
               << ":" << ref.getVerse() << " " << verse.getVerse();
            message = to_string(result) + ":" + ss.str();
            cout << "Sending: " << message << endl;
            log("Reply: " + message);
            sendfifo.send(message);

            // Get the next verse reference for multi-verse requests
            if (i < numVerses - 1) {
               ref = bible.next(ref, result);
               if (result != SUCCESS) {
                  // No more verses available
                  message = to_string(result) + ":" + bible.error(result);
                  sendfifo.send(message);
                  break;
               }
            }
         } else {
            // Verse not found - send error with status code
            message = to_string(result) + ":" + bible.error(result);
            cout << "Sending: " << message << endl;
            log("Reply: " + message);
            sendfifo.send(message);
            break; // Stop looking up further verses
         }
      }

      // Send end-of-response marker so the client knows we're done
      sendfifo.send("$end");
      sendfifo.fifoclose();
      cout << "Response complete." << endl << endl;
   }

   return 0;
}
