#include <unordered_map>
#include <string>
#include <map>
#include <cmath>
#include "container.h"
#include "Message.h"
using namespace std;

class Orderbook {
    private:
        class Level {
            private:
                uint32_t price;
                map<uint32_t, uint32_t> orders;
                uint32_t aggQuant = 0;

            public:
                Level(uint32_t price);
                void add(uint32_t ref, uint32_t quantity);
                void remove(uint32_t ref, uint32_t quantity);
                bool empty() const { return orders.size() == 0;}
                void printLevel();
                uint32_t getAggQuant() const;
        };

        
        unordered_map<uint32_t, uint32_t> askRef2Price;
        unordered_map<uint32_t, uint32_t> bidRef2Price;

        inline void findBestBid();
        inline void findBestAsk();

    public:
        uint32_t bestBid = 0;
        uint32_t bestAsk = UINT32_MAX;
        unordered_map<uint32_t, Level*> askLevels;
        unordered_map<uint32_t, Level*> bidLevels;
        void add(MarketAddMessage* msg);
        void remove(MarketCancelMessage* msg);
        void trade(MarketTradeMessage* msg);
        void printOrderbook();
        // ~Orderbook();
};