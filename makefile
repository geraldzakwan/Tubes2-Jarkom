all: 
	g++ -c acknowledgement.cpp -std=c++11
	g++ -c queue.cpp -std=c++11
	g++ -c window.cpp -std=c++11
	g++ -c receiver.cpp -std=c++11
	g++ -c transmitter.cpp -std=c++11
	g++ acknowledgement.o queue.o window.o receiver.o -pthread -o receiver -std=c++11
	g++ acknowledgement.o queue.o window.o transmitter.o -pthread -o transmitter -std=c++11
