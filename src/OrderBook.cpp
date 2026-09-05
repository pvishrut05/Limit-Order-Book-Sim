#include "OrderBook.h"
#include"IsSane.h"
#include<map>
#include<unordered_map>
#include<cstdint>
#include<algorithm>
#include<cassert>

namespace{
template <typename Book>
void take(Book& book, int64_t price, uint64_t amount) {
    auto it = book.find(price);
    if (it == book.end()) return;

    it->second -= std::min(it->second, amount);
    if (it->second == 0) book.erase(it);
}
}//namespace

void OrderBook::remove_from_level(int64_t price, uint64_t amount, int8_t side){
    assert((side == 1 || side == -1) && "bad side");

    if (side == 1) {
        take(bids_, price, amount);
    } else {
        take(asks_, price, amount);
    }
}





void OrderBook::New(uint64_t order_id, uint32_t size, int64_t price, int8_t side){
    assert((side == 1 || side == -1) && "bad side");
    assert(orders_.find(order_id) == orders_.end() && "duplicate order id");

    if(side == -1){ // we add to a type 1 and side -1 on ask
        asks_[price] += size;
    }else if(side == 1){//we add to a type 1 and side -1 on bid
        bids_[price] += size;
    }
   
    orders_[order_id].price_ticks = price;
    orders_[order_id].size = size;
    orders_[order_id].side = side;

}

void OrderBook::Cancel(uint64_t order_id, uint32_t size, int64_t price, int8_t side){
    
    auto it = orders_.find(order_id);

    if(it == orders_.end()){

        remove_from_level(price, size, side);
        unknown_id_hits_++;
        return;

    }

    RestingOrder& v = it->second;
    assert(v.price_ticks == price && "message price disagrees with recorded price");
    assert(v.side == side && "message side disagrees with recorded side");
    assert(v.size >= size && "cancelling more than the order holds");

    remove_from_level(v.price_ticks, size, v.side);

    v.size -= size;
    if (v.size == 0) orders_.erase(it);

}

void OrderBook::Delete(uint64_t order_id, uint32_t size, int64_t price, int8_t side){
    

    
    auto it = orders_.find(order_id);

    if(it == orders_.end()){
        remove_from_level(price, size, side);
        unknown_id_hits_++;
        return;
    }
    RestingOrder& v = it->second;
    assert(v.price_ticks == price && "message price disagrees with recorded price");
    assert(v.side == side && "message side disagrees with recorded side");

    remove_from_level(v.price_ticks, v.size, v.side);
    orders_.erase(it);

}

void OrderBook::Execute(uint64_t order_id, uint32_t size, int64_t price, int8_t side){
    
    auto it = orders_.find(order_id);

    if(it == orders_.end()){
        remove_from_level(price, size, side);
        unknown_id_hits_++;
        return;
    }

    RestingOrder& v = it->second;
    assert(v.price_ticks == price && "message price disagrees with recorded price");
    assert(v.side == side && "message side disagrees with recorded side");
    assert(v.size >= size && "executing more than the order holds");

    remove_from_level(v.price_ticks, size, v.side);

    v.size -= size;
    if (v.size == 0) orders_.erase(it);
}

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

    a.n_ask = static_cast<uint8_t>(n);

    i = 0;
    int m = std::min(bids_.size(), static_cast<size_t>(lvl));
    for(const auto& [k, v] : bids_){
        if(i >= m) break;
        a.bid[i].price = k;
        a.bid[i].size = v;
        i++;
    }
    a.n_bid = static_cast<uint8_t>(m);
    
    return a;

}

void OrderBook::seed(const TopOfBook& snap) {
    bids_.clear();
    asks_.clear();
    orders_.clear();
    unknown_id_hits_      = 0;
    

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