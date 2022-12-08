#include "Message.h"

ClientAddMessage* ClientAddMessage::decode(uint8_t* buf) {
    ClientAddMessage* ret = new ClientAddMessage();
    ret -> auth = *((uint32_t*) (buf+1));
    ret -> side = buf[5];
    ret -> price = *((uint32_t*) (buf+6));
    ret -> quant = *((uint32_t*) (buf+10));
    return ret;
} 

ClientCancelMessage* ClientCancelMessage::decode(uint8_t* buf) {
    ClientCancelMessage* ret = new ClientCancelMessage();
    ret -> auth = *((uint32_t*) (buf+1));
    ret -> side = buf[5];
    ret -> ref = *((uint32_t*) (buf+6));
    return ret;
}

uint32_t ClientAddMessage::toClientBinary(uint8_t* buf) {
    buf[0] = type;
    *((uint32_t*) (buf + 1)) = auth;
    buf[5] = side;
    *((uint32_t*) (buf + 6)) = price;
    *((uint32_t*) (buf + 10)) = quant;
    return 14;
}

uint32_t ClientCancelMessage::toClientBinary(uint8_t* buf) {
    buf[0] = type;
    *((uint32_t*) (buf + 1)) = auth;
    buf[5] = side;
    *((uint32_t*) (buf + 6)) = ref;
    return 10;
}

uint32_t MarketAddMessage::toMarketBinary(uint8_t* buf) {
    buf[0] = type;
    *((uint32_t*) (buf + 1)) = seq;
    buf[5] = side;
    *((uint32_t*) (buf + 6)) = ref;
    *((uint32_t*) (buf + 10)) = price;
    *((uint32_t*) (buf + 14)) = quant;
    return 18;
}

uint32_t MarketCancelMessage::toMarketBinary(uint8_t* buf) {
    buf[0] = type;
    *((uint32_t*) (buf + 1)) = seq;
    buf[5] = side;
    *((uint32_t*) (buf + 6)) = ref;
    return 10;
}

uint32_t MarketTradeMessage::toMarketBinary(uint8_t* buf) {
    buf[0] = type;
    *((uint32_t*) (buf + 1)) = seq;
    *((uint32_t*) (buf + 5)) = bidRef;
    *((uint32_t*) (buf + 9)) = askRef;
    *((uint32_t*) (buf + 13)) = price;
    *((uint32_t*) (buf + 17)) = quant;
    return 21;
}

MarketAddMessage* MarketAddMessage::decode(uint8_t* buf) {
    auto ret = new MarketAddMessage();
    ret -> seq = *((uint32_t*) (buf + 1));
    ret -> side = buf[5];
    ret -> ref = *((uint32_t*) (buf + 6));
    ret -> price = *((uint32_t*) (buf + 10));
    ret -> quant = *((uint32_t*) (buf + 14));
    return ret;
}

MarketCancelMessage* MarketCancelMessage::decode(uint8_t* buf) {
    auto ret = new MarketCancelMessage();
    ret -> seq = *((uint32_t*) (buf + 1));
    ret -> side = buf[5];
    ret -> ref = *((uint32_t*) (buf + 6));
    return ret;
}

MarketTradeMessage* MarketTradeMessage::decode(uint8_t* buf) {
    auto ret = new MarketTradeMessage();
    ret -> seq = *((uint32_t*) (buf + 1));
    ret -> bidRef = *((uint32_t*) (buf + 5));
    ret -> askRef = *((uint32_t*) (buf + 9));
    ret -> price = *((uint32_t*) (buf + 13));
    ret -> quant = *((uint32_t*) (buf + 17));
    return ret;
}
