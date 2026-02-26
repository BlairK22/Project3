/* Bible class function definitions
 * Computer Science, MVNU
 * CSC-3004 Introduction to Software Development
 *
 * NOTE: You may add code to this file, but do not
 * delete any code or delete any comments.
 *
 * STUDENT NAME: Blair Karamaga
 */

#include "Ref.h"
#include "Verse.h"
#include "Bible.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

// Default constructor
Bible::Bible()
{
   infile = "/home/class/csc3004/Bibles/web-complete";
   isOpen = false;
   indexSize = 0;
   lastOffsetAdded = 0;
   // Open the file and build the reference index
   instream.open(infile.c_str(), ios::in);
   if (instream.is_open()) {
      isOpen = true;
      buildRefIndex();
   }
}

// Constructor – pass bible filename
Bible::Bible(const string s) {
   infile = s;
   isOpen = false;
   indexSize = 0;
   lastOffsetAdded = 0;
   // Open the file and build the reference index
   instream.open(infile.c_str(), ios::in);
   if (instream.is_open()) {
      isOpen = true;
      buildRefIndex();
   }
}

// Build the reference index by scanning through the entire Bible text file.
// Each line starts with book:chapter:verse followed by the verse text.
// Record the byte offset of each line and map the reference to that offset.
void Bible::buildRefIndex() {
   string line;
   instream.clear();
   instream.seekg(0, ios::beg);

   while (true) {
      // Record the byte offset BEFORE reading the line
      int offset = instream.tellg();

      if (!getline(instream, line)) {
         break;  // End of file
      }
      if (line.empty()) {
         continue;  // Skip empty lines
      }

      // Parse the reference from the line using the Ref string constructor
      Ref ref(line);

      // Only add valid references to the index
      if (ref.getBook() > 0) {
         refIndex[ref] = offset;
         lastRefAdded = ref;
         lastOffsetAdded = offset;
         indexSize++;
      }
   }
}

// REQUIRED: lookup finds a given verse in this Bible
Verse Bible::lookup(Ref ref, LookupResult& status)
{
   // TODO: scan the file to retrieve the line that holds ref ...

   // Open file if not already open
   if (!isOpen) {
      instream.open(infile.c_str(), ios::in);
      if (!instream) {
         cerr << "Error - can't open input file: " << infile << endl;
         // update the status variable
         status = OTHER; // placeholder until retrieval is attempted
         // create and return the verse object
         return Verse();  // default verse
      }
      isOpen = true;
   }

   // Use the index to look up the reference directly
   map<Ref, int>::iterator it = refIndex.find(ref);
   if (it != refIndex.end()) {
      // Found in index - seek to the byte offset and read the verse
      instream.clear();
      instream.seekg(it->second);
      string line;
      if (getline(instream, line)) {
         // update the status variable
         status = SUCCESS;
         // create and return the verse object
         Verse aVerse(line);
         return(aVerse);
      }
   }

   // Reference not found in index - determine error type
   bool foundBook = false;
   bool foundChapter = false;
   for (map<Ref, int>::iterator mi = refIndex.begin(); mi != refIndex.end(); ++mi) {
      if (mi->first.getBook() == ref.getBook()) {
         foundBook = true;
         if (mi->first.getChapter() == ref.getChapter()) {
            foundChapter = true;
            break;  // Found book and chapter, must be missing verse
         }
      }
   }

   // update the status variable
   if (!foundBook) {
      status = NO_BOOK;
   } else if (!foundChapter) {
      status = NO_CHAPTER;
   } else {
      status = NO_VERSE;
   }
   // create and return the verse object
   Verse aVerse;   // default verse
   return(aVerse);
}
// REQUIRED: Return the next verse from the Bible file stream if the file is open.
// If the file is not open, open the file and return the first verse.
Verse Bible::nextVerse(LookupResult& status)
{
   // If the file is not open, open the file and return the first verse.
   if (!isOpen) {
      instream.open(infile.c_str(), ios::in);
      if (!instream) {
         cerr << "Error - can't open input file: " << infile << endl;
         status = OTHER;
         return Verse();
      }
      isOpen = true;
   }

   // Return the next verse from the Bible file stream if the file is open.
   string line;
   if (getline(instream, line)) {
      status = SUCCESS;
      Verse verse(line);
      return verse;
   }

   status = OTHER;
   Verse verse;
   return verse;
}
// REQUIRED: Return an error message string to describe status
string Bible::error(LookupResult status)
{
   switch (status) {
      case SUCCESS:
         return "Success";
      case NO_BOOK:
         return "No such book";
      case NO_CHAPTER:
         return "No such chapter";
      case NO_VERSE:
         return "No such verse";
      default:
         return "Other error";
   }
}
void Bible::display()
{
   cout << "Bible file: " << infile << endl;
}

// OPTIONAL access functions
// OPTIONAL: Return the reference after the given ref
Ref Bible::next(const Ref ref, LookupResult& status)
{
   // Use the index to find the next reference after ref
   map<Ref, int>::iterator it = refIndex.find(ref);
   if (it != refIndex.end()) {
      ++it;  // Move to next entry in the map
      if (it != refIndex.end()) {
         status = SUCCESS;
         return it->first;
      }
   }
   status = OTHER;
   return ref;
}

// OPTIONAL: Return the reference before the given ref
Ref Bible::prev(const Ref ref, LookupResult& status)
{
   return ref;
}

// Diagnostic functions for testing the index
int Bible::getIndexSize() {
   return indexSize;
}

Ref Bible::getLastRefAdded() {
   return lastRefAdded;
}

int Bible::getLastOffsetAdded() {
   return lastOffsetAdded;
}

// Print the byte offset for a specific reference (for verification)
void Bible::printRefInfo(Ref ref) {
   map<Ref, int>::iterator it = refIndex.find(ref);
   if (it != refIndex.end()) {
      cout << "  ";
      ref.display();
      cout << " => byte offset " << it->second << endl;
   } else {
      cout << "  ";
      ref.display();
      cout << " => NOT FOUND in index" << endl;
   }
}
