#include <iostream>
#include <errno.h>
#include <vector>
#include <string.h>
using namespace std;

#include <fcntl.h>
#include <unistd.h>

// LINUX ioctl(int fd, int request , ...)
#include <sys/ioctl.h>

int main(){
  // O_RDONLY, O_WRONLY, O_RDWR - read, write, read & write
  // O_EXEC, O_SEARCH
  
  //
  int d1 = open("fileio.tmp1", O_RDWR);
  cout << "FD = " << d1 << endl;
  perror(  "fileio() -> " );
  
  int d2 = open("fileio.tmp2", O_CREAT);
  cout << "FD = " << d2 << endl;
  perror(  "fileio() -> " );
  
  int d3 = open("fileio_fld", O_DIRECTORY);
  cout << "FD = " << d3 << endl;
  perror(  "fileio() -> " );
  
  // if path absolute than fd ignired
  // if path relative than fd - starting directory
  // if path relative and fd = AT_FDCWD -> same as open from current directory
  // to used by threads and  time-of-check-to-time-of-use problem
  int fd = d3;
  int d4 = openat(fd, "fileio.tmp2", O_RSYNC | O_RDWR);
  int d5 = openat(fd, "fileio.tmp3", O_RDWR);
  cout << "FD = " << d4  << " " << d5 << endl;
  perror(  "fileio() -> " );
  
  // ssize_t write(int fd, const void *buf, size_t nbytes);
  // Returns: number of bytes written if OK, −1 on error
  char data[100] = "HELLO WORLD\n";
  ssize_t written = write(d5, data, 10);
  cout << "written " << written << endl;
  perror(  "fileio() -> " );

  // off_t lseek(int fd, off_t offset, int whence);   
  // set reading/writing position of the reader
  // whence==SEEK_SET -> offset from beginning
  // whence==SEEK_CUR -> offset from current value
  // whence==SEEK_END -> offset from end of the file
  off_t pos1 =  lseek(d4, 0, SEEK_CUR);
  write(d4, "abcdefghij", 10);
  off_t pos2 =  lseek(d4, 10, SEEK_CUR);
  write(d4, "Abcdefghij", 10);
  cout << pos1 << " " << pos2 << " " << pos1 - pos2 << endl;
  
  // ssize_t read(int fd, void *buf, size_t nbytes);
  // Returns: number of bytes read, 0 if end of file, −1 on error
  // can read from file, terminal, network, pipe, devices
  lseek(d4, 0, SEEK_SET);
  char buf[1000];
  ssize_t bytes = read(d4, buf, 100);
  cout << bytes << " " << buf << endl;
  lseek(d4, 20, SEEK_SET);
  bytes = read(d4, buf, 100);
  cout << bytes << " " << buf << endl;
  
  // Single UNIX Specification
  pread(d5, buf, 10, 0); //  lseek + read atomically
  cout << buf << endl;
  pread(d5, buf, 10, 4);
  cout << buf << endl;
  // lseek + write atomically
  // ssize_t pwrite(int fd, const void *buf, size_t nbytes, off_t offset);
  // Returns: number of bytes written if OK, −1 on error

  // create(path, mode) is same as
  // open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
  
  //  existing file descriptor is duplicated by
  int d5_1 = dup(d5);
  int d5_2 = dup2(d5, 11);
  cout << "duplicates = " << d5_1 << " " << d5_2 << endl;
  
  // fcntl function - change property of the file arleady opened
  int answer = fcntl(d5, F_GETFL);
  cout << answer << " " << (answer & O_ACCMODE) << " " << O_RDWR << endl;
  
  // following is the same as fd = duo(0); mode is ignored
  int d10 = open("/dev/fd/0", O_RDWR);
  
  // close the file
  int result = close( d4 ); // #include <unistd.h>
  cout << result << endl;
  perror(  "fileio() -> " );
  
  // execrcises
  // 3.1
  // ubuffered - meaning there is no intermediate layer between process and kernel, so, yes
  // 3.2
  int arg1 = d5;
  int arg2 = 11;
  cout << "my dub2 call " << arg1 << " " << arg2 << endl;
  { // dup2, create copy of descriptor without fcntl
    if( arg1 == arg2 ) cout << "return " << arg1 << endl;
    
    close( arg2 );
    int i = 3;
    vector<int> to_close;
    for(; i < arg2; i++){
      int fd = open("tmp", O_CREAT);
      if( fd == arg2 ) break;
      if( fd == -1 ) cout << "error 1" << endl;
      to_close.push_back( i );
    }
    
    if( i != arg2-1 )
      close( arg2 );
    
    int fin = dup( arg1 );
    while( to_close.size() ) {
      int res = close( to_close.back() );
      if( res == -1 ) cout << "error 2" << endl;
      to_close.pop_back();
    }
    
    cout << "my dup2 result= " << fin << endl;
    
    int errnum = errno;
    cout << strerror( errnum ) << endl;
  }
  
  // 3.3
  // fd1 = open(path, oflags);
  // fd2 = dup(fd1);
  // fd3 = open(path, oflags);
  //
  // fd1 & fd2 share same file table ft1, and fd3 -> ft2
  // ft2 & ft1 points to same v-node
  // fcntl(fd1, F_SETFD) -> of fd1 only
  // fcntl(fd1, F_SETFL) -> of fd1 & fd2
  
  // 3.4
  // dup2(fd, 0);
  // dup2(fd, 1);
  // dup2(fd, 2);
  // if (fd > 2) close(fd);
  //
  // close 0,1,2 
  // redirect STDIO & STDERR & STDOUT to fd
  // 
  
  // 3.5
  // digit1>&digit2
  // redirect descriptor digit1 to the same file as descriptor digit2
  // ./a.out > outfile 2>&1
  // redirect 2 to 1 target 'outfile'
  // ./a.out 2>&1 > outfile
  // redirect 2 to STDOUT, then 1 to outfile
  
  // 3.6
  // possible to read, not possible to write
  int df = open("fileio_fld/fileio.tmp4", O_RDWR | O_APPEND);
  cout << "FD = " << df << endl;
  perror(  "fileio() -> " );
  
  char data_bif[100] = "hello worlds qwerty";
  char data_bof[100] = "0000000000000000000";
  int bytess = write(df, data_bif, 19);
  cout << bytess << endl;
  lseek(df, 0, SEEK_SET);
  bytess = read(df, data_bof, 19);
  cout << bytess << endl;
  cout << data_bof << endl;
  lseek(df, 0, SEEK_SET);
  bytess = write(df, data_bif+5, 5);
  cout << bytess << endl;
  
  return 0;
}
