#include <unordered_map>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <cmath>
#include <float.h>
#include <thread>
#include <sstream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "container.h"
#include "Message.h"
#pragma once
using namespace std;

class Orderbook {
    private:
        class Level {
            private:
                uint32_t price;
                map<uint32_t, uint32_t> orders;

            public:
                Level(uint32_t price);
                void add(uint32_t ref, uint32_t quantity);
                void remove(uint32_t ref, uint32_t quantity);
                bool empty() const {
                    return orders.size() == 0;
                }
                void trade(Level* level, SafeQueue<MarketMessage*>& outputQueue, uint32_t& seq);
                void printLevel();
                
        };

        unordered_map<uint32_t, Level*> askLevels;
        unordered_map<uint32_t, Level*> bidLevels;
        unordered_map<uint32_t, uint32_t> askRef2Price;
        unordered_map<uint32_t, uint32_t> bidRef2Price;
        SafeQueue<MarketMessage*>& outputQueue;
        uint32_t bestBid = 0;
        uint32_t bestAsk = UINT32_MAX;
        uint32_t bidRef = 0;
        uint32_t askRef = 0;
        uint32_t seq = 0;

        inline void findBestBid();
        inline void findBestAsk();

    public:
        Orderbook(SafeQueue<MarketMessage*>& outputQueue): outputQueue(outputQueue) {}
        uint32_t add(ClientAddMessage* msg);
        void remove(ClientCancelMessage* msg);
        void trade(uint32_t price);
        void printOrderbook();
        ~Orderbook();
};

