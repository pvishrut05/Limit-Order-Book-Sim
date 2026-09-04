#include "IsSane.h"


bool is_sane(const TopOfBook& b){
    for(int i = 0; i < b.n_ask; i++){
        if(b.ask[i].size == 0) return false;
        if( i > 0 && b.ask[i].price <= b.ask[i - 1].price) return false;
    } 

    for(int i = 0; i < b.n_bid; i++){
        if(b.bid[i].size == 0) return false;
        if( i > 0 && b.bid[i].price >= b.bid[i - 1].price) return false;
    }

    if(b.n_ask > 0 && b.n_bid > 0 && b.ask[0].price <= b.bid[0].price) return false;

    return true;

}