#include "acknowledgement.h"	

Acknowledgement::Acknowledgement() {
	ackOrNak = NAK;
	frameNumber = 0;
}

char Acknowledgement::getAckOrNak() { return ackOrNak; }

int Acknowledgement::getFrameNumber() { return frameNumber; } 

int Acknowledgement::getChecksum () { return checksum; }

void Acknowledgement::setFrameNumber(int k) { frameNumber = k; }

void Acknowledgement::setAckOrNak(char ch) { ackOrNak = ch; }

void Acknowledgement::setChecksum(int ch) { checksum = ch; }

string Acknowledgement::createAckString() {
	int intlen = to_string(getFrameNumber()).length();
	char ch[intlen+3]; 
	char frameNum[intlen];
	ch[0] = ackOrNak;

	sprintf(frameNum, "%d", getFrameNumber());	
	for (int i = 0; i < intlen; i++) {
		ch[i+1] = frameNum[i];
	}

	intlen++; ch[intlen] = ';';
	intlen++; ch[intlen] = '\0';

	string str(ch);
	return str;	
}

void Acknowledgement::extractAckString(char* frame) {
	ackOrNak = frame[0];
	char frameNum[intSize];
	
	int i = 1;
	while (frame[i] != ';') {
		frameNum[i-1] = frame[i];
		i++;
	}

	frameNum[i-1] = '\0';
	frameNumber = atoi(frameNum);
}