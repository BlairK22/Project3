/* Demo server program for Bible lookup using AJAX/CGI interface
 * By James Skon, Febrary 10, 2011
 * updated by Bob Kasper, January 2020
 * updated by Deborah Wilson, February 2026
 * Mount Vernon Nazarene University
 *
 * This sample program works using the cgicc AJAX library to
 * allow live communication between a web page and a program running on the
 * same server that hosts the web server.
 *
 * This program is run by a request from the associated html web document.
 * A Javascript client function invokes an AJAX request,
 * passing the input form data as the standard input string.
 *
 * The cgi.getElement function parses the input string, searching for the matching
 * field name, and returing a "form_iterator" oject, which contains the actual
 * string the user entered into the corresponding field. The actual values can be
 * accessed by dereferencing the form iterator twice, e.g. **verse
 * refers to the actual string entered in the form's "verse" field.
 *
 * STUDENT NAME: Blair Karamaga
 */

#include <iostream>
#include <stdio.h>
#include <string.h>

/* Required libraries for AJAX to function */
#include "/home/class/csc3004/cgicc/Cgicc.h"
#include "/home/class/csc3004/cgicc/HTTPHTMLHeader.h"
#include "/home/class/csc3004/cgicc/HTMLClasses.h"

/* Include Project 1 classes for Bible verse retrieval */
#include "Ref.h"
#include "Verse.h"
#include "Bible.h"

using namespace std;
using namespace cgicc;

int main()
{
   /* A CGI program must send a response header with content type
    * back to the web client before any other output.
    * For an AJAX request, our response is not a complete HTML document,
    * so the response type is just plain text to insert into the web page.
    */
   cout << "Content-Type: text/plain\n\n";

   Cgicc cgi;  // create object used to access CGI request data

   // GET THE INPUT DATA
   // browser sends us a string of field name/value pairs from HTML form
   // retrieve the value for each appropriate field name
   form_iterator st = cgi.getElement("search_type");
   form_iterator book = cgi.getElement("book");
   form_iterator chapter = cgi.getElement("chapter");
   form_iterator verse = cgi.getElement("verse");
   form_iterator nv = cgi.getElement("num_verse");
   form_iterator ver = cgi.getElement("version");

   // Convert and check input data
   bool validInput = false;
   if (chapter != cgi.getElements().end())
   {
      int chapterNum = chapter->getIntegerValue();
      if (chapterNum > 150)
      {
         cout << "<p>The chapter number (" << chapterNum << ") is too high.</p>" << endl;
      }
      else if (chapterNum <= 0)
      {
         cout << "<p>The chapter must be a positive number.</p>" << endl;
      }
      else
      {
         validInput = true;
      }
   }

   // Additional input validation for verse number
   if (validInput && verse != cgi.getElements().end())
   {
      int verseNum = verse->getIntegerValue();
      if (verseNum <= 0)
      {
         cout << "<p>The verse must be a positive number.</p>" << endl;
         validInput = false;
      }
   }
   else if (validInput)
   {
      cout << "<p>Please enter a verse number.</p>" << endl;
      validInput = false;
   }

   // Validate book selection
   if (validInput && book == cgi.getElements().end())
   {
      cout << "<p>Please select a book.</p>" << endl;
      validInput = false;
   }

   /* CALL BIBLE CLASS FUNCTIONS TO LOOK UP THE REQUESTED VERSES */
   if (validInput)
   {
      // Get the integer values from the cgicc form data
      int bookNum = book->getIntegerValue();
      int chapterNum = chapter->getIntegerValue();
      int verseNum = verse->getIntegerValue();
      int numVerses = 1;
      if (nv != cgi.getElements().end())
      {
         numVerses = nv->getIntegerValue();
         if (numVerses < 1) numVerses = 1;
      }

      // Validate book number range
      if (bookNum < 1 || bookNum > 66)
      {
         cout << "<p><b>Error:</b> Invalid book number.</p>" << endl;
      }
      else
      {
         // Determine which Bible version file to use based on user selection
      string bibleFile = "/home/class/csc3004/Bibles/web-complete";  // default
      if (ver != cgi.getElements().end())
      {
         string version = **ver;
         if (version == "kjv")
            bibleFile = "/home/class/csc3004/Bibles/kjv-complete";
         else if (version == "dby")
            bibleFile = "/home/class/csc3004/Bibles/dby-complete";
         else if (version == "ylt")
            bibleFile = "/home/class/csc3004/Bibles/ylt-complete";
         else if (version == "webster")
            bibleFile = "/home/class/csc3004/Bibles/webster-complete";
      }

      // Create Bible object pointing to the selected Bible version file
      Bible bible(bibleFile);

         // Create a Ref from the book number, chapter, and verse
         Ref ref(bookNum, chapterNum, verseNum);

         // Look up the verse using the Bible class from Project 1
         LookupResult result;
         Verse aVerse = bible.lookup(ref, result);

         if (result == SUCCESS)
         {
            // Get reference info from the found verse
            Ref verseRef = aVerse.getRef();
            int currentChapter = verseRef.getChapter();
            int currentBook = verseRef.getBook();

            // Display book name and chapter heading
            cout << "<b>" << verseRef.getBookName() << " " << currentChapter << "</b><br>" << endl;

            // Display verse number and text
            cout << verseRef.getVerse() << " " << aVerse.getVerse() << "<br>" << endl;

            // Retrieve additional verses if requested
            for (int i = 1; i < numVerses; i++)
            {
               aVerse = bible.nextVerse(result);
               if (result == SUCCESS)
               {
                  verseRef = aVerse.getRef();

                  // Stop if we moved to a new book
                  if (verseRef.getBook() != currentBook)
                  {
                     break;
                  }

                  // Print new chapter heading if chapter changed
                  if (verseRef.getChapter() != currentChapter)
                  {
                     currentChapter = verseRef.getChapter();
                     cout << "<br><b>" << verseRef.getBookName() << " " << currentChapter << "</b><br>" << endl;
                  }

                  // Display verse number and text
                  cout << verseRef.getVerse() << " " << aVerse.getVerse() << "<br>" << endl;
               }
               else
               {
                  break;
               }
            }
         }
         else
         {
            // Display the specific error message from Bible class
            cout << "<p><b>Error:</b> " << bible.error(result) << "</p>" << endl;
         }
      }
   }
   else if (chapter == cgi.getElements().end())
   {
      cout << "<p>Please enter a chapter number.</p>" << endl;
   }

   return 0;
}
