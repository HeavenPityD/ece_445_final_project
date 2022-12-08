CC = g++
CFLAGS = -Wall -g -pthread -w
 
main: main.cpp Orderbook.cpp Message.cpp Communicator.cpp
	$(CC) $(CFLAGS) -o main main.cpp Orderbook.cpp Message.cpp Communicator.cpp

client: Client.cpp Message.cpp
	$(CC) $(CFLAGS) -o client Client.cpp Message.cpp

strategy: strategy.cpp Message.cpp ClientOrderbook.cpp
	$(CC) $(CFLAGS) -o strategy strategy.cpp ClientOrderbook.cpp Message.cpp

clean:
	rm main client