#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>
#include <unistd.h>

#define REGISTER_OK "register ok"
#define REGISTER_FAILED "register failed: username exists"
#define LOGIN_OK "login successful"
#define LOGIN_FAILED "login failed: username or password error, or already logined"
#define SEARCH_USER_NO "no such user"
#define SEARCH_USER_ONLINE "user is online"
#define SEARCH_USER_OFFLINE "user is offline"
#define UNKNOWN_ERROR "unknown error"
#define ADD_FRIEND_ALREADY_FRIENDS "Already friends"
#define ADD_FRIEND_OFFLINE "user offline"
#define ADD_FRIEND_NOT_EXIST "user not exist"
#define DELETE_OK "removed successfully"

typedef unsigned int uint;

struct PDU
{
    uint uiPDULen; //total length of the PDU
    uint uiMsgType; // message type
    char caData[64]; // file name when uplaoding or downloading
    uint uiMsgLen; // length of the actual message
    int caMsg[]; // actual message
};

enum ENUM_MSG_TYPE{
    ENUM_MSG_TYPE_MIN=0,
    ENUM_MSG_TYPE_REGISTER_REQUEST,
    ENUM_MSG_TYPE_REGISTER_RESPONSE,
    ENUM_MSG_TYPE_LOGIN_REQUEST,
    ENUM_MSG_TYPE_LOGIN_RESPONSE,
    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST, // request all online users
    ENUM_MSG_TYPE_ALL_ONLINE_RESPONSE,
    ENUM_MSG_TYPE_SEARCH_USER_REQUEST, // search user
    ENUM_MSG_TYPE_SEARCH_USER_RESPONSE,
    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST, // add friend in online widget
    ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE,
    ENUM_MSG_TYPE_ADD_FRIEND_ACCEPT, // accpet
    ENUM_MSG_TYPE_ADD_FRIEND_REJECT, // reject
    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST, // flush friend list request
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPONSE,
    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST, // delete friend request
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPONSE,
    ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST, // group chat request
    ENUM_MSG_TYPE_PRIVATE_CHAT_RESPONSE,
    ENUM_MSG_TYPE_GROUP_CHAT_REQUEST, // private chat request
    ENUM_MSG_TYPE_GROUP_CHAT_RESPONSE,
    ENUM_MSG_TYPE_MAX=0x00ffffff
};

PDU *mkPDU(uint uiMsgLen);

#endif // PROTOCOL_H
