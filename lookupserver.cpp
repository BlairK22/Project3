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

using namespace std;

// Pipe names - these will be prefixed with SIG from fifo.h
string receive_pipe = "request";
string send_pipe = "reply";

int main() {
   // Create the Bible object - builds the reference index once
   cout << "Building Bible index..." << endl;
   Bible bible;
   cout << "Index built with " << bible.getIndexSize() << " references." << endl;

   // Create the communication fifos
   Fifo recfifo(receive_pipe);
   Fifo sendfifo(send_pipe);

   cout << "Lookup server ready. Waiting for requests..." << endl;

   recfifo.openread();

   // Server loop: repeat each time a request is received
   while (true) {
      // Get a request from the client
      string request = recfifo.recv();
      cout << "Received request: " << request << endl;

      // Parse the request string into a Ref using the Ref parse constructor
      // Request format: "book:chapter:verse"
      Ref ref(request);
      cout << "Parsed reference: ";
      ref.display();
      cout << endl;

      // Look up the verse using the index
      LookupResult result;
      Verse verse = bible.lookup(ref, result);

      // Build and send the reply
      sendfifo.openwrite();

      string message;
      if (result == SUCCESS) {
         // Format: "status:BookName C:V verse_text"
         stringstream ss;
         ss << ref.getBookName() << " " << ref.getChapter() << ":" << ref.getVerse() << " " << verse.getVerse();
         message = to_string(result) + ":" + ss.str();
      } else {
         message = to_string(result) + ":" + bible.error(result);
      }

      cout << "Sending reply: " << message << endl;
      sendfifo.send(message);

      // Send end-of-response marker
      sendfifo.send("$end");
      sendfifo.fifoclose();
      cout << "Response complete." << endl << endl;
   }

   return 0;
}
