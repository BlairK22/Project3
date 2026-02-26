/* Class Verse
 * Computer Science, MVNU
 * CSC-3004 Introduction to Software Development
 *
 * A Verse object represents a complete Bible verse, including its reference
 *    * Ref ref          - the reference (e.g., 1:1:1)
 *    * string verseText - the actual verse text (without reference)
 *
 * NOTE: You may add code to this file, but do not
 * delete any code or delete any comments.
 *
 * STUDENT NAME: Blair Karamaga
 */

#ifndef Verse_H
#define Verse_H

#include <string> 
#include <stdlib.h>
#include "Ref.h"

using namespace std;

class Verse
{
   private:
      Ref verseRef;       // The reference for this verse.
      string verseText;   // Actual verse text (without reference)

   public:
      Verse();            // Default constructor
   
      // REQUIRED: implement parse constructor
	  // pass entire line from file including reference and text
      Verse(const string s);
		
      // REQUIRED: implement accessor functions
      string getVerse();
      Ref getRef();

      // Display Verse on cout
	  // Display reference & verse [with line breaks as needed]
	  // The reference should be displayed by calling verseRef.display()
      void display();    
};

#endif //Verse_H
