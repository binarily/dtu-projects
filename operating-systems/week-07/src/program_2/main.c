#include <scwrapper.h>

int
main(int argc, char* argv[])
{
  int i = 0;
 while(i < 3)
 {
  prints("Pang\n");
  i++;
  yield();
 }
 return 0;
}
