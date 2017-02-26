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
struct sockaddr_in receiverAddr;	/* address receiver */
struct sockaddr_in transmitterAddr;	/* address transmitter */
unsigned int addrLen = sizeof(transmitterAddr);	/* length of address */

/* Limits */
int minUpperLimit = 2; /* To Send XOFF */
int maxLowerLimit = 5; /* To Send XON */ 

int parentThreadDone = 0;
int acc = 0;

char* receivedBuffer; 
char ack[maxAckLen];
char* consumedBuffer;

Queue queue(RXQSIZE); /* the buffer */
int sigVal; /* 0 = XOFF, 1 = XON */
int countReceived = 0;
int countConsumed = 0;

Acknowledgement ackResp;
Window window;

/* Functions declaration */
static char* rcvchar(int socketFD, Queue *queue);
static char* q_get(int socketFD, Queue *queue);
void* readByte(void *arg);
void* consumeByte(void *arg);
void printError(const char* errmsg);

int main(int argc, char *argv[]) {
	/* Initializing queue for receiving and sending Frame */
	receivedBuffer = (char *) malloc(maxMsgLen * sizeof(char));
	consumedBuffer = (char *) malloc(maxMsgLen * sizeof(char));

	/* Membentuk socket */
	if ((socketFD = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
 		printError("Gagal membentuk socket");
	}

	/* Mengatur receiver */
	memset((void *)&receiverAddr, 0, sizeof(receiverAddr)); /* zeroing memory untuk receiver address */
	receiverAddr.sin_family = AF_INET;						/* family dari receiver address */			
	int port = atoi(argv[1]);							
	receiverAddr.sin_port = htons(port);					/* port yang akan di bind receiver */
	receiverAddr.sin_addr.s_addr = INADDR_ANY;				
	//receiverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");	/* address = localhost */			

	/* Binding socket pada localhost, 127.0.0.1 */	
	if (bind(socketFD, (struct sockaddr *)&receiverAddr, sizeof(receiverAddr)) < 0) {
		printError("Gagal melakukan binding socket");
		return 0;
	}
	
	/* Binding socket berhasil */
	printf("Binding pada 127.0.0.1:%d\n", port);
	
	/* Mengatur transmitter */
	memset((char *) &transmitterAddr, 0, sizeof(transmitterAddr)); 	/* zeroing memory untuk transmitter address */
	transmitterAddr.sin_family = AF_INET;							/* family dari transmitter address */			
	transmitterAddr.sin_port = htons(port);							/* port yang akan di bind transmitter */
	
	/* Inisialisasi parent and child thread */
	pthread_t threadList[2];			/* Array of thread */
	sigVal = 1;							/* set flag menjadi XON */


	/* Membuat parent thread */
	if (pthread_create(&(threadList[0]), NULL, &readByte, NULL) != 0) {
		printError("Gagal membuat parent thread untuk readByte");
	}	

	/* Membuat childe thread */
	if (pthread_create(&(threadList[0]), NULL, &consumeByte, NULL) != 0) {
		printError("Gagal membuat child thread untuk consumeByte");
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

void* readByte(void *arg) {
	char* chck=NULL;
	
	while ( (parentThreadDone != 1) ) {	

		chck = rcvchar(socketFD, &queue);
	
		if ( chck != NULL ) {
			Frame F;
			bool isSOH = F.extractFrameString(chck);

			if ( (F.getMessage()).at(0) != 26) {
				cout << "Accepting frame " << F.getFrameNumber() << endl;
			} else {
				acc = F.getFrameNumber();
			}		
		} 	
	}

	return NULL;
}

void* consumeByte(void *arg) {
	/* Child Thread */
	char* chck;
	int i = 0;
	int length = 0;
	int start = 0;
	bool canBeConsumed = false;
	Frame Ftemp[100];
	int temp[100];
	int tempLength = 0;

	
	//inisialisasi elemen temp dengan -1
	for (i = 0; i <= 100; i++) {
		temp[i] = -1;
	}
	
	/* Sampai program diakhiri, konsumsi terus byte yang ada pada queue*/
	while (parentThreadDone != 1) {	
		chck = q_get(socketFD, &queue);
		
		if ( chck != NULL ) {
			Frame F;
			bool isSOH = F.extractFrameString(chck);
			// cout << "Ada SOH : " << isSOH << endl;

			// cout << "FN : " << endl;
			// cout << F.getFrameNumber() << endl;

			// cout << "FC : " << endl;
			// cout << F.getChecksum() << endl;

			// Set frame number dan checksum pada Acknowledgement
			int gf = F.getFrameNumber();
			ackResp.setFrameNumber(gf);
			ackResp.setChecksum(F.getChecksum());

			// cout << "sampe0" << endl;			
			// cout << F.generateChecksum(chck) << endl;
			// cout << "sampe0a" << endl;						

			// Checks checksum
			int x = 0;
			// if (gf%winSize!=(winSize/2)) { 
			// if (true) {
			if (rand()%winSize!=(winSize/2)) { 
				x = F.getChecksum();
			}
			if ( F.generateChecksum(chck) != x) {
				cout << "Sending NAK to frame " << F.getFrameNumber() << " due to invalid checksum" << endl;
				ackResp.setAckOrNak(NAK);
				countReceived--;
			} else {	
				// if (rand()%4<2) {
				if (true) {
					cout << "Sending ACK to frame " << F.getFrameNumber() << endl;
					ackResp.setAckOrNak(ACK);	
				} else {
					cout << "Sending NAK to frame " << F.getFrameNumber() << " due to invalid checksum" << endl;
					ackResp.setAckOrNak(NAK);	
					countReceived--;
				}
				// ackResp.setAckOrNak(ACK);	
			}

			// cout << "sampe1" << endl;			

			int num = ackResp.getFrameNumber();
			
			if (tempLength < ( ackResp.getFrameNumber() + 1 )) {
				tempLength = ackResp.getFrameNumber() + 1;
			}

			if (ackResp.getAckOrNak() == ACK)
			{
				if (num < i)
				{
					temp[num] = num;
					Ftemp[num] = F;
				}
				else {
					temp[i] = i;
					Ftemp[i] = F;
					if ( i > 0 ) { length++; }
				}
			}	
			else {
				//NAK
				if (num < i)
					temp[num] = -1;
				else
					temp[i] = -1;
			}

			int j = window.getLength();

			while ( ( temp[j] != -1 ) && ( j < tempLength ) ) {

				window.insertFrame(Ftemp[j], j);
				cout << "Consuming frame number : " << j << " = " << window.getFrame(j).getMessage() << endl;
				
				countConsumed++;
				window.slideWindow();
				j++;
			}
			i++;


			sprintf(ack, "%s", (ackResp.createAckString()).c_str());
			if (sendto(socketFD, ack, maxAckLen, 0, (struct sockaddr *)&transmitterAddr, addrLen) == -1) {
				printError("ACK");
			}

			if ( ( acc != 0 ) && ( (acc + 1) == countConsumed ) ) {
				parentThreadDone = 1;
			}

		}
		usleep((CONSUMESPEED * 1000));
	}

	window.iterateFrames();

	return NULL;
}

static char* rcvchar(int socketFD, Queue *queue) {
	int k = recvfrom(socketFD, receivedBuffer, maxMsgLen, 0, (struct sockaddr *)&transmitterAddr, &addrLen);
	//INI KALO MAU MAIN2 TIMEOUT
	// if(true) {
	// if (rand()%4<2) {
	if ((queue->count) > MINUPPERLIMIT && sigVal==1) {
		/* Membentuk sinyal XOFF */
		char sig[1];
		printf("Buffer > minimum upperlimit.\n");
		sprintf(sig, "%c",(char) XOFF);

		/* Mengirim sinyal XOFF ke transmitter */
		if (sendto(socketFD, sig, 1, 0, (struct sockaddr *)&transmitterAddr, sizeof(transmitterAddr)) == -1) {
			printError("Gagal mengirim sinyal XOFF");
		}

		/* Pesan keberhasilan mengirim */
		printf("Mengirim XOFF.\n");

		/* Set flag sigVal menjadi 0 */
		sigVal = 0;

		usleep(CONSUMESPEED * 1000);
		return NULL;
	} else {
		if(true) {
		// if (rand()%winSize!=(winSize/2)) {
			(*queue).addToQueue(receivedBuffer);
			return receivedBuffer;
		}
	} 
}

static char* q_get(int socketFD, Queue *queue) {
	int empty = (*queue).emptySlot();

	if (empty < RXQSIZE) {
		if ( ( (queue->count) < MAXLOWERLIMIT ) && ( sigVal == 0 ) ) {
			/* Membentuk sinyal XON */
			char sig[1];
			printf("Buffer < maximum lowerlimit.\n");
			sprintf(sig, "%c",(char) XON);

			/* Mengirim sinyal XON ke transmitter */
			if (sendto(socketFD, sig, 1, 0, (struct sockaddr *)&transmitterAddr, sizeof(transmitterAddr))==-1) {
				perror("sendto");
			}

			/* Pesan keberhasilan mengirim */
			printf("Mengirim XON.\n");

			/* Set flag sigVal menjadi 0 */
			sigVal = 1;

			usleep(CONSUMESPEED * 1000);

			/*Tidak bisa mengkonsumsi Byte, return NULL */
			return NULL;
			// consumedBuffer = (*queue).delFromQueue();
			// return consumedBuffer;
		} else {
			consumedBuffer = (*queue).delFromQueue();
			return consumedBuffer;
		}	
	} else {
		return NULL;
	}
}