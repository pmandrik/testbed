#include <iostream>
#include <errno.h>
#include <vector>
#include <string.h>
using namespace std;

#include <pwd.h>
#include <shadow.h>

#include <sys/stat.h> // stat fstat lstat

#include <unistd.h> // getgroups

#include <grp.h> // getgrgid

#include <sys/utsname.h> // uname

int main(){
  struct stat info;
  stat("sysinfo.cpp", &info);
  cout << info.st_uid << endl;
  struct passwd * pwuid = getpwuid( info.st_uid );
  cout << pwuid->pw_name << endl;
  cout << pwuid->pw_passwd << endl;
  cout << pwuid->pw_dir << endl;
  
  struct passwd * pwuid2 = getpwnam( "me" );
  cout << pwuid2->pw_name << endl;
  cout << pwuid2->pw_passwd << endl;
  cout << pwuid2->pw_dir << endl;
  
  cout << "LOOK OVER PASSWD ENTRIES : " << endl;
  struct passwd *ptr;
  setpwent();
  while ((ptr = getpwent()) != NULL)
    cout << ptr->pw_name << endl;
  endpwent();
  
  cout << "LOOK OVER SHADOWS : " << endl;
  struct spwd *ptrs;
  setspent();
  while ((ptrs = getspent()) != NULL)
    cout << ptrs->sp_namp << " " << ptrs->sp_pwdp << endl;
  endspent();
  
  cout << "GET SUPLIMENTARY GROUPS" << endl;
  gid_t sgroups[1000];
  int ngroups = getgroups(0, sgroups);
  cout << "get N = " << ngroups << endl;
  ngroups = getgroups(ngroups+10, sgroups);
  for(int i = 0; i < ngroups; i++){
    cout << i << " " << sgroups[i] << endl;
    struct group * gr = getgrgid( sgroups[i] );
    cout << gr->gr_name << endl;
  }
  
  perror("before uname error ");
  cout << "GET INFORMATION ON THE CURRENT HOST" << endl;
  struct utsname *name;
  cout << uname( name ) << endl;
  perror("uname error ");

  char hostname[1000];
  gethostname(hostname, 1000);
  cout << hostname << endl;
  
  if( uname( name ) != -1 ){
    cout << "sysname "  << name->sysname << endl;
    cout << "nodename " << name->nodename << endl;
    cout << "release "  << name->release << endl;
    cout << "version "  << name->version << endl;
    cout << "machine "  << name->machine << endl;
  }
  
  cout << "TIME_T check" << endl;
  time_t a = 1;
  int x = 0;
  while(true){
    a *= 10;
    cout << a << endl;
    if( a == 0 ) break;
  }
  
  return 0;
}










