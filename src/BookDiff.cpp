#include "DiffResult.h"
#include "TopOfBook.h"
#include "ReferenceBook.h"




// struct Level {

//     int64_t price = 0;
//     uint64_t size = 0;

// };


// struct TopOfBook{
//     Level ask[10];
//     Level bid[10];
//     uint8_t n_ask;
//     uint8_t n_bid;

// // };


// enum class Field {Occupancy, Price, Size};
// enum class BookSide {Bid, Ask};
// struct DiffResult{
//     bool match = true;

//     BookSide side = BookSide::Bid;
//     int level = 0;
//     Field field = Field::Occupancy;
//     int64_t expected = 0;
//     int64_t actual = 0;

// };

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
            return Mismatch(BookSide::Ask, i, Field::Price, real.ask[i].price, mine.ask[i].price);
        }else if(mine.ask[i].size != real.ask[i].size){
            return Mismatch(BookSide::Ask, i, Field::Size, real.ask[i].size, mine.ask[i].size);
        }
    }

    for(int i = 0; i < real.n_bid; i++){
        if(mine.bid[i].price != real.bid[i].price){
            return Mismatch(BookSide::Bid, i, Field::Price, real.bid[i].price, mine.bid[i].price);
        }else if(mine.bid[i].size != real.bid[i].size){
            return Mismatch(BookSide::Bid, i, Field::Size, real.bid[i].size, mine.bid[i].size);
        }
    }


    return DiffResult{};
}