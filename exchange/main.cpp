#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <execution>
#include <queue>
#include <functional>
#include <condition_variable>
#include <queue>
#include "Orderbook.h"
#include "Communicator.h"
#include <iomanip>

using namespace std;
using namespace std::chrono;

SafeQueue<MarketMessage*> outputQueue;
Orderbook book(outputQueue);
Communicator com(outputQueue, book);

void terminal() {
    while(1) {
        system("clear");
        book.printOrderbook();
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

int main() {
    cout << fixed;
    cout << setprecision(2);
    thread term(terminal);
    thread listen = com.listenThread();
    thread broadcast = com.broadcastThread();
    listen.join();
}
