/* Verse class function definitions
 * Computer Science, MVNU
 * CSC-3004 Introduction to Software Development
 *
 * NOTE: You may add code to this file, but do not
 * delete any code or delete any comments.
 *
 * STUDENT NAME: Blair Karamaga 
 */

#include "Verse.h"
#include <iostream>

using namespace std;

// Default constructor
Verse::Verse()
{  	
   verseText = "Uninitialized Verse!";
   verseRef = Ref();
} 

// REQUIRED: Parse constructor - pass verse string from file
Verse::Verse(const string s)
{
   // TODO: use Ref constructor to create verseRef
   // store the remainder of the string in verseText
   // Use Ref constructor to create verseRef (parses book:chapter:verse)
   verseRef = Ref(s);
   
   // Find the space after the reference and store the rest as verseText
   string::size_type spacePos = s.find(' ');
   if (spacePos != string::npos) {
      verseText = s.substr(spacePos + 1);
   } else {
      verseText = "";
   }
}  	

// REQUIRED: Accessors
string Verse::getVerse()
{
   return verseText;
}

Ref Verse::getRef()
{
   return verseRef;
}

// display reference and verse
void Verse::display()
{
   verseRef.display();
   cout << " " << verseText;
}
