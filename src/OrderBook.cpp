#include "OrderBook.h"
#include<map>
#include<unordered_map>
#include<vector>
#include <cstdint>
#include<algorithm>
#include<cassert>
#include<IsSane.h>

void OrderBook::New(uint64_t order_id, uint32_t size, int64_t price, int8_t side){

}
void OrderBook::Cancel(uint64_t order_id, uint32_t size){}
void OrderBook::Delete(uint64_t order_id){}
void OrderBook::Execute(uint64_t order_id, uint32_t size){}
//have a SetUp function that will get the top-10 level 10? also do some tracking by hand to get a better understanding of how the types work, how the bid and ask work.
TopOfBook OrderBook::top(int lvl){
    return TopOfBook{};
}






void OrderBook::seed(const TopOfBook& snap) {
    bids_.clear();
    asks_.clear();
    orders_.clear();
    unknown_id_hits_ = 0;

    for (int i = 0; i < snap.n_bid; ++i) {
        const Level& l = snap.bid[i];
        assert(l.size > 0 && "occupied seed level with zero size");
        bids_[l.price] = l.size;
    }
    for (int i = 0; i < snap.n_ask; ++i) {
        const Level& l = snap.ask[i];
        assert(l.size > 0 && "occupied seed level with zero size");
        asks_[l.price] = l.size;
    }

    assert(is_sane(top(10)) && "seeded book is not sane");
}

