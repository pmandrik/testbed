// use only the POSIX.1
#define _POSIX_C_SOURCE 200809L

// XSI version 4 
#define _XOPEN_SOURCE 700
// also gcc -D_XOPEN_SOURCE=700 will work

#include <limits.h> // compile limits
#include <unistd.h> // run-time function limits

#include <iostream>
#include <errno.h>
using namespace std;

int main(){

  // ISO
  cout << INT_MAX << endl;
  cout << LONG_MAX << endl;
  
  // POSIX
  cout << _POSIX_TIMER_MAX << endl;
  
  //  either the value of the variable (a return value ≥ 0) or an indication that the value is indeterminate. An indeterminate value is indicated by returning −1 and not changing the value of errno.
  cout << sysconf(_SC_LOGIN_NAME_MAX) << endl;
  perror(  "sysconf(_SC_LOGIN_NAME_MAX) -> " );
  
  cout << "_SC_OPEN_MAX = " << _SC_OPEN_MAX << " " << sysconf(_SC_OPEN_MAX) << endl;
  
  // return −1 and set errno to EINVAL if the name isn’t one of the appropriate constants
  cout << sysconf(9999999) << endl;
  perror(  "sysconf(9999999) -> " );
  
  // PATH_MAX
  cout << "PATH_MAX = " << PATH_MAX << ", ";
  cout << pathconf("/etc/", _PC_PATH_MAX) + 1 << endl; //not working
  
  long posix_version = sysconf(_SC_VERSION);
  long xsi_version   = sysconf(_SC_XOPEN_VERSION);
  cout << "POSIX, XSI = ";
  cout << posix_version << ", " << xsi_version << endl;
  
  // optionnal constants
  cout << _POSIX_RAW_SOCKETS << " " << sysconf(_SC_RAW_SOCKETS) << endl;
  
  return 0; 
}
