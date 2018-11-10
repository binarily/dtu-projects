#include <scwrapper.h>

char thread_stack[4096];
int sem, counter;

void competitor1(void){
	int i;
	semaphoredown(sem);
	for(i=0;i<3;i++){
		counter++;
		printhex(counter);
		prints(" Up - thread 1, program 1\n");
		yield(); //Move on to next thread
	}
	semaphoreup(sem);
	terminate();
}

void competitor2(void){
	int j;
	semaphoredown(sem);
	for(j=0;j<3;j++){
		counter--;
		printhex(counter);
		prints(" Down - thread 2, program 1\n");
		yield(); //Move on to next thread
	}
	semaphoreup(sem);
	terminate();
}

int main(int argc, char* argv[])
{
  prints("Program 1 - only to initialize threads\n");
  //Create a semaphore
  sem = createsemaphore(1);
  //Initialize the counter
  counter = 0;
  //Initialize threads
  createthread(competitor1, thread_stack+4096);
  createthread(competitor2, thread_stack+8192);
  return 0;
}
