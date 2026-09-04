#include "OrderBook.h"
#include<map>
#include<unordered_map>
#include<vector>
#include <cstdint>
#include<algorithm>
#include<cassert>
#include"IsSane.h"
//std::map<int64_t, uint64_t, std::greater<int64_t>>  bids_;
//std::map<int64_t, uint64_t>                         asks_;
//std::unordered_map<uint64_t, RestingOrder>          orders_;
//size_t unknown_id_hits_;
void OrderBook::New(uint64_t order_id, uint32_t size, int64_t price, int8_t side){
    if(side == -1){ // we add to a type 1 and side -1 on ask
        // if(asks_.contains(price)){
        //     asks_[price] += size;
        // }else{
        //     for(int i = 0; i < asks_.size(); i++){
        //     }
        // }

        asks_[price] += size;

    }else if(side == 1){//we add to a type 1 and side -1 on bid
        bids_[price] += size;
    }
    orders_[order_id].price_ticks = price;
    orders_[order_id].size = size;
    orders_[order_id].side = side;
}
void OrderBook::Cancel(uint64_t order_id, uint32_t size){

}
void OrderBook::Delete(uint64_t order_id){}
void OrderBook::Execute(uint64_t order_id, uint32_t size){}
//have a SetUp function that will get the top-10 level 10? also do some tracking by hand to get a better understanding of how the types work, how the bid and ask work.
TopOfBook OrderBook::top(int lvl) const{
    TopOfBook a{};

    int n = std::min(asks_.size(), static_cast<size_t>(lvl));
    int i = 0;
    for(const auto& [k, v] : asks_){
        if(i >= n) break;
        a.ask[i].price = k;
        a.ask[i].size = v;
        i++;
    }

    i = 0;
    int m = std::min(bids_.size(), static_cast<size_t>(lvl));
    for(const auto& [k, v] : bids_){
        if(i >= m) break;
        a.bid[i].price = k;
        a.bid[i].size = v;
        i++;
    }

    if(asks_.size() >= 10){
        a.n_ask = lvl;
    }else{
        a.n_ask = std::min(static_cast<uint8_t>(asks_.size()),static_cast<uint8_t>(lvl));
    }

    if(bids_.size() >= 10){
        a.n_bid = lvl;
    }else{
        a.n_bid = std::min(static_cast<uint8_t>(bids_.size()),static_cast<uint8_t>(lvl));
    }
    return a;

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

