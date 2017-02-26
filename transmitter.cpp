/*
* File 		: transmitter.cpp
* Author 	: Geraldi Dzakwan 13514065
* 			  Ramos Janoah H 13514089
* 			  Alvin Junianto L 13514105
*/

#include "acknowledgement.h"
#include "queue.h"
#include "window.h"

/* Global variable untuk socket programming*/
int socketFD; 						/* socket file descriptor */
struct sockaddr_in receiverAddr;	/* receiver addreceivedAcks */
unsigned int addrLen = sizeof(receiverAddr); /* length of addreceivedAcks */

/* Global variable untuk buffer */
char sentBuffer[maxMsgLen]; /* buffer yg dikirim ke receiver*/
char receivedAck[maxAckLen];

/* Global variable untuk signal handling (XON/XOFF) */
int sigVal; 			/* 0 = XOFF, 1 = XON */

/* Global variable untuk mengontrol thread */
int parentThreadDone;	/* 0 = masih berjalan, 1 = selesai */

/* Global variable untuk file teks dan char yg akan dibaca*/
char* filename;
int ch;
int ackornak = 0;

/* Frame and Window */
Window window;
Frame *frame[maxNumberOfFrame];
int frameCount = 0;

/* Functions declaration */
void* sendBuffer(void *arg);
void* receiveSignal(void *arg);
void initializeFrames();
void initializeWindow();
void printError(const char* errmsg);

int main(int argc, char *argv[]) {
	/* Cek apakah jumlah argumen sesuai spesifikasi */
	if (argc < 4) {
		printError("Argumen yang diperlukan : ip tujuan, port, dan nama file\n");
		return 0;
	}

	/* Membuat socket */
	if ((socketFD = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printError("Gagal membuat socket");
		return 0;
	}

	char* ip = argv[1]; 		/* ip tujuan */
	int port = atoi(argv[2]); 	/* nomor port */
	filename = argv[3]; 		/* nama file teks */

	memset((char *) &receiverAddr, 0, sizeof(receiverAddr)); 	/* zeroing memory untuk receiver addreceivedAcks */
	receiverAddr.sin_family = AF_INET; 							/* family dari receiver addreceivedAcks */
	receiverAddr.sin_port = htons(port);						/* port yang di bind receiver */

	/* Untuk mengecek apakah host valid */
	struct hostent *host; 
	if ((host = gethostbyname(ip)) == NULL) {
		printError("Receiver addreceivedAcks tidak ditemukan");
	}

	/* Copy host addreceivedAcks ke receiver addreceivedAcks */
	bcopy((char *)host->h_addr, (char *)&receiverAddr.sin_addr.s_addr, host->h_length);

	/* Pembuatan socket berhasil */
	printf("Membuat socket untuk koneksi ke %s:%d ...\n", ip, port);

	/* Inisialisasi parent and child thread */
	pthread_t threadList[2];/* Array of thread */
	sigVal = 1; 			/* set flag menjadi XON */
	parentThreadDone = 0; 	/* menyatakan bahwa thread baru mulai berjalan */
	initializeWindow();

	/* Membuat parent thread */
	if (pthread_create(&(threadList[0]), NULL, &sendBuffer, NULL) != 0) {
		printError("Gagal membuat parent thread untuk sendBuffer");
	}	

	/* Membuat childe thread */
	if (pthread_create(&(threadList[0]), NULL, &receiveSignal, NULL) != 0) {
		printError("Gagal membuat child thread untuk receiveSignal");
	}	

	/* Join thread, program berakhir */
	pthread_join( threadList[0], NULL);
	pthread_join( threadList[1], NULL);

	/* Menutup socket */
	close(socketFD); 

	/* Selesai */
	return 0;

}

/* Fungsi untuk menampilkan pesan error dan keluar program */
void printError(const char* errmsg) {
	printf("%s\n", errmsg);
	exit(1);
}	

void initializeFrames() {
	FILE *fp;
	fp = fopen(filename, "r");
	int c, counter = 0, idx = 0;
	char temp[(msgLen+1)];

	while ( ((c = fgetc(fp)) != EOF)) {	
		temp[counter] = c;
		counter++;
		if ( counter == msgLen) {
			temp[counter] = '\0';
			frame[idx] = new Frame();
			frame[idx]->setMessage(temp); 
			frame[idx]->setFrameNumber(idx); 
			counter = 0;
			idx++;
		}
	}

	if (counter != 0) {
		for ( int i = counter; i < msgLen; i++ ) {
			temp[i] = ' ';
		}
		frame[idx] = new Frame();
		frame[idx]->setMessage(temp); 
		frame[idx]->setFrameNumber(idx); 
		idx++;	
	}

	// End Of Frame
	temp[0] = 26;

	for ( int i = 1; i < msgLen; i++ ) {
		temp[i] = ' ';
	}

	frame[idx] = new Frame();
	frame[idx]->setMessage(temp); 
	frame[idx]->setFrameNumber(idx);
	idx++;	 

	frameCount = idx;
}

void initializeWindow() {
	initializeFrames();
	for (int i = 0; i < frameCount; i++) {
		window.insertFrame(*frame[i], i);
	}
}

void* sendBuffer(void *arg) {
	/* Parent Thread */
	int c, i = 0, counter = 0;
	bool ez = true;;

	cout << "Window length : " << window.getLength() << endl;
	while ( /*( !window.isEnd() || !window.isAllACK() )*/ !window.isAllACK() && ez) {
		while (sigVal == 0) {
			/* Jika flag adalah XOFF, maka tunggu hingga XON */
			printf("Waiting for XON...\n");
			usleep(SENDSPEED * 1000 * 2);
		}

		ackornak = 0;
		
		if ( window.getACK(window.getPointer()) != 1 ) {
			string temp = ((window.getCurrentFrame()).createFrameString());

			sprintf(sentBuffer, "%s", temp.c_str());
			if (sendto(socketFD, sentBuffer, maxMsgLen, 0, (struct sockaddr *)&receiverAddr, addrLen)==-1) {
				printError("SEND ERROR");
			} else {			
				printf("Sending frame number %d \n", window.getPointer());	
			}
		}

		window.slideWindow();
		window.nextSlot();
		// usleep((SENDSPEED * 1000)/winSize);
		// usleep((SENDSPEED * 1000)*2.5);
		usleep((SENDSPEED * 1000));
		// cout << "ackornak : " << ackornak << endl;
		// cout << "Window pointer : " << window.getPointer() << endl;
		if (ackornak==0) {
			// cout << "Timeout, no acknowledgement received" << endl;
		}
		// if (window.getPointer()==window.getLength()-1) {
		// 	ez = false;
		// }
		if (window.isAllACK()) {
			cout << "Masuk" << endl;
			parentThreadDone = 1;
		}
	}

	cout << "Keluar" << endl;
	parentThreadDone = 1;

	return NULL;
}

void* receiveSignal(void *arg) {
	/* Child Thread */

	while (parentThreadDone == 0 && !window.isAllACK()) {
		/* Listening XON XOFF signal */ 
		
		if ( recvfrom(socketFD, receivedAck, maxAckLen, 0, (struct sockaddr *)&receiverAddr, &addrLen) == -1 )
			perror("recvfrom");
		
		if ( receivedAck[0] == XON ) {
			/*Jika XON, ganti flag sigVal menjadi 1*/
			printf("XON diterima.\n"); 
			sigVal = 1;
		} else if ( receivedAck[0] == XOFF ) {
			/*Jika XOFF, ganti flag sigVal menjadi 0*/
			printf("XOFF diterima.\n"); 
			sigVal = 0;
		} else {
			Acknowledgement R;
			R.extractAckString(receivedAck);
			if ( R.getAckOrNak() == ACK ) {
				ackornak = ACK;
				window.setACK(R.getFrameNumber());
				printf("ACK received from frame number %d\n", R.getFrameNumber());
			} else {
				ackornak = NAK;
				printf("NAK received from frame number %d\n", R.getFrameNumber());	

				string temp = ((window.getFrame(R.getFrameNumber())).createFrameString());

				// sprintf(sentBuffer, "%s", temp.c_str());
				// if (sendto(socketFD, sentBuffer, maxMsgLen, 0, (struct sockaddr *)&receiverAddr, addrLen)==-1) {
				// 	printError("SEND ERROR");
				// } else {			
				// 	printf("Sending again frame number %d\n", R.getFrameNumber());
				// }	
			}
		}
	}

	cout << "Keluar" << endl;
	return NULL;
}

