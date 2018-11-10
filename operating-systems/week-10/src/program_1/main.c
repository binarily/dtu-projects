#include <scwrapper.h>

int
main(int argc, char* argv[])
{
 int32_t send_port, recv_port;

 recv_port=findport(0,1);
 if (recv_port < 0)
 {
  prints("program 1: findport for process 1 failed\n");
  return 0;
 }

 send_port=findport(0,0);
 if (send_port < 0)
 {
  prints("program 1: findport for process 0 failed\n");
  return 0;
 }

 while(1)
 {
  struct message msg;
  int32_t        sender;

  msg.int_0=0;
  msg.int_1=1;
  msg.int_2=2;
  msg.int_3=3;
  msg.int_4=4;
  msg.int_5=5;
  msg.int_6=6;
  msg.int_7=7;

  prints("program 1: sending ping\n");

  if (ALL_OK != send(send_port, &msg))
  {
   prints("program 1: send failed\n");
   return 0;
  }

  if (ALL_OK != receive(recv_port, &msg, &sender))
  {
   prints("program 1: receive failed\n");
   return 0;
  }

  /* Test message. */
  if ((7 != msg.int_0) ||
      (6 != msg.int_1) ||
      (5 != msg.int_2) ||
      (4 != msg.int_3) ||
      (3 != msg.int_4) ||
      (2 != msg.int_5) ||
      (1 != msg.int_6) ||
      (0 != msg.int_7))
  {
   prints("program 1: message test failed\n");
   return 0;
  }

  prints("program 1: received pong\n");
 }

 return 0;
}
