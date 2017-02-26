/*
* File : Queue.cpp
  Author : Geraldi Dzakwan 			13514065
  		   Ramos Janoah Hasudungan 	13514089
  		   Alvin Junianto Lan 		13514105
*/

#include "queue.h"

Queue::Queue(int size) {
	count = 0;
	front = 0;
	rear = -1;
	maxsize = size;
	msg = new string [maxsize];
}

void Queue::addToQueue(char* ch){
	if (rear==(maxsize - 1)) {
		rear = 0;
	} else {
		rear++;
	}

	count++;
	string str(ch);
	msg[rear] = str;
}

char* Queue::delFromQueue() {
	char *ch = (char*) malloc(sizeof(char)*maxLen);
	sprintf(ch, "%s", (msg[front]).c_str());
	msg[front] = "";

	if (front== (maxsize - 1)) {
		front = 0;
	} else {
		front++;
	}

	count--;
	return ch;	
}

int Queue::emptySlot() {
	return maxsize - count;
}