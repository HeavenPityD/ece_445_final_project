#include "ClientOrderbook.h"
#include <algorithm>
Orderbook::Level::Level(uint32_t price): price(price) {}

void Orderbook::Level::add(uint32_t ref, uint32_t quantity) {
    orders[ref] = quantity;
    aggQuant += quantity;
}

void Orderbook::Level::remove(uint32_t ref, uint32_t quantity) {
    if (quantity == 0) quantity = orders[ref];
    orders[ref] -= quantity;
    aggQuant -= quantity;
    if (orders[ref] == 0) orders.erase(ref);
}

void Orderbook::Level::printLevel() {
    cout << '$' << price / 100.0 << ':';
    for (auto& it: orders) {
        cout << it.second << ' ';
    }
    cout << endl;
}

uint32_t Orderbook::Level::getAggQuant() const {
    return aggQuant;
}

void Orderbook::add(MarketAddMessage* msg) {
    uint32_t price = msg -> price;
    cout << price << endl;
    if (!msg -> side) {
        if (bidLevels.find(price) == bidLevels.end()) {
            bidLevels[price] = new Level(price);
        }
        bidRef2Price[msg -> ref] = price;
        bidLevels[price] -> add(msg -> ref, msg -> quant);
        bestBid = max(bestBid, price);
    } else {
        if (askLevels.find(price) == askLevels.end()) {
            askLevels[price] = new Level(price);
        }
        askRef2Price[msg -> ref] = price;
        askLevels[price] -> add(msg -> ref, msg -> quant);
        bestAsk = min(bestAsk, price);
    }
}

void Orderbook::remove(MarketCancelMessage* msg) {
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
}

void Orderbook::trade(MarketTradeMessage* msg) {
    int price = msg -> price;

    bidLevels[price] -> remove(msg -> bidRef, msg -> quant);
    if (bidLevels[price] -> empty()) {
        bidLevels.erase(price);
        findBestBid();
    }
    askLevels[price] -> remove(msg -> askRef, msg -> quant);
    if (askLevels[price] -> empty()) {
        askLevels.erase(price);
        findBestAsk();
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
    for (unsigned i = 0; i < bidPrice.size(); i--) bidLevels[bidPrice[i]] -> printLevel();
}

inline void Orderbook::findBestBid() {
    if (bidLevels.empty()) bestBid = 0;
    else while (bidLevels.find(bestBid) == bidLevels.end()) bestBid -= 1;
}

inline void Orderbook::findBestAsk() {
    if (askLevels.empty()) bestAsk = UINT32_MAX;
    else while (askLevels.find(bestAsk) == askLevels.end()) bestAsk += 1;
}