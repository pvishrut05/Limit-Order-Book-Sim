

#include <string>
using namespace std;

struct NormalizedMessage {
    uint64_t timestamp_ns;
    uint8_t type;
    uint64_t order_id;
    uint32_t size;
    uint64_t price;
    uint8_t side;
};


