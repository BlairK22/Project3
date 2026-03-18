/* logfile.h
 * defines a simple function to log messages to a logfile
 * with a conditional macro to enable logging by defining a constant
 * 
 * Usage:
 * To turn on logging within a source file, add these lines near the top
 * of the file, substituting your own values for USERNAME and PROGNAME:
      #define logging
	  #define LOG_FILENAME "/tmp/USERNAME-PROGNAME.log"
      #include "logfile.h"
 * At the beginning of the main function, add
      #ifdef logging logFile.open(LogFile, ios::out); #endif
 * Wherever you want to write a log message, use
      log(messageString);
 * For example, to log receiving a request, use log("Request: " + keyString);
 * To turn off logging, comment out #define logging
 *    and recompile the file (using make).
 */

#ifdef logging
const string logFilename =  LOG_FILENAME;
ofstream logFile;
string timestamp() {
  time_t _tm = time( NULL );
  struct tm * curtime = localtime ( &_tm );
  string t = asctime(curtime);
  t.erase(t.end() - 1);
  return t;
}

#define log(message) logFile << timestamp() << ": " << message << endl
#else
#define log(message)
#endif // logging
