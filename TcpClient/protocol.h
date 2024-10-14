#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef unsigned int uint;

struct PDU
{
    uint uiPDULen; //total length of the PDU
    uint uiMsgType; // message type
    char caData[64];
    uint uiMsgLen; // length of the actual message
    int caMsg[]; // actual message
};

PDU *mkPDU(uint uiMsgLen);

#endif // PROTOCOL_H
