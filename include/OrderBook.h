#ifndef ORDERBOOK_H
#define ORDERBOOK_H


#include "TopOfBook.h"
#include <cstdint>
#include<map>
#include<unordered_map>

class OrderBook {
    public:
        void New(uint64_t order_id, uint32_t size, int64_t price, int8_t side);
        void Cancel(uint64_t order_id, uint32_t size, int64_t price, int8_t side);
        void Delete(uint64_t order_id, uint32_t size, int64_t price, int8_t side);
        void Execute(uint64_t order_id, uint32_t size, int64_t price, int8_t side);
        void seed(const TopOfBook& snap);
        size_t unknown_id_hits() const {return unknown_id_hits_;}

        TopOfBook top(int lvl) const;
    private:
        std::map<int64_t, uint64_t, std::greater<int64_t>>  bids_;
        std::map<int64_t, uint64_t>                         asks_;
        std::unordered_map<uint64_t, RestingOrder>          orders_;
        size_t unknown_id_hits_;
};




#endif