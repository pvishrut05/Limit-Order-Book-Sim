// tests/test_order_book_remove.cpp
#include "doctest.h"
#include "OrderBook.h"

static OrderBook seeded_two_orders() {
    OrderBook b;
    b.New(1001, 500, 2650, 1);   // bid
    b.New(1002, 300, 2650, 1);   // same level
    b.New(2001, 400, 2651, -1);  // ask
    return b;
}

TEST_CASE("execute against a known id") {
    OrderBook b = seeded_two_orders();
    b.Execute(2001, 150, 2651, -1);

    TopOfBook t = b.top(5);
    CHECK(t.n_ask == 1);
    CHECK(t.ask[0].size == 250);
}

TEST_CASE("delete empties a level when it was the only order") {
    OrderBook b;
    b.New(1001, 500, 2650, 1);
    b.New(1002, 300, 2649, 1);

    b.Delete(1001, 500, 2650, 1);

    TopOfBook t = b.top(5);
    CHECK(t.n_bid == 1);
    CHECK(t.bid[0].price == 2649);   // level erased, not left at zero
    CHECK(t.bid[0].size  == 300);
}

TEST_CASE("delete leaves the level when other orders remain") {
    OrderBook b = seeded_two_orders();
    b.Delete(1001, 500, 2650, 1);

    TopOfBook t = b.top(5);
    CHECK(t.n_bid == 1);
    CHECK(t.bid[0].price == 2650);
    CHECK(t.bid[0].size  == 300);
}

TEST_CASE("reduce subtracts the amount, not the remainder") {
    OrderBook b;
    b.New(1001, 500, 2650, 1);
    b.Cancel(1001, 200, 2650, 1);    // remove 200, leaving 300

    TopOfBook t = b.top(5);
    CHECK(t.bid[0].size == 300);     // 100 would mean it was read as "new size"
}

TEST_CASE("reduce to zero erases the level") {
    OrderBook b;
    b.New(1001, 500, 2650, 1);
    b.New(1002, 300, 2649, 1);
    b.Cancel(1001, 500, 2650, 1);

    TopOfBook t = b.top(5);
    CHECK(t.n_bid == 1);
    CHECK(t.bid[0].price == 2649);
}



TEST_CASE("execute consuming the whole order erases the level") {
    OrderBook b;
    b.New(2001, 400, 2651, -1);
    b.Execute(2001, 400, 2651, -1);

    TopOfBook t = b.top(5);
    CHECK(t.n_ask == 0);
}

TEST_CASE("unknown id falls back to the message and counts") {
    OrderBook b;

    TopOfBook snap{};
    snap.bid[0] = {2650, 1000};
    snap.n_bid  = 1;
    snap.ask[0] = {2651, 800};
    snap.n_ask  = 1;
    b.seed(snap);                     // anonymous depth, orders_ is empty

    CHECK(b.unknown_id_hits() == 0);

    b.Delete(999999, 400, 2650, 1);   // an id we never saw
    CHECK(b.unknown_id_hits() == 1);

    TopOfBook t = b.top(5);
    CHECK(t.bid[0].size == 600);

    b.Cancel(888888, 300, 2650, 1);
    b.Execute(777777, 200, 2651, -1);
    CHECK(b.unknown_id_hits() == 3);

    t = b.top(5);
    CHECK(t.bid[0].size == 300);
    CHECK(t.ask[0].size == 600);
}

TEST_CASE("unknown-id delete that empties seeded depth") {
    OrderBook b;
    TopOfBook snap{};
    snap.bid[0] = {2650, 500};
    snap.bid[1] = {2649, 300};
    snap.n_bid  = 2;
    b.seed(snap);

    b.Delete(999999, 500, 2650, 1);

    TopOfBook t = b.top(5);
    CHECK(t.n_bid == 1);
    CHECK(t.bid[0].price == 2649);
}

// TEST_CASE("orders_ and the price map stay consistent") {
//     OrderBook b = seeded_two_orders();
//     CHECK(b.order_count() == 3);

//     b.Cancel(1001, 200, 2650, 1);     // partial: order survives
//     CHECK(b.order_count() == 3);

//     b.Cancel(1001, 300, 2650, 1);     // remainder: order gone
//     CHECK(b.order_count() == 2);
// }