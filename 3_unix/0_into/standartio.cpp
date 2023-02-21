#include <iostream>
#include <errno.h>
#include <vector>
#include <string.h>
using namespace std;

#include <fcntl.h>
#include <unistd.h>

#include <stdio.h> // stdin stdout stderr

struct{
  short count;
  long total;
} item;

int main(){
  cout << stderr << " " << stdout << " " << stdin << endl;

  // open part
  FILE *fp = fopen("stdio_0.txt", "r+");
  if( not fp ){
    fp = fopen("stdio_0.txt", "w+");
  }

  // read part
  string chars;
  bool f1read = true;
  while(true){
    int ch = getc( fp );
    if( feof( fp ) > 0 ) break;

    chars.push_back( (char) ch );

    if( not f1read ) continue;
    ungetc( ch+1, fp );
    f1read = false;
  }
  cout << "read \'" << chars << "\'" << endl;

  // write part
  chars = "aaa";
  for( int ch : chars ){
    putc( ch, fp );
  }

  // binary read/write part
  FILE *fp2 = fopen("stdio_1.txt", "r+");
  if( not fp2 ){
    fp2 = fopen("stdio_1.txt", "w+");
    item.count = 0;
    item.total = 0;
  } else {
    while(true){
      if( feof( fp2 ) > 0 ) break;
      fread( &item, sizeof(item), 1, fp2);
    }
  }

  cout << item.count << " " << item.total << endl;

  item.count++;
  item.total++;
  fwrite( &item, sizeof(item), 1, fp2);

  fclose(fp);
  fclose(fp2);

  return 0;
} 


