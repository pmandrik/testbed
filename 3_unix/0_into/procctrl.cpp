#include <iostream>
#include <errno.h>
#include <vector>
#include <string.h>
using namespace std;

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
  
  int var = 0; // automatic
  register int var_reg = 0;
  volatile int var_vol = 0;
  int * a = new int;
  *a = 0;
  
  int fd = open("procctrl.log", O_RDWR);
  
  pid_t pid = fork(); // copy memory, no modifications in fath memory
  // pid_t pid = vfork(); // no copy, fath memory undefined
  
  if( pid == 0){
    var++;
    var_reg++;
    var_vol++;
    (*a)++;
    
    cout << "fork" << var << " " << var_reg << " " << var_vol << " " << a << " " << *a << " " << fd << endl;
    
    off_t pos0 =  lseek(fd, 10, SEEK_CUR);
    cout << pos0 << endl;
  } else {
    sleep(2);
    cout << "fath" << var << " " << var_reg << " " << var_vol << " " << a << " " << *a << " " << fd << endl;
    
    off_t pos1 =  lseek(fd, 0, SEEK_CUR);
    cout << pos1 << endl;
  }
  
  return 0;
}



