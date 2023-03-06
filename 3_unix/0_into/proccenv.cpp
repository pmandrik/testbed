#include <iostream>
#include <errno.h>
#include <vector>
#include <string.h>
using namespace std;

#include <stdlib.h>
#include <unistd.h>

void at_exit_1(void){
  puts("call exit 1");
}

void at_exit_2(void){
  string a = " call exit 2 ";
  for( char x : a )
    putchar( x );
}

extern char **environ;

int main(){
 
  cout << "REGISTER AT EXIT FUNCTION, will call in reverce order" << endl;
  atexit( at_exit_1 );
  atexit( at_exit_2 );
  
  cout << "ENVIRON" << endl;
  char **environ_ = environ;
  while( *environ_ != NULL ){
    cout << *environ_ << endl;
    environ_ += 1;
  }
  
  return 0;
}
