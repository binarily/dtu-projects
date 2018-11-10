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

struct port ports[MAX_THREADS];

void kernel_late_init(void)
{
 int i;
  //Initialize processes and threads  
 for(i=0;i<MAX_PROCESSES;i++)
   processes[i].exists = 0;
 for(i=0;i<MAX_THREADS;i++){
   threads[i].exists = 0;
   threads[i].next = (struct thread*)-1;
 }
 for(i=0;i<MAX_THREADS;i++){
 	ports[i].exists = 0;
 	ports[i].thread_list = (struct thread*)-1;
 }
 	
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

  case SYSCALL_ALLOCATE:
  {
  	current_thread->eax = allocate(current_thread->edi);
  	break;
  }
  case SYSCALL_FREE:
  {
  	current_thread->eax = deallocate(current_thread->edi);
  	break;
  }
  case SYSCALL_ALLOCATEPORT:
  {
  	current_thread->eax = allocate_port(current_thread->edi, current_thread->process);
  	break;
  }
  case SYSCALL_FINDPORT:
  {
  	current_thread->eax = find_port(current_thread->edi, &processes[current_thread->esi]);
  	break;
  }
  case SYSCALL_SEND:
  {
  	current_thread->eax = send(current_thread->edi, (struct message*)current_thread->esi);
  	break;
  }
  case SYSCALL_RECEIVE:
  {
  	current_thread->eax = receive(current_thread->edi, (struct message*) current_thread->esi, &(current_thread->edi));
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
  int i, process_id, thread_id, port_id;
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
  threads[thread_id].suspended = 0;
  
  //We need to allocate a port, but that also requires changing its ownership
  port_id = allocate_port(0, &processes[process_id]);

  if(port_id == ERROR){
  	return ERROR;
  }
  //Return
  return ALL_OK;
}
void terminate (void){
  int i;
  //Remove current thread
  current_thread->exists = 0;
  current_thread->process->number_of_threads--;
  //If the process has no threads: free all semaphores, memory pages, ports
  if(current_thread->process->number_of_threads == 0){
    current_thread->process->exists = 0;

  	for(i=0;i<MAX_NUMBER_OF_FRAMES;i++){
  		if(page_frame_table[i].owner == current_thread->process){
  			deallocate(i*4096); //Keep it safe
  		}
  	}
  	
  	for(i=0;i<MAX_THREADS;i++){
  		if(ports[i].process == current_thread->process){
  			ports[i].exists = 0;
  			while(ports[i].thread_list != (struct thread*)-1){
  				ports[i].thread_list->suspended = 0;
  				ports[i].thread_list->eax = ERROR;
  				ports[i].thread_list = ports[i].thread_list->next;
  			}
  		}
  	}
  }
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
    if(threads[i].exists && !threads[i].suspended){
      current_thread->eax = ALL_OK;
      current_thread = &(threads[i]);
      return;
    }
  }
  //If not the next, then the first in the array (or the same one, if no other)
  for(i=0;i<current_id+1;i++){
    if(threads[i].exists && !threads[i].suspended){
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
  threads[thread_id].suspended = 0;
  threads[thread_id].eip = (uint32_t)instructions;
  threads[thread_id].esp = (uint32_t)stack;
  
  //We need to increment the thread count in a process
  current_thread->process->number_of_threads++;
  //Return
  return ALL_OK;
}
/* Add more definitions here if you need. */
int allocate(uint32_t bytes)
{
	int i, counter;
	int pages = ((bytes+4095)>>12);
	i = 0;
	
	while(i <= MAX_NUMBER_OF_FRAMES - pages){
		//If we find an empty frame - check if we have enough consecutive frames
		if(page_frame_table[i].owner == 0 
			&& page_frame_table[i].free_is_allowed == 1){
			for(counter=0;counter<pages;counter++){
				//If frame owned by someone - it's allocated. Break.
				if(page_frame_table[i+counter].owner != 0 
					|| page_frame_table[i+counter].free_is_allowed != 1){
					break;
				}
			}
			//If we found enough pages - break the while loop
			if(counter == pages){
				break;
			} 
			//Otherwise skip already-met frames and continue
			else {
				i += (counter + 1);
			}
		}
		//If we find an allocated frame - continue
		else{
			i++;
		}
	}
	//If we didn't meet enough consecutive frames - return an error
	if(i + pages > MAX_NUMBER_OF_FRAMES){
		return ERROR;
	}
	//Otherwise: allocate and return the address
	for(counter=0;counter<pages;counter++){
		page_frame_table[i+counter].owner = current_thread->process;
		page_frame_table[i+counter].start = i;
		//Note we don't set if we can deallocate - that was done by the kernel
	}
	return i*4096;
}

int deallocate(uint32_t address)
{
	int original_index;
	int index = address / 4096;
	original_index = index;
	
	//Test if first frame (which usually means the whole block) is correct
	//(belongs to the process and can be freed and starts its own block)
	if(page_frame_table[index].owner != current_thread->process
		|| page_frame_table[index].free_is_allowed != 1
		|| page_frame_table[index].start != original_index){
		return ERROR;
	}
	//Look for all frames that match first index (belong to same block),
	//can be freed and belong to calling process and deallocate them
	while(page_frame_table[index].start == original_index
		&& index < MAX_NUMBER_OF_FRAMES
		&& page_frame_table[index].owner == current_thread->process)
	{
		//Check if something really bad happened
		if(page_frame_table[index].free_is_allowed != 1){
			return ERROR;
		}
		//Otherwise set the page as free and move on to the next page
		page_frame_table[index].owner = (struct process*) 0;
		index++;
	}
	return ALL_OK;		
}

int allocate_port(uint32_t id, struct process* process)
{
	int i;
	if(find_port(id, process) != ERROR)
	{
		//Port already exists, error
		return ERROR;
	}
	//Find place for a port
	for(i=0;i<MAX_THREADS;i++){
		if(!ports[i].exists){
			ports[i].id = id;
			ports[i].process = process;
			ports[i].exists = 1;
			
			return i;
		}
	}
	
	//No place for a port - return error
	return ERROR;
}

int find_port(uint32_t id, struct process* owner)
{
	int i;
	for(i=0;i<MAX_THREADS;i++){
		//If an existing port with proper owner and ID encountered: return its index
		if(ports[i].process == owner && ports[i].exists && ports[i].id == id){
			return i;
		}
	}
	
	//If no such port found - return error
	return ERROR;
}

int send(uint32_t port, struct message* to_send)
{
	struct thread* current;
	struct message* message_pointer;
	int i;
	//If there are no threads waiting for send/receive
	if(ports[port].thread_list == (struct thread*) -1){
		//Put this thread at the beginning of the list
		ports[port].thread_list = current_thread;
		//Set the list as a list of sending threads
		ports[port].is_receive = 0;
		//Put the message at the start of the list
		ports[port].message_list = to_send;
		//Suspend current thread
		current_thread->suspended = 1;
		//Make sure it doesn't point to any other thread (same with the message)
		current_thread->next = (struct thread*)-1;
		to_send->next = (struct message*)-1;
		//Yield - this thread is suspended
		yield();
	} 
	//If there are other threads waiting to send
	else if(ports[port].is_receive == 0){
			//Suspend this thread
			current_thread->suspended = 1;
			//Make sure it doesn't point to any other thread
			current_thread->next = (struct thread*)-1;
			//Put it at the end of the queue
			current = ports[port].thread_list;
			while(current->next != (struct thread*)-1){
				current = current->next;
			}
			current->next = current_thread;
			
			//Make sure the message doesn't point anywhere else
			to_send->next = (struct message*)-1;
			//Put it at the end of the list
			message_pointer = ports[port].message_list;
			while(message_pointer->next != (struct message*)-1){
				current = current->next;
			}
			message_pointer->next = to_send;
			//Yield - this thread is suspended
			yield();
		} 
	//If there are threads waiting to receive
	else{
			//Send the message to the first thread on the list
			ports[port].message_list->int_0 = to_send->int_0;
			ports[port].message_list->int_1 = to_send->int_1;
			ports[port].message_list->int_2 = to_send->int_2;
			ports[port].message_list->int_3 = to_send->int_3;
			ports[port].message_list->int_4 = to_send->int_4;
			ports[port].message_list->int_5 = to_send->int_5;
			ports[port].message_list->int_6 = to_send->int_6;
			ports[port].message_list->int_7 = to_send->int_7;			
			
			//Inform the receiver of the ID that the sending process has
			for(i=0;i<MAX_PROCESSES;i++){
				if(&(processes[i]) == current_thread->process){
					*(ports[port].message_list->sender) = i;
				}
			}
			
			//Keep track of the thread to unsuspend
			current = ports[port].thread_list;
			//Unsuspend receiving thread
			ports[port].thread_list->suspended = 0;
			//Remove it from the list
			ports[port].thread_list = ports[port].thread_list->next;
			//Make sure unsuspended thread doesn't point to any other thread
			current->next = (struct thread*)-1;
			
			//Keep track of the message that was sent
			message_pointer = ports[port].message_list;
			//Remove the message from the list
			ports[port].message_list = message_pointer->next;
			//Make sure the message doesn't point to any other message
			message_pointer->next = (struct message*) -1;
			//Yield - let the receiver handle the message first
			//yield();
			}
		//Return to calling thread that the message was sent - it will receive it only
		//after the message was received and this thread was unsuspended
		return ALL_OK;
}
	
int receive(uint32_t port, struct message* buffer, uint32_t* sender){
	struct thread* current;
	struct message* message_pointer;
	int i;
	
	//Check if this thread can receive from this port
	if(ports[port].process != current_thread->process){
		return ERROR; //If not - return ERROR
	}
	//If there are no threads waiting for send/receive
	if(ports[port].thread_list == (struct thread*) -1){
		//Suspend the thread
		current_thread->suspended = 1;
		//Make sure the thread doesn't point to any other thread
		current_thread->next = (struct thread*)-1;
		//Put current thread on the list
		ports[port].thread_list = current_thread;
		//Set the list as containing threads waiting to receive
		ports[port].is_receive = 1;
		//Set the pointer to an integer that will hold sender's process ID
		buffer->sender = sender;
		//Make sure the message doesn't point to any other message
		buffer->next = (struct message*)-1;
		//Put the message on the list
		ports[port].message_list = buffer;
		//Yield - now we wait for the sender
		yield();
	} 
	//If there are other threads waiting to receive
	else if(ports[port].is_receive == 1){
			//Suspend this thread
			current_thread->suspended = 1;
			//Make sure the thread doesn't point to any other thread
			current_thread->next = (struct thread*)-1;
			//Put it at the end of the list
			current = ports[port].thread_list;
			while(current->next != (struct thread*)-1){
				current = current->next;
			}
			current->next = current_thread;
			
			//Make sure the message doesn't point to any other thread
			buffer->next = (struct message*)-1;
			//Set the pointer to an integer that will hold sender's process ID
			buffer->sender = sender;
			//Put the message at the end of the list
			message_pointer = ports[port].message_list;
			while(message_pointer->next != (struct message*)-1){
				current = current->next;
			}
			message_pointer->next = buffer;
			
			//Yield - now we wait for the sender
			yield();
		} 
	//If there are threads waiting to send
	else{
			//Get the message from the sender
			buffer->int_0 = ports[port].message_list->int_0;
			buffer->int_1 = ports[port].message_list->int_1;
			buffer->int_2 = ports[port].message_list->int_2;
			buffer->int_3 = ports[port].message_list->int_3;
			buffer->int_4 = ports[port].message_list->int_4;
			buffer->int_5 = ports[port].message_list->int_5;
			buffer->int_6 = ports[port].message_list->int_6;
			buffer->int_7 = ports[port].message_list->int_7;			
			
			//Unsuspend the sender
			ports[port].thread_list->suspended = 0;
			//Keep track of the thread to unsuspend
			current = ports[port].thread_list;
			//Get the ID of sending thread's process
			for(i=0;i<MAX_PROCESSES;i++){
				if(&(processes[i]) == current->process){
					*sender = i;
				}
			}			
			//Remove unsuspended thread from the list
			ports[port].thread_list = ports[port].thread_list->next;
			//Make sure unsuspended thread doesn't point to any other thread
			current->next = (struct thread*)-1;
			//Keep track of the message handled
			message_pointer = ports[port].message_list;
			//Remove the message from the list
			ports[port].message_list = message_pointer->next;
			//Make sure the message doesn't point to any other message
			message_pointer->next = (struct message*) -1;
			
			//Yield - let the sender work first
			//yield();
	}
	//Return to the calling thread that the message was received - it will get
	//the response after the message was sent and this thread was unsuspended
	return ALL_OK;
}