#ifndef REFERENCEBOOK_H
#define REFERENCEBOOK_H

#include <cstdint>
using namespace std;

struct ReferenceBook{

    int64_t ask_px[10];
    uint64_t ask_sz[10];
    int64_t bid_px[10];
    uint64_t bid_sz[10];
    uint8_t n_ask = 0;
    uint8_t n_bid = 0;
};


#endif