/* Class Ref
 * Computer Science, MVNU
 * CSC-3004 Introduction to Software Development
 *
 * Ref is a class for representing a Bible reference consisting of
 *    * integer book   - the book from 1 (Genesis) to 66 (Revalation)
 *    * integer chap   - the chapter number >1
 *    * integer verse  - the verse number >1
 *
 * NOTE: You may add code to this file, but do not
 * delete any code or delete any comments.
 *
 * STUDENT NAME: Blair Karamaga 
 */

#ifndef Ref_H
#define Ref_H

#include <string> 
#include <stdlib.h>

using namespace std;

// GetNextToken returns a token from str, which contains all characters
// up to the first character from delimiters
string GetNextToken(string& str, const string& delimiters);

class Ref
{
   private:
      short book, chapter, verse;	// Reference information
	  
   public:
      // Constructors
      Ref();  	        // Default constructor
      Ref(string s); 	// Parse constructor - example parameter "43:3:16"
      Ref(const int, const int, const int); 	// Construct from three integers
      
	  // Accessors
      int getBook();	// Access book number
      int getChapter();	// Access chapter number
      int getVerse();	// Access verse number
      
	  // REQUIRED Comparison: determine if two references are equal
      bool operator==(const Ref &) const;
	  
      // OPTIONAL: < and > comparisons for Ref objects might also be useful
   
      // REQUIRED: Display the reference on cout, example output: John 3:16
	  // Your version of display should show the book name
      // corresponding to the stored book number.
      void display();
string getBookName();  // Return book name
};

#endif //Ref_H
