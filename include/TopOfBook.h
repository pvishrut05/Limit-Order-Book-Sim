#ifndef TOPOFBOOK_H
#define TOPOFBOOK_H

#include <cstdint>

struct Level {

    int64_t price = 0;
    uint64_t size = 0;

};

struct TopOfBook{
    Level ask[10];
    Level bid[10];
    uint8_t n_ask;
    uint8_t n_bid;

};

#endif