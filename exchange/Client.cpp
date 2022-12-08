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
#define PORT     8080 
#define MAXLINE 1024
#define SERVER "127.0.0.1"

using namespace std; 

struct sockaddr_in servaddr; 
int sockfd;

void parseInput(ClientAddMessage* msg) {
    string temp;
    cin >> temp;
    msg -> side = stoi(temp);
    cin >> temp;
    msg -> price = stoi(temp);
    cin >> temp;
    msg -> quant = stoi(temp);
}

void sendThread() {
    uint8_t len;
    uint8_t buf[MAXLINE];
    ClientAddMessage* msg = new ClientAddMessage();
    while (1) {
        parseInput(msg);
        len = msg -> toClientBinary(&buf[0]);
    
        sendto(sockfd, buf, len, 
            MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
                sizeof(servaddr));
    }
}

void receiveThread() {
    uint8_t len;
    uint8_t buf[MAXLINE];
    char buffer[MAXLINE];
    int n;
    while (1) {
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
                    MSG_WAITALL, (struct sockaddr *) &servaddr, 
                    (socklen_t*) &len);
        if (n > 0) {
            if (buffer[0] == 0) {
                cout << "add order market data" << endl;
            } else if (buffer[0] == 1) {
                cout << "cancel order market data" << endl;
            } else if (buffer[0] == 2) {
                cout << "trade market data" << endl;
            } else {
                if (buffer[1] == 0) {
                    cout << "add order successful, reference number " << *((uint32_t*)(buffer+2)) << endl;
                }
            }
        }
    }
}

int main() { 
    uint8_t buf[MAXLINE];
    char buffer[MAXLINE]; 
    memset(&servaddr, 0, sizeof(servaddr)); 
        
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        
    int n; 
    uint8_t len;
    
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
            perror("socket creation failed"); 
            exit(EXIT_FAILURE); 
        } 
    buf[0] = 3;
    *((uint32_t*) (buf+1)) = 0;
    sendto(sockfd, buf, 5, 
            MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
                sizeof(servaddr)); 
    thread t(sendThread);
    // thread t2(receiveThread);
    t.join();
    // t2.join();
    return 0;
}