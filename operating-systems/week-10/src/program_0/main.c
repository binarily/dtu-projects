/*! \file
 *      \brief The first user program - tests the message passing system calls.
 *
 */
#include <scwrapper.h>

int
main(int argc, char* argv[])
{
 int32_t recv_port;

 if (ALL_OK != createprocess(1))
 {
  prints("program 0: createprocess(1) failed\n");
  return 0;
 }

 if (ALL_OK != createprocess(2))
 {
  prints("program 0: createprocess(2) failed\n");
  return 0;
 }

 recv_port=findport(0,0);
 if (recv_port < 0)
 {
  prints("program 0: findport for process 0 failed\n");
  return 0;
 }

 while(1)
 {
  struct message msg;
  int32_t        sender;
  int32_t        send_port;

  if (ALL_OK != receive(recv_port, &msg, &sender))
  {
   prints("program 0: receive failed\n");
   return 0;
  }

  prints("program 0: received ping\n");

  /* Test message. */
  if ((0 != msg.int_0) ||
      (1 != msg.int_1) ||
      (2 != msg.int_2) ||
      (3 != msg.int_3) ||
      (4 != msg.int_4) ||
      (5 != msg.int_5) ||
      (6 != msg.int_6) ||
      (7 != msg.int_7))
  {
   prints("program 0: message test failed\n");
   printhex(msg.int_3);
   return 0;
  }

  send_port=findport(0, sender);
  if (send_port < 0)
  {
   prints("program 0: findport for process failed\n");
   return 0;
  }


  msg.int_0=7;
  msg.int_1=6;
  msg.int_2=5;
  msg.int_3=4;
  msg.int_4=3;
  msg.int_5=2;
  msg.int_6=1;
  msg.int_7=0;

  if (ALL_OK != send(send_port, &msg))
  {
   prints("program 0: send failed\n");
   return 0;
  }
  prints("program 0: sent pong\n");
 }

 return 0;
}
