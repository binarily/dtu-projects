/* Copyright (c) 1997-2017, FenixOS Developers
   All Rights Reserved.

   This file is subject to the terms and conditions defined in
   file 'LICENSE', which is part of this source code package.
 */

/*! \file kernel_customization.c This file holds definitions
  for the kernel that you can change and alter. */

#include <stdint.h>
#include <instruction_wrappers.h>
#include <sysdefines.h>

#include "kernel.h"

void kernel_late_init(void)
{
  int i;
  //Initialize processes and threads  
 for(i=0;i<MAX_PROCESSES;i++)
   processes[i].exists = 0;
 for(i=0;i<MAX_THREADS;i++)
   threads[i].exists = 0;
 //Set up first process
 create_process(0);
 /* Go to user space. */
 go_to_user_space();
}

void handle_system_call(void)
{
 switch (current_thread->eax)
 {
  case SYSCALL_VERSION:
  {
   current_thread->eax = 0x00010000;
   break;
  }
 case SYSCALL_PRINTS:
  {
    kprints((char*)current_thread->edi);
    current_thread->eax = ALL_OK;
    break;
  }
 case SYSCALL_PRINTHEX:
  {
    kprinthex(current_thread->edi);
    current_thread->eax = ALL_OK;
    break;
  }
 case SYSCALL_CREATEPROCESS:
  {
    current_thread->eax = create_process(current_thread->edi);
    break;
  }
 case SYSCALL_TERMINATE:
  {
    terminate();
    break;
  }
 case SYSCALL_YIELD:
  {
    yield();
    break;
  }
  case SYSCALL_CREATETHREAD:
  {
  	current_thread->eax = create_thread((uint32_t*)current_thread->edi, (uint32_t*)current_thread->esi);
  	break;
  }

  default:
  {
   /* Unrecognized system call. Not good. */
   current_thread->eax = ERROR_ILLEGAL_SYSCALL;
  }
 }

 go_to_user_space();
}

int create_process(uint8_t execute_point){
  //Find a place for process
  int i, process_id, thread_id;
  process_id = -1;
  thread_id = -1;
  for(i=0;i<MAX_PROCESSES;i++){
    if(!processes[i].exists){
      process_id = i;
      break;
    }
  }
  if(process_id == -1){
    return ERROR;
  }
  //Find a place for a thread
  for(i=0;i<MAX_THREADS;i++){
    if(!threads[i].exists){
      thread_id = i;
      break;
    }
  }
  if(thread_id == -1){
    return ERROR;
  }
  //As there is a place for both the process and the thread, we put them in
  processes[process_id].exists = 1;
  processes[process_id].number_of_threads = 1;

  threads[thread_id].exists = 1;
  threads[thread_id].process = &(processes[process_id]);
  threads[thread_id].eip = executable_table[execute_point];

  //Return
  return ALL_OK;
}
void terminate (void){
  //Remove current thread
  current_thread->exists = 0;
  current_thread->process->number_of_threads--;
  //If the process has no threads: remove it as well
  if(current_thread->process->number_of_threads == 0)
    current_thread->process->exists = 0;
  //Move to the next thread (nothing to do here)
  yield();
}
void yield (void){
  int i, current_id;
  current_id = 0;
  //Calculate current thread ID
  for(i=0;i<MAX_THREADS;i++){
    if(&threads[i]==current_thread){
      current_id = i;
      break;
    }
  }
  //Try to find the next thread
  for(i = current_id+1; i < MAX_THREADS;i++){
    if(threads[i].exists){
      current_thread->eax = ALL_OK;
      current_thread = &(threads[i]);
      return;
    }
  }
  //If not the next, then the first in the array (or the same one, if no other)
  for(i=0;i<current_id+1;i++){
    if(threads[i].exists){
      current_thread->eax = ALL_OK;
      current_thread = &(threads[i]);
      return;
    }
  }
  //If absolutely no thread exists, then halt the machine - nothing will be called
  halt_the_machine();
  return;
}
int create_thread(uint32_t* instructions, uint32_t* stack){
  //Find parent process in array
  int i, thread_id;
  thread_id = -1;

  //Find a place for a thread
  for(i=0;i<MAX_THREADS;i++){
    if(!threads[i].exists){
      thread_id = i;
      break;
    }
  }
  if(thread_id == -1){
    return ERROR;
  }
  //As there is a place for the thread, we set it up
  threads[thread_id].exists = 1;
  threads[thread_id].process = current_thread->process;
  threads[thread_id].eip = (uint32_t)instructions;
  threads[thread_id].esp = (uint32_t)stack;
  
  //We need to increment the thread count in a process
  current_thread->process->number_of_threads++;
  //Return
  return ALL_OK;
}
/* Add more definitions here if you need. */
