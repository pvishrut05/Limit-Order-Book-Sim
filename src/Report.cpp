#include "Report.h"
#include "MsgType.h"
#include <ostream>
#include <iomanip>
#include <unordered_set>
#include <algorithm>


//void print_mismatch(ostream& os, NormalizedMessage& nm, DiffResult& d, TopOfBook& mine, TopOfBook& ref, int lvl); 

std::string hhmmss(uint64_t ts_ns) {
    uint64_t s = ts_ns / 1'000'000'000ULL;
    char buf[16];
    std::snprintf(buf, sizeof buf, "%02llu:%02llu:%02llu",
                  static_cast<unsigned long long>(s / 3600),
                  static_cast<unsigned long long>((s / 60) % 60),
                  static_cast<unsigned long long>(s % 60));
    return buf;
}

std::string dollars(int64_t ticks) {
    char buf[32];
    std::snprintf(buf, sizeof buf, "%.2f", static_cast<double>(ticks) / 10000.0);
    return buf;
}

void print_side_by_side(std::ostream& os, const TopOfBook& mine,
                        const TopOfBook& ref, int levels) {
    os << "\n         " << std::setw(28) << "MINE"
       << "   |   " << std::setw(28) << "REFERENCE" << "\n";
    os << "  lvl  " << std::setw(12) << "ask" << std::setw(8) << "sz"
       << std::setw(12) << "bid" << std::setw(8) << "sz"
       << "  | " << std::setw(12) << "ask" << std::setw(8) << "sz"
       << std::setw(12) << "bid" << std::setw(8) << "sz" << "\n";

    for (int i = 0; i < levels; ++i) {
        os << "  " << std::setw(3) << (i + 1) << "  ";
        auto cell = [&](const TopOfBook& b) {
            if (i < b.n_ask)
                os << std::setw(12) << (b.ask[i].price)
                   << std::setw(8)  << b.ask[i].size;
            else
                os << std::setw(12) << "--" << std::setw(8) << "--";
            
            if (i < b.n_bid)
                os << std::setw(12) << (b.bid[i].price)
                   << std::setw(8)  << b.bid[i].size;
            else
                os << std::setw(12) << "--" << std::setw(8) << "--";
            
        };
        cell(mine);
        os << "  | ";
        cell(ref);
        os << "\n";
    }
    os << "  n_ask " << static_cast<int>(mine.n_ask)
       << " n_bid " << static_cast<int>(mine.n_bid)
       << "   |   n_ask " << static_cast<int>(ref.n_ask)
       << " n_bid " << static_cast<int>(ref.n_bid) << "\n";
}


void print_histogram(ostream& os, vector<NormalizedMessage>& msgs, size_t ref_row){
    size_t counts[8] = {};
    uint64_t min_px = std::numeric_limits<uint64_t>::max();
    uint64_t max_px = std::numeric_limits<uint64_t>::min();
    std::unordered_set<uint64_t> add_ids;

    for (const NormalizedMessage& m : msgs) {
        if (m.type >= 1 && m.type <= 7) ++counts[m.type];
        if (m.price > 0) {
            min_px = min(min_px, m.price);
            max_px = max(max_px, m.price);
        }
        if (static_cast<MsgType>(m.type) == MsgType::New) add_ids.insert(m.order_id);
    }

    os << "=== load summary ===\n";
    os << "  messages        " << msgs.size() << "\n";
    os << "  reference rows  " << ref_row
       << (msgs.size() == ref_row ? "  (match)\n" : "  *** MISMATCH ***\n");

    if (!msgs.empty()) {
        os << "  first ts        " << hhmmss(msgs.front().timestamp_ns) << "\n";
        os << "  last ts         " << hhmmss(msgs.back().timestamp_ns)  << "\n";
        os << "  price range     $" << dollars(min_px)
           << " .. $" << dollars(max_px) << "\n";
    }
    os << "  distinct add ids " << add_ids.size() << "\n";

    os << "\n=== message types ===\n";
    for (int t = 1; t <= 7; ++t) {
        double pct = msgs.empty() ? 0.0
                   : 100.0 * static_cast<double>(counts[t]) / static_cast<double>(msgs.size());
        os << "  " << std::left << std::setw(13)
           << msgToChar(static_cast<MsgType>(t)) << std::right
           << std::setw(10) << counts[t]
           << std::setw(8)  << std::fixed << std::setprecision(2) << pct << "%\n";
    }

    size_t leave = counts[2] + counts[3];
    if (counts[4] > 0){
        os << "\n  (reduce+delete) / execute = "
           << std::fixed << std::setprecision(1)
           << static_cast<double>(leave) / static_cast<double>(counts[4]) << "\n";
        os << "\n";
    }

}



void print_mismatch(std::ostream& os, size_t event_idx, const NormalizedMessage& m, const DiffResult& d, const TopOfBook& mine, const TopOfBook& ref, int levels) {
    const char* field = d.field == Field::Occupancy ? "OCCUPANCY"
                      : d.field == Field::Price     ? "PRICE" : "SIZE";
    const char* side  = d.side  == BookSide::Bid ? "BID" : "ASK";

    os << "\n*** MISMATCH at event " << event_idx << " ***\n";
    os << "  message   " << msgToChar(static_cast<MsgType>(m.type))
       << "  id " << m.order_id
       << "  " << (m.side == 1 ? 'B' : 'S')
       << "  $" << dollars(m.price) << " (" << m.price << " ticks)"
       << "  x" << m.size
       << "  @ " << hhmmss(m.timestamp_ns) << "\n";
    os << "  field     " << side << " " << field;
    if (d.field != Field::Occupancy) os << " level " << d.level;
    os << "\n";
    os << "  expected  " << d.expected << "\n";
    os << "  actual    " << d.actual   << "\n";

    print_side_by_side(os, mine, ref, levels);
}
