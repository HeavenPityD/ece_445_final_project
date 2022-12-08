#include "ClientOrderbook.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include "Message.h"
#include <thread>
#include <string>
#include <iostream>
#include <unordered_map>
#include <iomanip>
#include <mutex>
#include <chrono>
#include <ctime>  
#define RECEIVE_PORT 8080 
#define BROADCAST_PORT 9090
#define MAXLINE 1024
#define SERVER "127.0.0.1"
Orderbook book;
uint32_t curSeq = 0;
int32_t stockBalance = 0;
uint32_t balance = 10000;
uint32_t lastTrade = 100;


unordered_map<uint32_t, ClientAddMessage*> pendingBidOrders;
mutex bidLock;
unordered_map<uint32_t, ClientAddMessage*> pendingAskOrders;
mutex askLock;
mutex globalLock;
bool flag = false;
void receiveThread() {
    uint8_t buf[MAXLINE];
    struct sockaddr_in     servaddr; 
    int sockfd;
    
    unordered_map<uint32_t, MarketMessage*> seq2Msg;
    
    int n;
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
            perror("socket creation failed"); 
            exit(EXIT_FAILURE); 
    } 

    memset(&servaddr, 0, sizeof(servaddr)); 
    
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.255.255.255");
    servaddr.sin_port = htons(BROADCAST_PORT); 
    
    uint8_t len;
    
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    while (1) {
        n = recvfrom(sockfd, (char *)buf, MAXLINE,  
                    MSG_WAITALL, (struct sockaddr *) &servaddr, 
                    (socklen_t*) &len);
        lock_guard<mutex> guard(globalLock);
        if (n > 0 && buf[0] != 3) {
            uint32_t seq = *((uint32_t*) (buf+1));
            if (buf[0] == 0) seq2Msg[seq] = MarketAddMessage::decode(buf);
            else if (buf[0] == 1) seq2Msg[seq] = MarketCancelMessage::decode(buf);
            else if (buf[0] == 2) seq2Msg[seq] = MarketTradeMessage::decode(buf);
            while (seq2Msg.find(curSeq) != seq2Msg.end()) {
                if (seq2Msg[curSeq] -> type == 0) book.add((MarketAddMessage*) seq2Msg[curSeq]);
                if (seq2Msg[curSeq] -> type == 1) book.remove((MarketCancelMessage*) seq2Msg[curSeq]);
                if (seq2Msg[curSeq] -> type == 2) {
                    auto tradeMsg = (MarketTradeMessage*) seq2Msg[curSeq];    
                    book.trade(tradeMsg);
                    lastTrade = tradeMsg -> price;
                    if (pendingAskOrders.find(tradeMsg -> askRef) != pendingAskOrders.end()) {
                        uint32_t quant = tradeMsg -> quant;
                        pendingAskOrders[tradeMsg -> askRef] -> quant -= quant;
                        if (pendingAskOrders[tradeMsg -> askRef] -> quant == 0) pendingAskOrders.erase(tradeMsg -> askRef);
                        balance += quant * tradeMsg -> price;
                        stockBalance -= quant;
                    }
                    if (pendingBidOrders.find(tradeMsg -> bidRef) != pendingBidOrders.end()) {
                        uint32_t quant = tradeMsg -> quant;
                        pendingBidOrders[tradeMsg -> bidRef] -> quant -= quant;
                        if (pendingBidOrders[tradeMsg -> bidRef]-> quant == 0) pendingBidOrders.erase(tradeMsg -> bidRef);
                        balance -= quant * tradeMsg -> price;
                        stockBalance += quant;
                    }
                }
                curSeq++;
            }
        }
    }
}

void terminal() {
    while(1) {
        globalLock.lock();
        system("clear");
        book.printOrderbook();
        for (unsigned i = 0; i < 5; i++) cout << endl;
        cout << "pending orders" << endl;
        for (auto& it: pendingAskOrders) {
            cout << "reference number: " << it.first << " side: " << it.second -> side << " price: " << it.second -> price << " quantity: " << it.second -> quant << endl;
        }
        for (auto& it: pendingBidOrders) {
            cout << "reference number: " << it.first << " side: " << it.second -> side << " price: " << it.second -> price << " quantity: " << it.second -> quant << endl;
        }
        for (unsigned i = 0; i < 5; i++) cout << endl;
        cout << "stock holdings: " << stockBalance << " shares" << endl;
        cout << "account value: " << (stockBalance * lastTrade + balance)/100.0 << endl;
        globalLock.unlock();
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

void strategyThread() {
    uint8_t buf[MAXLINE];
    char buffer[MAXLINE]; 
    struct sockaddr_in servaddr; 
    int sockfd;
    memset(&servaddr, 0, sizeof(servaddr)); 
        
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(RECEIVE_PORT); 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        
    int n; 
    uint8_t len;
    
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
            perror("socket creation failed"); 
            exit(EXIT_FAILURE); 
        } 
    uint32_t myBid = book.bestBid + 1;
    uint32_t myAsk = book.bestAsk - 1;
    while (1) {
        globalLock.lock();
        if (book.bestBid >= myAsk) {
            auto start = std::chrono::system_clock::now();
            ClientAddMessage* add = new ClientAddMessage();
            add -> auth = 0;
            add -> side = 1;
            add -> price = book.bestBid;
            add -> quant = book.bidLevels[book.bestBid]->getAggQuant();
            n = add->toClientBinary(buf);
            sendto(sockfd, buf, n, 
                    MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
                        sizeof(servaddr)); 
            n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
                            MSG_WAITALL, (struct sockaddr *) &servaddr, 
                            (socklen_t*) &len);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end-start;
            std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        
            std::cout << "elapsed time: " << elapsed_seconds.count() << "s"
                    << std::endl;
            uint32_t ref = *((uint32_t*)(buffer+2));
            pendingAskOrders[ref] = add;
        } else if (book.bestAsk <= myBid) {
            ClientAddMessage* add = new ClientAddMessage();
            add -> auth = 0;
            add -> side = 0;
            add -> price = book.bestAsk;
            add -> quant = book.askLevels[book.bestAsk]->getAggQuant();
            n = add->toClientBinary(buf);
            sendto(sockfd, buf, n, 
                    MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
                        sizeof(servaddr)); 
            n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
                            MSG_WAITALL, (struct sockaddr *) &servaddr, 
                            (socklen_t*) &len);
            uint32_t ref = *((uint32_t*)(buffer+2));
            pendingBidOrders[ref] = add;
        } else {
            myBid = book.bestBid + 1;
            myAsk = book.bestAsk - 1;
        }
        globalLock.unlock();
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

int main() {   
    cout << fixed;
    cout << setprecision(2);
    thread t1(receiveThread);
    thread t2(terminal);
    thread t3(strategyThread);
    t1.join();
    int a;
    return 0;
}