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

int main(int argc, char *argv[]){
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
    exit(127);
  } 
  cout << "create fork with pid " << pid << endl;
  cout << "wait fork ..." << endl;
  int status;
  pid = waitpid( pid, &status, 0 );
  cout << "fork done! " << status << endl;
} 


