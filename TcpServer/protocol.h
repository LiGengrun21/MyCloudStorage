#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define REGISTER_OK "register ok"
#define REGISTER_FAILED "register failed: username exists"
#define LOGIN_OK "login successful"
#define LOGIN_FAILED "login failed: username or password error, or already logined"

typedef unsigned int uint;

struct PDU
{
    uint uiPDULen; //total length of the PDU
    uint uiMsgType; // message type
    char caData[64];
    uint uiMsgLen; // length of the actual message
    int caMsg[]; // actual message
};

enum ENUM_MSG_TYPE{
    ENUM_MSG_TYPE_MIN=0,
    ENUM_MSG_TYPE_REGISTER_REQUEST,
    ENUM_MSG_TYPE_REGISTER_RESPONSE,
    ENUM_MSG_TYPE_LOGIN_REQUEST,
    ENUM_MSG_TYPE_LOGIN_RESPONSE,
    ENUM_MSG_TYPE_MAX=0x00ffffff
};


PDU *mkPDU(uint uiMsgLen);

#endif // PROTOCOL_H
