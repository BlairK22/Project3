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
}

// Constructor – pass bible filename
Bible::Bible(const string s) { 
   infile = s; 
   isOpen = false;
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
   } else {
      // Reset to beginning of file for new search
      instream.clear();
      instream.seekg(0, ios::beg);
   }
      
   bool foundBook = false;
   bool foundChapter = false;
   
   string line;
   while (getline(instream, line)) {
      // create and return the verse object
      Verse aVerse(line);  // Verse object constructed from a line in the file
      Ref currentRef = aVerse.getRef();
      
      // Check if we found the book
      if (currentRef.getBook() == ref.getBook()) {
         foundBook = true;
         
         // Check if we found the chapter
      if (currentRef.getChapter() == ref.getChapter()) {
            foundChapter = true;
            
            // Check if we found the verse
      if (currentRef.getVerse() == ref.getVerse()) {
               // update the status variable
               status = SUCCESS;
               return(aVerse);
            }
         }
         // If we passed the chapter, verse doesn't exist
         else if (currentRef.getChapter() > ref.getChapter() && foundChapter) {
            // update the status variable
            status = NO_VERSE;
            return Verse();
         }
      }
      // If we passed the book, check what we found
      else if (currentRef.getBook() > ref.getBook()) {
         // update the status variable
         if (!foundBook) {
            status = NO_BOOK;
         } else if (!foundChapter) {
            status = NO_CHAPTER;
         } else {
            status = NO_VERSE;
         }
         return Verse();
      }
   }
   
   // Reached end of file - update the status variable
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
   return ref;
}

// OPTIONAL: Return the reference before the given ref
Ref Bible::prev(const Ref ref, LookupResult& status)
{
   return ref;
}
