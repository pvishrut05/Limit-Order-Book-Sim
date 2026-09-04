#include "OrderBook.h"
#include<map>
#include<unordered_map>
#include<vector>
#include <cstdint>
#include<algorithm>



void OrderBook::New(uint64_t order_id, uint32_t size, int64_t price, int8_t side){
    
}
void OrderBook::Cancel(uint64_t order_id, uint32_t size){}
void OrderBook::Delete(uint64_t order_id){}
void OrderBook::Execute(uint64_t order_id, uint32_t size){}
//have a SetUp function that will get the top-10 level 10? also do some tracking by hand to get a better understanding of how the types work, how the bid and ask work.
TopOfBook OrderBook::top(int lvl){
    return TopOfBook{};
}