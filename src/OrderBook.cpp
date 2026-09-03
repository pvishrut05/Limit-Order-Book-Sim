#include "OrderBook.h"



void OrderBook::New(uint64_t order_id, uint32_t size, int64_t price, int8_t side){}
void OrderBook::Cancel(uint64_t order_id, uint32_t size){}
void OrderBook::Delete(uint64_t order_id){}
void OrderBook::Execute(uint64_t order_id, uint32_t size){}

TopOfBook OrderBook::top(int lvl){
    return TopOfBook{};
}