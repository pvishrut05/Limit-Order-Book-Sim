#include "doctest.h"
#include "TopOfBook.h"
#include "DiffResult.h"
#include "BookDiff.h"
#include <initializer_list>
#include <vector>


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

static TopOfBook makeTest(std::initializer_list<Level> ask, std::initializer_list<Level> bid){
    TopOfBook mk{};

    int i = 0;
    for(const Level& l : ask){
        mk.ask[i++] = l;
    }
    mk.n_ask = static_cast<uint8_t>(ask.size());
    i = 0;
    for (const Level& l : bid){
        mk.bid[i++] = l;
    }
    mk.n_bid = static_cast<uint8_t>(bid.size());
    return mk;

}


TEST_CASE("Two identical books match"){

    TopOfBook same = makeTest({{275200, 66}, {275100, 400}, {275300, 1000}, {275000, 100}},
                              {{275200, 66}, {275100, 400}, {275300, 1000}, {275000, 100}});

    DiffResult check = BookDiff(same, same, 5);

    CHECK(check.match);
}


TEST_CASE("Size differs at bid lvl 1"){
    TopOfBook mine = makeTest({{275200, 66}, {275100, 400}, {275300, 1000}, {275000, 100}},
                              {{275200, 66}, {275100, 400}, {275300, 1000}, {275000, 100}});
    TopOfBook act = makeTest({{275200, 66}, {275100, 400}, {275300, 1000}, {275000, 100}},
                             {{275200, 67}, {275100, 400}, {275300, 1000}, {275000, 100}});

    DiffResult check = BookDiff(mine, act, 5);
    CHECK_FALSE(check.match);
    CHECK(check.side == BookSide::Bid);
    CHECK(check.level == 1);
    CHECK(check.field == Field::Size);
    CHECK(check.expected == 67);
    CHECK(check.actual == 66);
    
}

TEST_CASE("Price differs at ask lvl 3"){
    TopOfBook mine = makeTest({{275200, 66}, {275100, 400}, {275500, 1000}, {275000, 100}}, 
                              {{275200, 66}, {275100, 400}, {275300, 1000}, {275000, 100}});
    TopOfBook act = makeTest({{275200, 66}, {275100, 400}, {275300, 1000}, {275000, 100}},
                             {{275200, 66}, {275100, 400}, {275300, 1000}, {275000, 100}});

    DiffResult check = BookDiff(mine, act, 5);
    CHECK_FALSE(check.match);
    CHECK(check.side == BookSide::Ask);
    CHECK(check.level == 3);
    CHECK(check.field == Field::Price);
    CHECK(check.expected == 275300);
    CHECK(check.actual == 275500);
}

TEST_CASE("My book thinner"){
    TopOfBook mine = makeTest({{275200, 66}, {275100, 400}}, 
                              {{275300, 66}, {275100, 400}});
    TopOfBook act = makeTest({{275200, 66}, {275100, 400}, {275500, 1000}, {275000, 100}}, 
                              {{275300, 66}, {275100, 400}, {275300, 1000}, {275000, 100}});

    DiffResult check = BookDiff(mine, act, 5);
    CHECK_FALSE(check.match);
    CHECK(check.field == Field::Occupancy);
    CHECK(check.expected == 4);
    CHECK(check.actual == 2);
}


TEST_CASE("My book bigger"){
     TopOfBook act = makeTest({{275200, 66}, {275100, 400}}, 
                              {{275200, 66}, {275100, 400}});
    TopOfBook mine = makeTest({{275200, 66}, {275100, 400}, {275500, 1000}, {275000, 100}}, 
                              {{275200, 66}, {275100, 400}, {275300, 1000}, {275000, 100}});
    DiffResult check = BookDiff(mine, act, 5);
    
    CHECK_FALSE(check.match);
    CHECK(check.field == Field::Occupancy);
    CHECK(check.expected == 2);
    CHECK(check.actual == 4);
}















