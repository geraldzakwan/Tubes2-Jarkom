#include "frame.h"

void Frame::setNumber (int i) {
	number = i;	
}

void Frame::setMessage (char* ch) {
	string str(ch);
	msg = ch;
}

void Frame::setChecksum (char* ch) {
	checksum = GenerateChecksumCRC(ch);
}

int Frame::GenerateChecksum(char* checkString) {
	cout << "BIASA" << endl;
	int bitSum = 0, x = 0, r = 0;
	int bitTest, bitHeader, bitBody, checkSum;

	for ( int i = 0; i < strlen(checkString); i++ ) {
		bitSum += (int) checkString[i];
	}

	bitTest = bitSum;
	while ( bitTest != 0 ) {
		bitTest = bitTest >> 1;
		++r;
		if ( r % 4 == 1 ) {
			++x;
		}
	}

	x *= 4;

	bitHeader = ( bitSum >> (x - 4) ) << (x - 4);
	bitBody = bitSum - bitHeader;

	checkSum = ~(bitBody + (bitSum >> (x - 4))) & 0x000000ff;

	return checkSum;
}
void Frame::printbit(long long a){
int i;
	for(i= sizeof(long long)*8; i>0; i--){
		printf("%d", (int)((a>>(i-1))&1));
	}
	printf("\n");
}
int Frame::GenerateChecksumCRC(char* checkString) {
	cout << "CRC" << endl;
	unsigned long long dummy= 0;
	char* temp=checkString;
	//int bitSum = 0, x = 0, r = 0;
	//int bitTest, bitHeader, bitBody, checkSum;
	
	unsigned long long seed= 0x131;
	//temp[strlen(temp)-1]=0;
	int i=0;
	do{
		dummy = dummy <<8;
		dummy +=  (unsigned long long)  temp[i];
		i++;
	}while(temp[i]!=ETX);
	dummy = dummy <<8;
	/*for ( int i = 0; i < strlen(temp); i++ ) {
		dummy =dummy << 8;
		dummy +=  (unsigned long long)  temp[i];
	}*/
	//printbit( dummy);
	//cout<<"\n\n\n";
	for(int i = sizeof(long long)*8; i>8; i--){
		if((dummy>>(i-1))&1){
			dummy = dummy ^ (seed<<(i-9));
		}
	}
	return (int) dummy;
	
}

void Frame::setLength (int l) {
	length = l;	
}

int Frame::getNumber () {
	return number;
}

string Frame::getMessage () {
	return msg;
}

int Frame::getLength () {
	return length;
}

int Frame::getChecksum () {
	return checksum;
}

string Frame::getCompiled () {
	int n = intLen(getNumber());
	int m;
	int size = 1 + n + 1 + msgLen + 1 + 1 + 1;
	char ret[size]; 
	char num[n];
	char chk[m];

	ret[0] = SOH;

	sprintf(num, "%d", getNumber());	
	

	for (int i = 0; i < n; i++) {
		ret[i+1] = num[i];
	}

	ret[1 + n] = STX;

	for (int i = 0; i < msgLen; i++) {
		ret[1 + n + i + 1] = (getMessage()).at(i);	
	} 

	ret[1 + n + msgLen + 1] = ETX;

	this->checksum = GenerateChecksumCRC(ret);
	m = intLen(getChecksum());
	sprintf(chk, "%d", getChecksum());
	
	for (int i = 0; i < m; i++) {
		ret[1 + n + msgLen + 2 + i] = chk[i];
	}

	ret[1 + n + msgLen + m + 2] = '\0';

	string str(ret);
	return str;
}


void Frame::getDecompiled (char* frame) {
	if ( frame[0] == SOH ) {

		int i = 1;
		char num[33];
		char chk[33];
		char msg[msgLen];

		while ( frame[i] != STX ) {
			num[i-1] = frame[i];
			++i;
		}

		num[i-1] = '\0';
		++i;
		number = atoi(num);
		int n = 0;

		while ( frame[i] != ETX ) {
			msg[n] = frame[i];
			++i; ++n;
		}
		msg[n] = '\0';
		++i;

		string temp(msg);
		msg = temp;

		n = 0;
		while ( frame[i] != '\0' ) {
			chk[n] = frame[i];
			++i; ++n;			
		}
		chk[n] = '\0';

		checksum = atoi(chk);

	} else {
		printf("Error - Frame Corrupted");
	}
}

int Frame::intLen(int num) {
	int len = 0;

	do {
		len++;
		num /= 10;
	} while (num > 0);

	return len;
}