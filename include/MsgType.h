#ifndef MSGTYPE_H
#define MSGTYPE_H


#include <cstdint>
#include <cstdio>
enum class MsgType{New = 1, Cancel = 2, Delete = 3, Execute = 4, Hidden = 5, Cross = 6, Halt = 7};

inline MsgType convertMsgType(uint8_t type){
    if(type < 1 || type > 7){
        fprintf(stderr, "Wrong type of %llu", static_cast<unsigned long long>(type));
    }

    return static_cast<MsgType>(type);
}


#endif
