#include "doctest.h"
#include "OrderBook.h"
#include "IsSane.h"

TEST_CASE("three adds per side, distinct prices") {
    OrderBook b;

    // side 1 = bid, side -1 = ask
    b.New(1001, 500, 2650, 1);
    b.New(1002, 300, 2649, 1);
    b.New(1003, 100, 2648, 1);

    b.New(2001, 400, 2651, -1);
    b.New(2002, 200, 2652, -1);
    b.New(2003, 600, 2653, -1);

    TopOfBook t = b.top(5);

    CHECK(t.n_bid == 3);
    CHECK(t.n_ask == 3);

    // bids descend from the best
    CHECK(t.bid[0].price == 2650);  CHECK(t.bid[0].size == 500);
    CHECK(t.bid[1].price == 2649);  CHECK(t.bid[1].size == 300);
    CHECK(t.bid[2].price == 2648);  CHECK(t.bid[2].size == 100);

    // asks ascend from the best
    CHECK(t.ask[0].price == 2651);  CHECK(t.ask[0].size == 400);
    CHECK(t.ask[1].price == 2652);  CHECK(t.ask[1].size == 200);
    CHECK(t.ask[2].price == 2653);  CHECK(t.ask[2].size == 600);

    CHECK(is_sane(t));
}

TEST_CASE("insertion order does not matter") {
    OrderBook b;

    // deliberately out of rank order
    b.New(1002, 300, 2649, 1);
    b.New(1003, 100, 2648, 1);
    b.New(1001, 500, 2650, 1);

    TopOfBook t = b.top(5);
    CHECK(t.n_bid == 3);
    CHECK(t.bid[0].price == 2650);
    CHECK(t.bid[1].price == 2649);
    CHECK(t.bid[2].price == 2648);
}

TEST_CASE("multiple orders at one price aggregate") {
    OrderBook b;

    b.New(1001, 500, 2650, 1);
    b.New(1002, 300, 2650, 1);
    b.New(1003, 200, 2650, 1);

    TopOfBook t = b.top(5);

    CHECK(t.n_bid == 1);              // one price level, not three
    CHECK(t.bid[0].price == 2650);
    CHECK(t.bid[0].size  == 1000);
}

TEST_CASE("fewer levels than requested") {
    OrderBook b;
    b.New(1001, 500, 2650, 1);
    b.New(2001, 400, 2651, -1);

    TopOfBook t = b.top(5);

    CHECK(t.n_bid == 1);
    CHECK(t.n_ask == 1);

    // slots past the count must be untouched
    CHECK(t.bid[1].price == 0);  CHECK(t.bid[1].size == 0);
    CHECK(t.ask[4].price == 0);  CHECK(t.ask[4].size == 0);
}

TEST_CASE("more levels than requested truncates and reports the truncated count") {
    OrderBook b;
    for (int i = 0; i < 8; ++i) {
        b.New(1000 + i, 100, 2650 - i, 1);
        b.New(2000 + i, 100, 2651 + i, -1);
    }

    TopOfBook t = b.top(5);

    CHECK(t.n_bid == 5);              // not 8
    CHECK(t.n_ask == 5);
    CHECK(t.bid[0].price == 2650);
    CHECK(t.bid[4].price == 2646);
    CHECK(t.ask[0].price == 2651);
    CHECK(t.ask[4].price == 2655);

    // the sixth-best exists in the book but must not have been written
    CHECK(t.bid[5].price == 0);
    CHECK(t.ask[5].price == 0);
}

TEST_CASE("one-sided book") {
    OrderBook b;
    b.New(1001, 500, 2650, 1);
    b.New(1002, 300, 2649, 1);

    TopOfBook t = b.top(5);
    CHECK(t.n_bid == 2);
    CHECK(t.n_ask == 0);
    CHECK(is_sane(t));                // no ask side means nothing to cross
}

TEST_CASE("empty book") {
    OrderBook b;
    TopOfBook t = b.top(5);
    CHECK(t.n_bid == 0);
    CHECK(t.n_ask == 0);
    CHECK(is_sane(t));
}

TEST_CASE("seed reproduces the snapshot it came from") {
    TopOfBook snap{};
    snap.bid[0] = {2650, 500};
    snap.bid[1] = {2649, 300};
    snap.n_bid  = 2;
    snap.ask[0] = {2651, 400};
    snap.ask[1] = {2652, 200};
    snap.n_ask  = 2;

    OrderBook b;
    b.seed(snap);

    TopOfBook t = b.top(5);
    CHECK(t.n_bid == 2);
    CHECK(t.n_ask == 2);
    CHECK(t.bid[0].price == 2650);  CHECK(t.bid[0].size == 500);
    CHECK(t.ask[1].price == 2652);  CHECK(t.ask[1].size == 200);
}