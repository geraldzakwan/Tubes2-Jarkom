/*
* File : Queue.h
  Author : Geraldi Dzakwan 			13514065
  		   Ramos Janoah Hasudungan 	13514089
  		   Alvin Junianto Lan 		13514105
*/

#ifndef _QUEUE_H_
#define _QUEUE_H_

/* Library */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <netdb.h>
#include <pthread.h>
#include <iostream>
using namespace std;

#define maxLen 50
#define MINUPPERLIMIT 5
#define MAXLOWERLIMIT 2
#define RXQSIZE 8
#define CONSUMESPEED 2000
#define SENDSPEED 500 
/* XON/XOFF protocol */
#define XON (0x11)
#define XOFF (0x13)

typedef unsigned char Byte;

class Queue {
	public:
		int count;
		int front;
		int rear;
		int maxsize;
		string *msg;

		Queue(int=RXQSIZE);
		void addToQueue(char*);
		char* delFromQueue();
		int emptySlot();
};

#endif