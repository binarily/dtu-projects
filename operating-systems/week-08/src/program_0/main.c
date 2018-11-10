/*! \file \brief The first user program - simply tests the thread creation routine
 *
 */
#include <scwrapper.h>


char thread_stack[4096];

int 
main(int argc, char* argv[])
{
 //Test scheduling against other processes
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
 
 //Show that scheduling works
 while(1){
  prints("Program 0\n");
  yield();
 }

 return 0;
}
