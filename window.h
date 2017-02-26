/*
* File : Window.h
  Author : Geraldi Dzakwan 			13514065
  		   Ramos Janoah Hasudungan 	13514089
  		   Alvin Junianto Lan 		13514105
*/

#ifndef _WINDOW_H_
#define _WINDOW_H_  		   

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

#define winSize 4
#define maxNumberOfFrame 10000

/* ASCII CONST */
#define SOH 1
#define STX 2
#define ETX 3
#define EOT 4
#define ACK 6
#define NAK 15

#define msgLen 5
#define maxMsgLen 25
#define intLen 32

class Frame {
	private:
		int frameNumber;
		string msg;
		int checksum;
		
	public:
		int getFrameNumber();
		string getMessage();
		int getChecksum();

		void setFrameNumber(int);
		void setChecksum(char*);
		void setMessage(char*);		

		string createFrameString();
		bool extractFrameString(char*);

		int generateChecksum(char*);
};

class Window {
	private:
		Frame *Frames;
		int *ackStatus;
		int start;
		int size;
		int pointer;
		int length;

	public:
		Window();
		~Window();
		void insertFrame(Frame F, int i);
		Frame getCurrentFrame();
		Frame getFrame(int FNum);
		int getLength();
		int getPointer();
		int getACK(int i);
		void slideWindow();
		void nextSlot();
		void setACK(int i);
		void iterateFrames();
		int isEnd();
		int isAllACK();
		int totalFrames;
};

#endif