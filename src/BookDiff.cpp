#include "DiffResult.h"
#include "TopOfBook.h"
#include "ReferenceBook.h"


static DiffResult Mismatch(BookSide s, int l, Field f, int64_t exp, int64_t act){
    return DiffResult{false, s, l, f, exp, act};
}



DiffResult BookDiff(TopOfBook& mine, TopOfBook& real){

    if(mine.n_ask != real.n_ask){
        return Mismatch(BookSide::Ask, 0, Field::Occupancy, real.n_ask, mine.n_ask);
    }
    if(mine.n_bid != real.n_bid){
        return Mismatch(BookSide::Bid, 0, Field::Occupancy, real.n_bid, mine.n_ask);
    }

    for(int i = 0; i < real.n_ask; i++){
        if(mine.ask[i].price != real.ask[i].price){
            return Mismatch(BookSide::Ask, i+1, Field::Price, real.ask[i].price, mine.ask[i].price);
        }else if(mine.ask[i].size != real.ask[i].size){
            return Mismatch(BookSide::Ask, i+1, Field::Size, real.ask[i].size, mine.ask[i].size);
        }
    }

    for(int i = 0; i < real.n_bid; i++){
        if(mine.bid[i].price != real.bid[i].price){
            return Mismatch(BookSide::Bid, i+1, Field::Price, real.bid[i].price, mine.bid[i].price);
        }else if(mine.bid[i].size != real.bid[i].size){
            return Mismatch(BookSide::Bid, i+1, Field::Size, real.bid[i].size, mine.bid[i].size);
        }
    }


    return DiffResult{};
}