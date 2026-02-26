/* Ref class function definitions
 * Computer Science, MVNU
 * CSC-3004 Introduction to Software Development
 *
 * NOTE: You may add code to this file, but do not
 * delete any code or delete any comments.
 *
 * STUDENT NAME: Blair Karamaga 
 */

#include "Ref.h"
#include <iostream>
#include <string>

using namespace std;

// Array of book names - index 0 unused, books 1-66
const string bookNames[] = {
    "", // 0 - unused
    "Genesis", "Exodus", "Leviticus", "Numbers", "Deuteronomy",
    "Joshua", "Judges", "Ruth", "1 Samuel", "2 Samuel",
    "1 Kings", "2 Kings", "1 Chronicles", "2 Chronicles", "Ezra",
    "Nehemiah", "Esther", "Job", "Psalms", "Proverbs",
    "Ecclesiastes", "Song of Solomon", "Isaiah", "Jeremiah", "Lamentations",
    "Ezekiel", "Daniel", "Hosea", "Joel", "Amos",
    "Obadiah", "Jonah", "Micah", "Nahum", "Habakkuk",
    "Zephaniah", "Haggai", "Zechariah", "Malachi",
    "Matthew", "Mark", "Luke", "John", "Acts",
    "Romans", "1 Corinthians", "2 Corinthians", "Galatians", "Ephesians",
    "Philippians", "Colossians", "1 Thessalonians", "2 Thessalonians", "1 Timothy",
    "2 Timothy", "Titus", "Philemon", "Hebrews", "James",
    "1 Peter", "2 Peter", "1 John", "2 John", "3 John",
    "Jude", "Revelation"
};

/* GetNextToken - general routine for getting next token from a string
 * Parameters
 *    str - string to search. search is destructive, i.e., token is removed.
 *    delimiters - string, a list of characters to use a delimiters between tokens
 * Returns: string token (removed from str)
 */
string GetNextToken(string& str, const string& delimiters = " ")
{
   // Skip delimiters at beginning                                          
   string::size_type startPos = str.find_first_not_of(delimiters, 0);
  
   // Find position of delimiter at end of token                                           
   string::size_type endPos = str.find_first_of(delimiters, startPos);

   // Found a token, remove it from string, and return it                       
   string next = str.substr(startPos, endPos - startPos);
   string rest = str.substr(endPos - startPos + 1, string::npos);
   str = rest;
   return(next);
}

// Ref member functions

// Default constructor
Ref::Ref() {book = 0; chapter = 0; verse = 0;}  	

// Parse constructor - receives a line "34:5:7 text"
Ref::Ref(const string s)
{ 
   string rtext = s; // make local copy of string to avoid modifying parameter
   // parse the reference - notice, currently there is no error checking!
    
   // Get book number
   string strbook = GetNextToken(rtext,":");
   book = atoi(strbook.c_str());
    
   // Get the chapter number
   string strchap = GetNextToken(rtext,":");
   chapter = atoi(strchap.c_str());
    
   // Get the verse number
   string strverse = GetNextToken(rtext," ");
   verse = atoi(strverse.c_str());
}

// Construct Ref from three integers
Ref::Ref(const int b, const int c, const int v) 
{ 	
   book = b;
   chapter = c;
   verse = v;
}

// Accessors
int Ref::getBook() {return book;}	     // Access book number
int Ref::getChapter() {return chapter;}	 // Access chapter number
int Ref::getVerse() {return verse;};     // Access verse number


// REQUIRED: == comparison
bool Ref::operator==(const Ref & ref) const
{
   return (book == ref.book && chapter == ref.chapter && verse == ref.verse);
}

// OPTIONAL: define < and > comparisons


// Display Reference
void Ref::display()
{
   // TODO: modify display function to show book name instead of book number
if (book >= 1 && book <= 66) {
      cout << bookNames[book] << " " << chapter << ":" << verse;
   } else {
      cout << book << ":" << chapter << ":" << verse;
   }
 }
// Returning the book name as a string
string Ref::getBookName() {
   if (book >= 1 && book <= 66) {
      return bookNames[book];
   }
   return "Unknown";
}
