#pragma once
#include <stdint.h>
struct ClientMessage {
    uint8_t type;
    uint32_t auth;
    virtual uint32_t toClientBinary(uint8_t* buf) = 0;
};

struct ClientAddMessage: public ClientMessage {
    uint8_t side;
    uint32_t price;
    uint32_t quant;
    ClientAddMessage() { type = 0;}
    static ClientAddMessage* decode(uint8_t* buf);
    uint32_t toClientBinary(uint8_t* buf);
};

struct ClientCancelMessage: public ClientMessage {
    uint8_t side;
    uint32_t ref;
    ClientCancelMessage() { type = 1;}
    static ClientCancelMessage* decode(uint8_t* buf);
    uint32_t toClientBinary(uint8_t* buf);
};

struct MarketMessage {
    uint8_t type;
    uint32_t seq;
    virtual uint32_t toMarketBinary(uint8_t* buf) = 0;
};

struct MarketAddMessage: public MarketMessage {
    uint8_t side;
    uint32_t ref;
    uint32_t price;
    uint32_t quant;
    MarketAddMessage() { type = 0;}
    uint32_t toMarketBinary(uint8_t* buf);
    static MarketAddMessage* decode(uint8_t* buf);
};

struct MarketCancelMessage: public MarketMessage {
    uint8_t side;
    uint32_t ref;
    MarketCancelMessage() { type = 1;}
    uint32_t toMarketBinary(uint8_t* buf);
    static MarketCancelMessage* decode(uint8_t* buf);
};

struct MarketTradeMessage: public MarketMessage {
    uint32_t bidRef;
    uint32_t askRef;
    uint32_t price;
    uint32_t quant;
    MarketTradeMessage() { type = 2;}
    uint32_t toMarketBinary(uint8_t* buf);
    static MarketTradeMessage* decode(uint8_t* buf);
};
