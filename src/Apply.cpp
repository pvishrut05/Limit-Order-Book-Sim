#include "Apply.h"
#include "MsgType.h"


void apply(OrderBook& book, NormalizedMessage& m){
    switch(static_cast<MsgType>(m.type)){
        case MsgType::New: 
            book.New(m.order_id, m.size, m.price, m.side);
            break;
        case MsgType::Cancel: 
            book.Cancel(m.order_id, m.size, m.price, m.side);
            break;
        case MsgType::Delete: 
            book.Delete(m.order_id, m.size, m.price, m.side);
            break;
        case MsgType::Execute: 
            book.Execute(m.order_id, m.size, m.price, m.side);
            break;
        case MsgType::Hidden:
            //No need to work on these, as hidden doesn't change anything within the book, cross is getting something outside, and halt just changes the state doesn't effect the book
            break;
        case MsgType::Cross:
            //No need to work on these
            break;
        case MsgType::Halt:
            //No need to work on these
            break;
    }
}