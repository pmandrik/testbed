// read directory
#include <dirent.h>

// read,  write std
#include <unistd.h>

// 
#include <sys/wait.h>

// error handling
#include <errno.h>
#include <string.h> // strerror
#include <stdio.h>  // perror

#include <iostream>
#include <vector>

using namespace std;
// #include "apue.h"

void sig_int(int a){
  cout << "interrupr " << a << endl;
}

void list_dir( string path ){
  DIR * dir = opendir( path.c_str() );
  struct dirent * dirp;
  while( true ){
    dirp = readdir( dir );
    if( dirp == NULL ) break;
    cout << dirp->d_name << endl;
  }
}

#include <sys/resource.h>
#include <setjmp.h>
jmp_buf jmpbuffer;
void jumper(){
  cout << "jumper call" << endl;
  longjmp(jmpbuffer, 1);
}

void jump_point(){
  cout << "jump_point start" << endl;
  int answer = setjmp(jmpbuffer);
  if( answer == 1 ){
    cout << "jump_point someone jumped to this ONE " << endl;
  } else if( answer == 2 ){
    cout << "jump_point someone jumped to this TWO " << endl;
  } else {
    jumper();
  }
}

int main(int argc, char *argv[]){
    /*
Least-privilege design - programs should use the least privilege necessary to given task: `setuid(uid_t uid)`, `setgid(gid_t gid)` - root will set real UID, eff UID, saved set-UID; not-root will set eff UID to real UID or saved set-UID. Set-UID & eff UID is taken from executable set-UID bit. Real UID is set by login program.  
`seteuid(uid_t uid)`& `setegid(gid_t gid)` - set only effective ids.  

`#! pathname [ optional-argument ]` - interpreter files e.g. `#!/bin/awk -f`, `#!/bin/sh`  

`int system(char * cmd)`

TODO
write daemon program which will monitor given folder
when file is received it will create a fork to analyze file
if it is a text file -> create report about size or something  
if it is a image     -> create report about metadata  
  */

  vector<string> vars = {"HOME", "DATEMSK", "LANG", "LC_ALL", "LINES", "LOGNAME", "MSGVERB", "PATH", "PWD", "SHELL", "TERM", "TMPDIR", "TZ"};
  for( string var : vars ){
    char * data = getenv( var.c_str() );
    if( data )
      cout << var << " " << data << endl;
    else 
      cout << var << " " << "NULL" << endl;
  }

  jump_point();
  cout << "!!!" << endl;

  vector<int> limis = {RLIMIT_AS, RLIMIT_CORE, RLIMIT_CPU, RLIMIT_DATA, RLIMIT_FSIZE, RLIMIT_MEMLOCK, RLIMIT_MSGQUEUE, RLIMIT_NICE, RLIMIT_NOFILE, RLIMIT_NOFILE, RLIMIT_NPROC, RLIMIT_RSS, RLIMIT_SIGPENDING, RLIMIT_STACK};
  for( int var : limis ){
    struct rlimit *rlptr;
    int answer =  getrlimit( var,  rlptr);
    if( not answer )
      cout << var << " " << rlptr->rlim_cur << " " << rlptr->rlim_max << endl;
    else 
      cout << var << " " << "NULL" << endl;
  }
  exit(0);

  // Exercieses
  // 1. Verify on your system that the directories dot and dot-dot are not the same, except in the root directory.
  list_dir("/.");
  list_dir("/..");
  list_dir("/home/me/work/projects/LEARNING/.");
  list_dir("/home/me/work/projects/LEARNING/..");
  // 2. In the output from the program in Figure 1.6, what happened to the processes with process IDs 852 and 853?
  // unspecified
  // 3. In Section 1.7, the argument to perror is defined with the ISO C attribute const, whereas the integer argument to strerror isn’t defined with this attribute. Why?
  // 4,5
  int j = 0, i = 1;
  if(false){
    for( ; i > 0; i++, j++){ }
    cout << j << " " << i << endl;  
  }
  /*
  >>> 1970 + 2147483647 / 31276800
  2038
  >>> 2147483647 / 100 / 60 / 60 / 24
  248
  */
  
  // user ID, group ID
  printf("uid = %d, gid = %d\n", getuid(), getgid());
  
  // errno / man intro(2)
  cout << "EACCES = " << EACCES << endl;
  DIR * dp0 = opendir("/etc1233");
  // readdir( dp0 );
  int errnum = errno;
  cout << strerror( errnum ) << endl;
  perror(  (string("my perror -> ") + argv[0]).c_str() );

  // get list of files
  DIR * dp;
  struct dirent * dirp;
  string dname = argc < 2 ? "." : argv[1];

  dp = opendir( dname.c_str() );
  if( dp == NULL ){
    cout << "no dir " << dname << endl;
    return 1;
  }
  else cout << "open dir ok " << dname << endl;

  while( true ){
    dirp = readdir( dp );
    if( dirp == NULL ) break;
    cout << dirp->d_name << endl;
  }
  closedir( dp );

  // read stdin & write stdout
  // & signals
  if (signal(SIGINT, sig_int) == SIG_ERR)
    printf("signal error\n");

  cout << "print char into stdin ... empty string to exit" << endl;
  int BUFFSIZE = 100;
  char buf[BUFFSIZE];
  while( true ){
    int n = read(STDIN_FILENO, buf, BUFFSIZE);
    if(n == 1) break;
    for(int i = 0; i < BUFFSIZE; ++i)
      buf[i] += 1;
    write(STDOUT_FILENO, buf, n);
  }

  // get process id
  cout << "Process id" << getpid() << endl;

  // fork execute
  pid_t pid = fork();
  if( pid == 0 ) {
    cout << "Hi, I'm fork" << endl;
    string execer = string(getcwd(buf, BUFFSIZE)) + "/hello.sh";
    execlp( execer.c_str(), "" );
    exit(127);o
  } 
  cout << "create fork with pid " << pid << endl;
  cout << "wait fork ..." << endl;
  int status;
  pid = waitpid( pid, &status, 0 );
  cout << "fork done! " << status << endl;
} 















