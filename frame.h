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

#define msgLen 5
#define maxMsgLen 25

class Frame {
	private:
		int checksum;
		int length;
		string msg;
		int number;
		
	public:
		// Frame();
		// Frame(const Frame &F);
		// ~Frame();

		int getChecksum ();
		string getCompiled();
		string getCompiledWithoutChecksum ();
		void getDecompiled (char*);
		int getLength ();
		string getMessage ();
		int getNumber ();

		void setChecksum (char*);
		void setLength (int l);
		void setMessage (char*);		
		void setNumber (int);

		int GenerateChecksum(char*);
		int GenerateChecksumCRC(char*);
		void printbit(long long);	
		int intLen(int);
};

#endif