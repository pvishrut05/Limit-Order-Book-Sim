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

inline const char* msgToChar(MsgType type){
    switch (type){
        case MsgType::New:      return "New";
        case MsgType::Cancel:   return "Cancel";
        case MsgType::Delete:   return "Delete";
        case MsgType::Execute:  return "Execute";
        case MsgType::Hidden:   return "Hidden";
        case MsgType::Cross:    return "Cross";
        case MsgType::Halt:     return "Halt";
    }
    return "Error processing msgToChar";
}

#endif
