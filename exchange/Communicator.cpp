#include "Communicator.h"

Communicator::Communicator(SafeQueue<MarketMessage*>& outputQueue, Orderbook& book): outputQueue(outputQueue), book(book) {
    
}

void Communicator::listen() {
    uint8_t buffer[MAXLINE]; 
    uint8_t resp[MAXLINE];
    int len; 
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    len = sizeof(cliaddr);
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
                
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
        
    servaddr.sin_family    = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(RECEIVE_PORT); 
        
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 

    while (1) {
        recvfrom(sockfd, (uint8_t *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &cliaddr, (socklen_t*) &len); 
        if (buffer[0] == 0) {
            ClientAddMessage* msg = ClientAddMessage::decode(buffer);
            uint32_t ref = book.add(msg);
            resp[0] = 3;
            resp[1] = 0;
            *((uint32_t*) (resp+2)) = ref;
        } else if (buffer[0] == 1) {
            ClientCancelMessage* msg = ClientCancelMessage::decode(buffer);
            book.remove(msg);
            resp[0] = 3;
            resp[1] = 1;
        } else {
            uint32_t auth = *((uint32_t*) &buffer[1]);
            users[auth] = ( struct sockaddr *) &cliaddr;
            // cout << auth << endl;
            resp[0] = 3;
            resp[1] = 3;
        }
        sendto(sockfd, (char*) resp, 6,  
            MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
                len);
    }
}

void Communicator::broadcast() {
    uint8_t buffer[MAXLINE]; 
    uint8_t resp[MAXLINE];
    int len; 
    int sockfd;
    struct sockaddr_in bcaddr;
    len = sizeof(bcaddr);
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
                
    memset(&bcaddr, 0, sizeof(bcaddr)); 

    bcaddr.sin_family = AF_INET;
    bcaddr.sin_addr.s_addr = inet_addr("127.255.255.255");
    bcaddr.sin_port = htons(BROADCAST_PORT);

    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &bcaddr,
        sizeof(bcaddr)) == -1) {
        perror("setsockopt (SO_BROADCAST)");
        exit(1);
    }
    while (1) {
        if (!outputQueue.empty()) {
            uint8_t buffer[MAXLINE];
            int len = sizeof(bcaddr);
            MarketMessage* msg = outputQueue.pop();
            int msgLen = msg -> toMarketBinary(buffer);
            sendto(sockfd, buffer, msgLen, MSG_CONFIRM, (struct sockaddr *) &bcaddr, sizeof(bcaddr));
            // cout << "message sent" << endl;
        }
    }
}