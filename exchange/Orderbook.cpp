#include "Orderbook.h"
#include <algorithm>
using namespace std;

volatile bool highLiquidityDone = false;
volatile bool lowLiquidityDone = false;
mutex globalLock;
stringstream bufferStream;

Orderbook::Level::Level(uint32_t price): price(price) {}

void Orderbook::Level::add(uint32_t ref, uint32_t quantity) {
    orders[ref] = quantity;
}

void Orderbook::Level::remove(uint32_t ref, uint32_t quantity) {
    if (quantity == 0) quantity = orders[ref];
    orders[ref] -= quantity;
    if (orders[ref] == 0) orders.erase(ref);
}

void Orderbook::Level::trade(Level* level, SafeQueue<MarketMessage*>& outputQueue, uint32_t& seq) {
    while (!empty() && !level -> empty()) {
        auto leftIt = orders.begin();
        auto rightIt = level -> orders.begin();
        uint32_t quant = min((*leftIt).second, (*rightIt).second);
        MarketTradeMessage* tradeMsg = new MarketTradeMessage();
        tradeMsg -> type = 2;
        tradeMsg -> seq = seq++;
        tradeMsg -> bidRef = leftIt -> first;
        tradeMsg -> askRef = rightIt -> first;
        tradeMsg -> price = price;
        cout << "trade price: " << price << endl;
        tradeMsg -> quant = quant;
        outputQueue.push(tradeMsg);
        remove((*leftIt).first, quant);
        level -> remove((*rightIt).first, quant);
    }
}

void Orderbook::Level::printLevel() {
    cout << '$' << price / 100.0 << ':';
    for (auto& it: orders) {
        cout << it.second << ' ';
    }
    cout << endl;
}

uint32_t Orderbook::add(ClientAddMessage* msg) {
    uint32_t price = msg -> price;
    uint32_t ret;
    if (!msg -> side && price > bestAsk) price = 0;
    if (msg -> side && price < bestBid) price = 0;
    if (price == 0) {
        if (!msg -> side) price = bestAsk;
        else price = bestBid;
    }
    if (!msg -> side) {
        if (bidLevels.find(price) == bidLevels.end()) {
            bidLevels[price] = new Level(price);
        }
        bidRef2Price[bidRef] = price;
        ret = bidRef;
        bidLevels[price] -> add(bidRef++, msg -> quant);
        bestBid = max(bestBid, price);
    } else {
        if (askLevels.find(price) == askLevels.end()) {
            askLevels[price] = new Level(price);
        }
        askRef2Price[askRef] = price;
        ret = askRef;
        askLevels[price] -> add(askRef++, msg -> quant);
        bestAsk = min(bestAsk, price);
    }
    MarketAddMessage* addMsg = new MarketAddMessage();
    addMsg -> type = 0;
    addMsg -> seq = seq++;
    addMsg -> side = msg -> side;
    addMsg -> ref = ret;
    addMsg -> price = msg -> price;
    addMsg -> quant = msg -> quant;
    outputQueue.push(addMsg);
    if (bestBid == bestAsk) trade(bestBid);
    return ret;
}

void Orderbook::remove(ClientCancelMessage* msg) {
    int price;
    if (!msg -> side) price = bidRef2Price[msg -> ref];
    else price = askRef2Price[msg -> ref];
    if (!msg -> side) {
        bidRef2Price.erase(msg -> ref);
        bidLevels[price] -> remove(msg -> ref, 0);
        if (bidLevels[price] -> empty()) {
            bidLevels.erase(price);
            if (price == bestBid) findBestBid();
        }
    } else {
        askRef2Price.erase(msg -> ref);
        askLevels[price] -> remove(msg -> ref, 0);
        if (askLevels[price] -> empty()) {
            askLevels.erase(price);
            if (price == bestAsk) findBestAsk();
        }
    }
    MarketCancelMessage* cancelMsg = new MarketCancelMessage();
    cancelMsg -> type = 1;
    cancelMsg -> seq = seq++;
    cancelMsg -> side = msg -> side;
    cancelMsg -> ref = msg -> ref;
    outputQueue.push(cancelMsg);
}

void Orderbook::trade(uint32_t price) {
    cout << "trade" << endl;
    bidLevels[price] -> trade(askLevels[price], outputQueue, seq);
    if (bidLevels[price] -> empty()) {
        bidLevels.erase(price);
        findBestBid();
    }
    if (askLevels[price] -> empty()) {
        askLevels.erase(price);
        findBestAsk();
    }
}


inline void Orderbook::findBestBid() {
    if (bidLevels.empty()) bestBid = 0;
    else while (bidLevels.find(bestBid) == bidLevels.end()) bestBid -= 1;
}

inline void Orderbook::findBestAsk() {
    if (askLevels.empty()) bestAsk = UINT32_MAX;
    else while (askLevels.find(bestAsk) == askLevels.end()) bestAsk += 1;
}

Orderbook::~Orderbook() {
    for (auto it = bidLevels.begin(); it != bidLevels.end(); it++) {
        delete (*it).second;
    }
    for (auto it = askLevels.begin(); it != askLevels.end(); it++) {
        delete (*it).second;
    }
}

void Orderbook::printOrderbook() {
    cout << "--------------Orderbook-------------" << endl;
    vector<uint32_t> askPrice;
    for (auto& it: askLevels) {
        askPrice.push_back(it.first);
    }
    sort(askPrice.begin(), askPrice.end());
    for (int i = askPrice.size()-1; i >= 0; i--) askLevels[askPrice[i]] -> printLevel();
    cout << "------------------------------------" << endl;
    vector<uint32_t> bidPrice;
    for (auto& it: bidLevels) {
        bidPrice.push_back(it.first);
    }
    sort(bidPrice.begin(), bidPrice.end());
    for (int i = bidPrice.size() - 1; i >= 0; i--) bidLevels[bidPrice[i]] -> printLevel();

    for (unsigned i = 0; i < 5; i++) cout << endl;
}
