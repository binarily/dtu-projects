#include <scwrapper.h>

int
main(int argc, char* argv[])
{
 if (0 != createprocess(1))
 {
  prints("createprocess of program 1 failed.\n");
  return 1;
 }

 if (0 != createprocess(2))
 {
  prints("createprocess of program 2 failed.\n");
  return 1;
 }

 while(1)
 {
  prints("Ping\n");
  yield();
 }

 return 0;
}
