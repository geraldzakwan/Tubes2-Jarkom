/*
* File : Frame.h
  Author : Geraldi Dzakwan 			13514065
  		   Ramos Janoah Hasudungan 	13514089
  		   Alvin Junianto Lan 		13514105
*/

#ifndef _FRAME_H_
#define _FRAME_H_  		   

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

/* ASCII CONST */
#define SOH 1
#define STX 2
#define ETX 3
#define EOT 4
#define ACK 6
#define NAK 15

#define maxAckLen 25
#define intSize 32

class Acknowledgement {
private:
	char ackOrNak;
	int frameNumber;
	int checksum;

public:
	Acknowledgement();

	char getAckOrNak();
	int getFrameNumber();
	int getChecksum();

	void setFrameNumber(int);
	void setAckOrNak(char);
	void setChecksum(int);

	string createAckString();
	void extractAckString(char*);
};

#endif