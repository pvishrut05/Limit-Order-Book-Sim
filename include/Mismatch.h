#ifndef MISMATCH_H
#define MISMATCH_H

#include <cstdint>



enum class Field {Occupancy, Price, Size};
enum class BookSide {Bid, Ask};
struct Mismatch{
    bool match = true;
    
    BookSide side = BookSide::Bid;
    int level = 0;
    Field field = Field::Occupancy;
    int64_t expected = 0;
    int64_t actual = 0;

};


#endif