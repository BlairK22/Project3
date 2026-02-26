/* testreader.cpp
 * Test program for Bible reference index
 * CSC-3004 Introduction to Software Development - Project 3, Part 1
 *
 * Usage: ./testreader <book> <chapter> <verse> [numVerses]
 *
 * STUDENT NAME: Blair Karamaga
 */

#include "Bible.h"
#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[]) {
   // Check command line arguments
   if (argc < 4 || argc > 5) {
      cerr << "Usage: " << argv[0] << " <book> <chapter> <verse> [numVerses]" << endl;
      return 1;
   }

   int book = atoi(argv[1]);
   int chap = atoi(argv[2]);
   int verse = atoi(argv[3]);
   int numVerses = 1;
   if (argc == 5) {
      numVerses = atoi(argv[4]);
   }

   // Create Bible object - this opens the file and builds the reference index
   Bible bible;

   // --- Diagnostic output to verify index construction ---
   cout << "Bible reference index built:" << endl;
   cout << "  Number of references in index: " << bible.getIndexSize() << endl;
   cout << "  Last reference added: ";
   bible.getLastRefAdded().display();
   cout << endl;
   cout << "  Byte offset of last reference: " << bible.getLastOffsetAdded() << endl;
   cout << endl;

   // Print byte offsets for a few known references to verify correctness
   cout << "Verification - byte offsets for known references:" << endl;
   bible.printRefInfo(Ref(1, 1, 1));    // Genesis 1:1
   bible.printRefInfo(Ref(1, 1, 2));    // Genesis 1:2
   bible.printRefInfo(Ref(43, 3, 16));  // John 3:16
   cout << endl;

   // --- Lookup the requested reference ---
   Ref ref(book, chap, verse);
   LookupResult result;

   // Look up and display the requested verse(s)
   for (int i = 0; i < numVerses; i++) {
      Verse v = bible.lookup(ref, result);
      if (result == SUCCESS) {
         v.display();
         cout << endl;
         // Get the next reference for multi-verse lookup
         if (i < numVerses - 1) {
            ref = bible.next(ref, result);
            if (result != SUCCESS) {
               break;
            }
         }
      } else {
         cout << bible.error(result) << endl;
         break;
      }
   }

   return 0;
}
