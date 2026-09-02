#ifndef ORDERBOOK_H
#define ORDERBOOK_H


#include "TopOfBook.h"
#include <cstdint>

class OrderBook {
    public:
        void New(uint64_t order_id, uint32_t size, uint64_t price, int8_t side);
        void Cancel(uint64_t order_id, uint32_t size);
        void Delete(uint64_t order_id);
        void Execute(uint64_t order_id, uint32_t size);

        TopOfBook top(int lvl);
};




#endif