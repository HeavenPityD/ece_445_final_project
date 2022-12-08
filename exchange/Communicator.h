#include "Message.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <unordered_map>
#include <string>
#include <thread>
#include "container.h"
#include "Orderbook.h"
#define RECEIVE_PORT 8080 
#define BROADCAST_PORT 9090
#define MAXLINE 1024 

using namespace std;
class Communicator {
    private:
        SafeQueue<MarketMessage*>& outputQueue;
        uint32_t seqNum = 0;
        Orderbook& book;
        unordered_map<uint32_t, sockaddr*> users;
        
        
        void listen();
        void broadcast();
    public:
        Communicator(SafeQueue<MarketMessage*>& outputQueue, Orderbook& book);

        thread broadcastThread() { return thread([=] {broadcast();});}

        thread listenThread() { return thread([=] {listen();});}
};

