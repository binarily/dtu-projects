#include <scwrapper.h>

int
main(int argc, char* argv[])
{
 //Show that scheduling works
 while(1)
 {
  prints("Program 2\n");
  yield();
 }
 return 0;
}
