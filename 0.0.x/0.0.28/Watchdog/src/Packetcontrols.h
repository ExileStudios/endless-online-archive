#ifndef PacketcontrolsH
#define PacketcontrolsH

#include <Classes.hpp>
#include <SysUtils.hpp>
#include <ScktComp.hpp>
#include <DBTables.hpp>
#include <string.h>
#include <stdio.h>
#include <vector.h>
#include <deque.h>
#include <list.h>
#include <stack.h>
#include <queue.h>

// The bytes of the server's init reply: 255,255 and then the code.
enum InitReply
{
    InitReply_OutOfDate = 1,
    InitReply_Ok = 2,
    InitReply_OnlineList = 12,
    InitReply_RawPacket = 255
};

enum PacketAction
{
    AC_REQUEST = 1,
    AC_CONFIRM = 2,
    AC_RESULT = 3,
    AC_SET = 8,
    AC_LIST = 18,
    AC_SYN1 = 240
};

enum PacketFamily
{
    OP_SECURITY = 1,
    OP_PLAYERS = 22,
    OP_SYSTEM = 35
};
#define EO_NUMBER_BASE 253
#define EO_NUMBER_BASE_CUBED 16194277
#define EO_NUMBER_BASE_SQUARED 64009
#define EO_NUMBER_EMPTY 0xFE  // pads a number to its width, and ends it when read

struct PacketOperator;
struct PacketProcessor;


struct Client;

// The Watchdog's connection to the game server: the init handshake, the packet encryption and a field reader.
struct Client
{
    bool out_of_date;
    bool initialized;
    bool unused_flag2;
    bool unused_flag3;
    bool unused_byte4;
    bool unused_flag5;
    int unused_mode;
    int time_offset;
    int date_offset;
    bool unused_byte20;
    int unused_value24;
    bool unused_byte28;
    int challenge_response;
    int challenge;
    PacketOperator *packet_operator;
    PacketProcessor *packet_processor;
    String reader_data;
    int reader_pos;
    int reader_len;
    char reader_break_byte;
    String packet_data;
    int packet_action;
    int packet_family;
    char pad_0x4c[4];
    int sequence_start;
    int sequence_counter;
    int server_encryption_multiple;
    int client_encryption_multiple;
    int player_id;
    int unused_value100;
    int unused_value104;
    TStringList *packet_list;
    String pending_data;

    Client();
    ~Client();
    void SendInit();
    void SetPlayerID(int id);
    bool HandleInit(String data);
    void HandleRead(String address, int socket_handle, String data);
    void SplitPackets(String data);
    String EncodeNumber(unsigned int value, int width);
    int DecodeNumber(String data);
    String EncodePacket(int multiple, String data);
    String DecodePacket(int multiple, String data);
    String EncodeInterleave(int multiple, char *begin, char *end);
    String DecodeDeinterleave(int multiple, char *begin, char *end);
    String FlipMSB(String data);
    bool SendPacket(int action, int family, String data);
    void ReaderInit(String data, char break_byte);
    String ReaderGetBreakString();
    void SetSequenceAndEncryption(int sequence, int server_multiple, int client_multiple);
    void SetSequence(int sequence);
    String BuildPacket(int multiple, int action, int family, String data);
    void ReadPacket(String packet);
    int GetFamily();
    int GetAction();
    String GetData();
    int DecodeByte(unsigned char value);
    void Unused();
};

// Shared body: a class defines its DecodeNumber with DEFINE_DECODE_NUMBER(TheClass).
#define DEFINE_DECODE_NUMBER(Class) \
int Class::DecodeNumber(String data) \
{ \
    int result = 0; \
    try \
    { \
        for (int i = 1; i <= data.Length(); i++) \
        { \
            char c = data[i]; \
            unsigned char ch = c; \
            if (ch == EO_NUMBER_EMPTY) \
                break; \
            int digit = ch; \
            digit--; \
            if (i == 1) \
                result += digit; \
            if (i == 2) \
                result += EO_NUMBER_BASE * digit; \
            if (i == 3) \
                result += EO_NUMBER_BASE_SQUARED * digit; \
            if (i == 4) \
                result += EO_NUMBER_BASE_CUBED * digit; \
        } \
    } \
    catch (...) \
    { \
        result = 0; \
    } \
    return result; \
}

#endif
