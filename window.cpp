#include "window.h"

int Frame::getFrameNumber() { return frameNumber; }

string Frame::getMessage() { return msg; }

int Frame::getChecksum() { return checksum; }

void Frame::setFrameNumber(int k) { frameNumber = k; }

void Frame::setMessage(char* ch) { 
	string temp(ch);
	msg = temp;
}

void Frame::setChecksum(char* ch) { checksum = generateChecksum(ch); }

string Frame::createFrameString() {
	int intl = to_string(getFrameNumber()).length();
	char frame[intl + msgLen + 5]; 
	char frameNum[intl];
	char ch[intLen];

	frame[0] = SOH;

	sprintf(frameNum, "%d", getFrameNumber());	

	for (int i=0; i<intl; i++) {
		frame[i+1] = frameNum[i];
	}

	frame[intl+1] = STX;

	for (int i=0; i<msgLen; i++) {
		frame[intl+i+2] = (getMessage()).at(i);	
	} 

	frame[intl+msgLen+2] = ETX;

	checksum = generateChecksum(frame);
	int len = to_string(getChecksum()).length();
	sprintf(ch, "%d", getChecksum());
	
	for (int i=0; i<len; i++) {
		frame[intl+msgLen+3+i] = ch[i];
	}

	frame[intl+len+msgLen+3] = '\0';

	string str(frame);
	return str;
}

bool Frame::extractFrameString (char* frame) {
	// if (frame[0]!= SOH) {
	// 	frame[0] = SOH;
	// }
	// cout << "FRAME : " << frame << endl;
	// cout << "STRLEN : " << strlen(frame) << endl;
	// int so = frame[0];
	// cout << "SOH : " << so << endl;
	if (frame[0]==SOH) {
		int i = 1;
		char num[intLen];
		char ch1[intLen];
		char ch2[msgLen];
		while (frame[i]!=STX) {
			num[i-1] = frame[i];
			i++;
		}

		num[i-1] = '\0';
		i++;
		frameNumber = atoi(num);
		
		int k = 0;
		while (frame[i]!=ETX) {
			ch2[k] = frame[i];
			i++;
			k++;
		}
		ch2[k] = '\0';
		i++;

		string temp(ch2);
		msg = temp;

		k = 0;
		while (frame[i] != '\0')  {
			ch1[k] = frame[i];
			i++;
			k++;
		}
		ch1[k] = '\0';

		checksum = atoi(ch1);
		return true;
	} else {
		// cout << "Missing SOH" << endl;
		// cout << strlen(frame) << endl;
		// checksum = 1;
		// for (int i=0; i<strlen(frame); i++) {
		// 	int f = frame[i];
		// 	cout << "Frame ke-[" << i << "] : " << f << endl;
		// }
		return false;
	}
}

int Frame::generateChecksum(char* ch) {
	unsigned long long chksum= 0;
	
	unsigned long long sd= 0x131;
	
	int i = 0;
	do {
		// cout << i << endl;
		chksum=chksum <<8;
		chksum+=(unsigned long long) ch[i];
		i++;
	} while(ch[i]!=ETX);
	chksum = chksum <<8;
	
	for(int i = sizeof(long long)*8; i>8; i--){
		if((chksum>>(i-1))&1){
			chksum = chksum ^ (sd<<(i-9));
		}
	}
	return (int)chksum;
}

Window::Window() {
	Frames = new Frame [maxNumberOfFrame];
	ackStatus = new int [maxNumberOfFrame];
	totalFrames = 0;
	start = 0;
	pointer = 0;
	length = 0;
	size = winSize;
}

Window::~Window() {
	delete [] Frames;
	delete [] ackStatus;
}

void Window::insertFrame(Frame F, int i) {
	Frames[i] = F;
	ackStatus[i] = 0;
	++length;
}

Frame Window::getCurrentFrame() {
	return (Frames[pointer]);
}

Frame Window::getFrame(int FNum) {
	return (Frames[FNum]);
}

int Window::getLength() {
	return length;
}

int Window::getACK(int i) {
	return ackStatus[i];
}

void Window::setACK(int i) {
	ackStatus[i] = 1;
}

int Window::getPointer() {
	return pointer;	
}

void Window::nextSlot() {
	++pointer;

	if ( (pointer < start) || (pointer >= start + size) || (pointer >= length) ) {
		pointer = start;
	}
}

void Window::slideWindow() {
	while ((ackStatus[start]==1) && ((start+size)<(length))) {
		start++;
		cout << "Slide window to : " << start << endl;
	}
}

void Window::iterateFrames() {
	for ( int i = 0; i <= length; i++ ) {
		cout << (Frames[i]).getMessage();
	}
	cout << endl;
}

int Window::isEnd() {
	// return ( pointer == length-1 ); 
	return ( pointer == length); 
}

int Window::isAllACK() {
	int ret = 1;

	for (int i = 0; i < length; i++) {
		if (ackStatus[i] != 1) {
			ret = 0;
		}
	}

	return ( ret ); 
}