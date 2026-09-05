#include "DiffResult.h"
#include "TopOfBook.h"
#include "ReferenceBook.h"
#include <algorithm>

static DiffResult Mismatch(BookSide s, int l, Field f, int64_t exp, int64_t act){
    return DiffResult{false, s, l, f, exp, act};
}



DiffResult BookDiff(const TopOfBook& mine, const TopOfBook& real, int lvl){

    int mine_na = std::min<int>(mine.n_ask, lvl);
    int real_na = std::min<int>(real.n_ask, lvl);
    if(mine_na != real_na){
        return Mismatch(BookSide::Ask, 0, Field::Occupancy, real.n_ask, mine.n_ask);
    }
    int mine_nb = std::min<int>(mine.n_bid, lvl);
    int real_nb = std::min<int>(real.n_bid, lvl);
    if(mine_nb != real_nb){
        return Mismatch(BookSide::Bid, 0, Field::Occupancy, real.n_bid, mine.n_ask);
    }

    int na = min<int>(real.n_ask, lvl);
    for(int i = 0; i < na; i++){
        if(mine.ask[i].price != real.ask[i].price){
            return Mismatch(BookSide::Ask, i+1, Field::Price, real.ask[i].price, mine.ask[i].price);
        }else if(mine.ask[i].size != real.ask[i].size){
            return Mismatch(BookSide::Ask, i+1, Field::Size, real.ask[i].size, mine.ask[i].size);
        }
    }
    int nb = min<int>(real.n_bid, lvl);
    for(int i = 0; i < nb; i++){
        if(mine.bid[i].price != real.bid[i].price){
            return Mismatch(BookSide::Bid, i+1, Field::Price, real.bid[i].price, mine.bid[i].price);
        }else if(mine.bid[i].size != real.bid[i].size){
            return Mismatch(BookSide::Bid, i+1, Field::Size, real.bid[i].size, mine.bid[i].size);
        }
    }


    return DiffResult{};
}

