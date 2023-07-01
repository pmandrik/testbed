#include <limits.h> // compile limits
#include <unistd.h> // run-time function limits

#include <vector>
#include <iostream>
#include <errno.h>
using namespace std;

#include <sys/stat.h> // stat fstat lstat
#include <fcntl.h> // creat
#include <sys/sysmacros.h> // minor & major

int main(int argc, char *argv[]){
  
  struct stat buf;
  char *ptr;

  vector<string> files = {"fileio_fld", "fildir", "/dev/log", "/dev/tty", "/var/lib/texmf/", "/dev/fb0", "/lib/libnss_mdns_minimal.so"};
  for(int i = 0; i < argc; ++i){
    // printf("%s: ", argv[i]);
    files.push_back( argv[i] );
  }

  // ==============================================
  cout << "file type check =============" << endl;
  for( auto file : files ){
    string ptr = "** unknown mode **";
    
    int status = lstat( file.c_str(), &buf );
    if( status < 0 ){
      perror( (file + " -> ").c_str() );
      continue; 
    }
    
    if (S_ISREG(buf.st_mode)) ptr = "regular";
    else if (S_ISDIR(buf.st_mode)) ptr = "directory";
    else if (S_ISCHR(buf.st_mode)) ptr = "character special";
    else if (S_ISBLK(buf.st_mode)) ptr = "block special";
    else if (S_ISFIFO(buf.st_mode)) ptr = "fifo";
    else if (S_ISLNK(buf.st_mode)) ptr = "symbolic link";
    else if (S_ISSOCK(buf.st_mode)) ptr = "socket";
    
    cout << file << " " << ptr << endl;
  }
  
  // ==============================================
  cout << "access check =============" << endl;
  for( auto file : files ){
    if( access( file.c_str(), R_OK ) < 0 ){
      perror( (file + " -> ").c_str() );
      continue;
    }
    cout << file << " R_OK" << endl;
  }
  
  // ==============================================
  cout << "access mask =============" << endl;
  umask(0);
  int RWRWRW =  (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
  creat("fildir_1.txt", RWRWRW);
  umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  creat("fildir_2.txt", RWRWRW);
  
  lstat("fildir_1.txt", &buf);
  cout << (buf.st_mode & S_IROTH) << " " << (buf.st_mode & S_IRGRP) << endl;
  lstat("fildir_2.txt", &buf);
  cout << (buf.st_mode & S_IROTH) << " " << (buf.st_mode & S_IRGRP) << endl;

  // ==============================================  
  cout << "chmod =============" << endl;
  int res = chmod("fildir_2.txt", RWRWRW);
  lstat("fildir_2.txt", &buf);
  cout << res << " " << (buf.st_mode & S_IROTH) << " " << (buf.st_mode & S_IRGRP) << endl;

  // ==============================================
  cout << "chown =============" << endl;
  cout << "_POSIX_CHOWN_RESTRICTED " << _POSIX_CHOWN_RESTRICTED << endl;
  int pcr = pathconf("fildir_1.txt", _PC_CHOWN_RESTRICTED);
  cout << pcr << endl;
  pcr = pathconf("/etc/vconsole.conf", _PC_CHOWN_RESTRICTED);
  cout << pcr << endl;
  
  res = chown("fildir_2.txt", 1, 1);
  cout << res << endl;
  perror( "chown -> " ); // in my OS chown only for root
  lstat("fildir_2.txt", &buf);
  cout << res << " " << (buf.st_uid) << " " << (buf.st_gid) << endl;
  lstat("fildir_1.txt", &buf);
  cout << res << " " << (buf.st_uid) << " " << (buf.st_gid) << endl;
  
  // ================ file size
  cout << buf.st_size << endl; // initial
  int fd = open("fildir_1.txt", O_WRONLY);
  write(fd, "HELLO WORLD", 11);
  cout << buf.st_size << endl; // not updated
  lstat("fildir_1.txt", &buf);
  cout << "written = " << buf.st_size << endl; // updated with extra 11 bytes
  cout << buf.st_blocks << " " << buf.st_blocks << endl;
  ftruncate(fd, 7);
  lstat("fildir_1.txt", &buf);
  cout << "truncated = " << buf.st_size << endl;
  
  // st_nlink ================
  cout << "st_nlink ===============" << endl;
  cout << "st_nlink = " << buf.st_nlink << endl;
  lstat("fileio_fld", &buf);
  cout << "st_nlink = " << buf.st_nlink << endl;
  
  // link & unlink ================
  cout << "link ===============" << endl;
  lstat("fildir_1.txt", &buf);
  cout << "st_nlink = " << buf.st_nlink << endl;
  link( "fildir_1.txt", "fildir_1.txt_c1" );
  link( "fildir_1.txt_c1", "fildir_1.txt_c2" );
  link( "fildir_1.txt_c2", "fildir_1.txt_c3" );
  lstat("fildir_1.txt", &buf);
  cout << "st_nlink = " << buf.st_nlink << endl;
  unlink( "fildir_1.txt_c1" );
  unlink( "fildir_1.txt_c2" );
  remove( "fildir_1.txt_c3" ); // same as unlink or rmdir
  lstat("fildir_1.txt", &buf);
  cout << "st_nlink = " << buf.st_nlink << endl;
  
  // symlink  ================
  cout << "symlink ===============" << endl;
  symlink("/my/target/does/not/exist", "my_target.symlink");
  char nbuf[100];
  readlink("my_target.symlink", nbuf, 25);
  cout << nbuf << endl;
  unlink("my_target.symlink");
  
  // mkdir  ================
  int RWXRWXRWX =  (S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH);
  cout << "mkdir ===============" << endl;
  mkdir("fildir_fld", RWXRWXRWX);
  int fd_f = open("fildir_fld", O_RDONLY);
  mkdirat( fd_f, "fildir_fld2", RWXRWXRWX);
  
  cout << rmdir( "fildir_fld" ) << endl; // not empty
  cout << rmdir( "fildir_fld/fildir_fld2" ) << endl;
  cout << rmdir( "fildir_fld" ) << endl;
  
  // chdir  ================
  cout << "chdir ===============" << endl;
  char cwd_buff[1000], cwd_bufff[1000];
  cout << getcwd(cwd_buff, 1000) << endl;
  chdir("../..");
  cout << getcwd(cwd_bufff, 1000) << endl;
  chdir(cwd_buff);
  cout << getcwd(cwd_bufff, 1000) << endl;

  // dev special files
  cout << "dev ===============" << endl;
  lstat("fildir_1.txt", &buf);
  cout << major(buf.st_dev) << " " << minor(buf.st_dev) << endl;
  
  // questions
  // 4.1 - stat follow the symbol links
  // 4.2 - no any permissions to the file
  cout << "4.2" << endl;
  umask( 0777 );
  cout << creat("fildir_3.txt", RWXRWXRWX) << endl;
  // --wxrw-rw-
  perror("4.2 -> ");
  // 4.3 - ok
  // 4.4 - rewrite file
  umask( 022 );
  cout << creat("fildir_4.txt", RWXRWXRWX) << endl;
  perror("4.4 -> ");
  cout << creat("fildir_4.txt", RWXRWXRWX) << endl;
  perror("4.4 -> ");
  // 4.5 - no, link has always at least empty string size, directory has size of . and .. elements names ?
  // 4.6 TODO
  // 4.7 - GW permission inherited from folder
  // 4.8 - du goes throu directories, df check file system
  // 4.9 - if unlink trigger deletion of the file
  // 4.10 - crash
  // 4.11 - TODO
  
  return 0; 
}





