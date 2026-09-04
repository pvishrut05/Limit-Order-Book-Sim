#ifndef TOPOFBOOK_H
#define TOPOFBOOK_H

#include <cstdint>

struct Level {

    int64_t price = 0;
    uint64_t size = 0;

};

struct RestingOrder{
    int64_t price_ticks;
    uint64_t size;
    int8_t side;
};

struct TopOfBook{
    Level ask[10];
    Level bid[10];
    uint8_t n_ask;
    uint8_t n_bid;

};

#endif