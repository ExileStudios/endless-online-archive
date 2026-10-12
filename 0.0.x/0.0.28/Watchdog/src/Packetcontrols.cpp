#include <vcl.h>
#pragma hdrstop

#include "Packetcontrols.h"
#include "Packetoperations.h"
#include "Packetproces.h"
#include "Tmainform.h"

#pragma package(smart_init)

#include <stdlib.h>
#include "Constants.h"

// Creates the packet helpers and resets the connection state.
Client::Client()
{
    packet_operator = new PacketOperator();
    packet_processor = new PacketProcessor();
    packet_list = new TStringList();
    client_encryption_multiple = 0;
    server_encryption_multiple = 0;
    sequence_start = 0;
    player_id = 0;
    unused_value100 = 0;
    out_of_date = false;
    initialized = false;
    unused_flag2 = false;
    unused_flag3 = false;
    unused_byte4 = false;
    unused_flag5 = false;
    unused_mode = 1;
    unused_value24 = 0;
    unused_byte28 = false;
    unused_byte20 = false;
    time_offset = -1;
    date_offset = -1;
    unused_value104 = 0;
}

Client::~Client()
{
}

// Sends the init packet that opens the connection to the game server.
void Client::SendInit()
{
    try
    {
        randomize();
    }
    catch (...)
    {
    }
    try
    {
        DWORD VolumeSerialNumber;
        DWORD MaximumComponentLength;
        DWORD FileSystemFlags;
        char VolumeNameBuffer[1002];
        char FileSystemNameBuffer[VOLUME_INFO_BUFFER_SIZE];
        GetVolumeInformationA("c:\\", VolumeNameBuffer, 1000, &VolumeSerialNumber, &MaximumComponentLength,
            &FileSystemFlags, FileSystemNameBuffer, 1000);
        // the drive serial goes along as the hdid, 32 digits at most
        String hdid = VolumeSerialNumber;
        if (hdid.Length() > 32)
            hdid = hdid.SubString(1, 32);
        // the init header is 255,255
        char init_family = -1;
        char init_action = init_family;
        challenge = random(1000000);
        String init_packet = "";
        init_packet = init_packet + (char)init_action;
        init_packet = init_packet + (char)init_action;
        init_packet = init_packet + EncodeNumber(challenge, 3);
        init_packet = init_packet + EncodeNumber(MainForm->version_major, 1);
        init_packet = init_packet + EncodeNumber(MainForm->version_minor, 1);
        init_packet = init_packet + EncodeNumber(MainForm->version_patch, 1);
        init_packet = init_packet + EncodeNumber(112, 1);
        init_packet = init_packet + EncodeNumber(hdid.Length(), 1);
        init_packet = init_packet + hdid;
        // every packet starts with its length in two bytes
        init_packet = EncodeNumber(init_packet.Length(), 2) + init_packet;
        sequence_counter = 0;
        MainForm->client->Socket->SendText(init_packet);
    }
    catch (...)
    {
    }
}

void Client::SetPlayerID(int id)
{
    player_id = id;
}

// Reads the server's answer to the init packet; true when the packet was an init reply ( it starts 255,255 ).
bool Client::HandleInit(String data)
{
    bool is_init = 0;
    if (data.Length() >= 2)
    {
        try
        {
            if (DecodeByte((unsigned char)data[1]) == InitReply_RawPacket)
            {
                if (DecodeByte((unsigned char)data[2]) == InitReply_RawPacket)
                {
                    is_init = 1;
                    if (data.Length() == 3)
                    {
                        if (DecodeByte((unsigned char)data[3]) == InitReply_OutOfDate)
                            out_of_date = true;
                    }
                    if (data.Length() >= 12)
                    {
                        // .. ok: the sequence start, both encryption multiples, the challenge answer and the player id
                        if (DecodeByte((unsigned char)data[3]) == InitReply_Ok)
                        {
                            int seq1 = DecodeByte((unsigned char)data[4]);
                            int seq2 = DecodeByte((unsigned char)data[5]);
                            int seq_start = seq1 * 7 + seq2;
                            seq_start = seq_start - 13;
                            SetSequenceAndEncryption(seq_start, DecodeByte((unsigned char)data[6]),
                                DecodeByte((unsigned char)data[7]));
                            String player_id_bytes = data.SubString(8, 2);
                            challenge_response = DecodeNumber(data.SubString(10, 3));
                            SetPlayerID(DecodeNumber(player_id_bytes));
                            String accept_data = EncodeNumber(client_encryption_multiple, 2);
                            accept_data = accept_data + EncodeNumber(server_encryption_multiple, 2);
                            accept_data = accept_data + EncodeNumber(player_id, 2);
                            initialized = true;
                            Unused();
                            // accept the connection: both encryption multiples and the player id go back to the server
                            SendPacket(AC_CONFIRM, OP_SECURITY, accept_data);
                        }
                    }
                    if (data.Length() >= 3)
                    {
                        // .. the online list can come back raw, without encryption, as a Players List packet
                        if (DecodeByte((unsigned char)data[3]) == InitReply_OnlineList)
                            packet_processor->HandlePacket(AC_LIST, OP_PLAYERS,
                                data.SubString(4, data.Length() - 3));
                    }
                }
            }
        }
        catch (...)
        {
        }
    }
    return is_init;
}

// Handles what the socket received: splits it into packets and passes on every one that is not an init reply.
void Client::HandleRead(String address, int socket_handle, String data)
{
    SplitPackets(data);
    try
    {
        for (int i = 0; i < packet_list->Count; i++)
        {
            String packet = packet_list->Strings[i];
            if (!HandleInit(packet))
            {
                ReadPacket(packet);
                packet_processor->HandlePacket(GetAction(), GetFamily(), GetData());
            }
        }
        packet_list->Clear();
    }
    catch (...)
    {
        packet_list->Clear();
    }
}

// Cuts the received data into packets by their two-byte length; an unfinished packet waits in pending_data.
void Client::SplitPackets(String data)
{
    try
    {
        packet_list->Clear();
        if (pending_data != "")
        {
            data = pending_data + data;
            pending_data = "";
        }
        do
        {
            if (data.Length() >= 3)
            {
                String length_prefix = data[1];
                length_prefix = length_prefix + data[2];
                int packet_end = DecodeNumber(length_prefix) + 2;
                if (data.Length() >= packet_end)
                {
                    String packet = data.SubString(3, packet_end - 2);
                    data = data.SubString(packet_end + 1, data.Length());
                    packet_list->Add(packet);
                }
                else
                {
                    if (data.Length() < packet_end - 2)
                        pending_data = data;
                    data = "";
                }
            }
            else
                data = "";
        } while (data.Length() >= 3);
    }
    catch (...)
    {
        packet_list->Clear();
    }
}

// Writes a number the protocol's way: base 253, lowest digit first, each digit plus one, padded with 254.
String Client::EncodeNumber(unsigned int value, int width)
{
    unsigned int remaining = value;
    String result = "";
    try
    {
        unsigned int quotient;
        do
        {
            int rem;
            char digit;
            double scaled = remaining * 0.00395256916996047430813L;  // 1 / 253
            quotient = scaled;
            rem = remaining % 253;
            digit = rem + 1;
            result.Insert(digit, result.Length() + 1);
            if (quotient >= 1)
                remaining = quotient;
        } while (quotient >= 1);
    }
    catch (...)
    {
        result = "";
    }
    if (result.Length() < width)
    {
        char pad = -2;
        int pad_count = width - result.Length();
        for (int i = 0; i < pad_count; i++)
            result.Insert(pad, result.Length() + 1);
    }
    return result;
}

// Reads a number of up to four bytes, the reverse of EncodeNumber.
DEFINE_DECODE_NUMBER(Client)

// Encrypts a packet for the server: interleaves its bytes by the multiple, then flips their top bits.
String Client::EncodePacket(int multiple, String data)
{
    String encoded = "";
    std::string handle(data.c_str());
    encoded = EncodeInterleave(multiple, handle.begin(), handle.end());
    encoded = FlipMSB(encoded);
    return encoded;
}

// Decrypts a packet from the server, the reverse of EncodePacket.
String Client::DecodePacket(int multiple, String data)
{
    String decoded = "";
    data = FlipMSB(data);
    std::string handle(data.c_str());
    decoded = DecodeDeinterleave(multiple, handle.begin(), handle.end());
    return decoded;
}

// Reverses every run of bytes the multiple divides, then weaves the bytes from both ends ( first, last, second, .. ).
String Client::EncodeInterleave(int multiple, char *begin, char *end)
{
    int divisor;
    String result = "";
    divisor = multiple;
    try
    {
        std::stack<char > pending;
        std::deque<char > woven;
        for (; begin != end; begin++)
        {
            unsigned char input_byte = *begin;
            int char_value = input_byte;
            if (!(char_value % divisor))
                pending.push(*begin);
            else
            {
                while (!pending.empty())
                {
                    woven.push_back(pending.top());
                    pending.pop();
                }
                woven.push_back(*begin);
            }
        }
        while (!pending.empty())
        {
            woven.push_back(pending.top());
            pending.pop();
        }
        while (!woven.empty())
        {
            if (!woven.empty())
            {
                result.Insert((char)woven.front(), result.Length() + 1);
                woven.pop_front();
            }
            if (!woven.empty())
            {
                result.Insert((char)woven.back(), result.Length() + 1);
                woven.pop_back();
            }
        }
    }
    catch (...)
    {
        result = "";
    }
    return result;
}

// Undoes EncodeInterleave: unweaves the bytes, then turns the runs the multiple divides back around.
String Client::DecodeDeinterleave(int multiple, char *begin, char *end)
{
    int divisor;
    String result = "";
    divisor = multiple;
    try
    {
        std::stack<char > pending;
        std::queue<char > woven;
        char toggle = 1;
        for (; begin != end; begin++)
        {
            if (toggle)
            {
                woven.push(*begin);
                toggle = 0;
            }
            else
            {
                pending.push(*begin);
                toggle = 1;
            }
        }
        while (!pending.empty())
        {
            woven.push(pending.top());
            pending.pop();
        }
        pending.empty();
        while (!woven.empty())
        {
            char next_char = woven.front();
            woven.pop();
            unsigned char raw = next_char;
            int raw_value = raw;
            if (!(raw_value % divisor))
                pending.push(next_char);
            else
            {
                while (!pending.empty())
                {
                    result.Insert((char)pending.top(), result.Length() + 1);
                    pending.pop();
                }
                result.Insert((char)next_char, result.Length() + 1);
            }
        }
        while (!pending.empty())
        {
            result.Insert((char)pending.top(), result.Length() + 1);
            pending.pop();
        }
    }
    catch (...)
    {
        result = "";
    }
    return result;
}

// Flips the top bit of every byte; a 128 stays as it is.
String Client::FlipMSB(String data)
{
    if (data.Length() >= 1)
    {
        for (int i = 1; i <= data.Length(); i++)
        {
            int value = (unsigned char)data[i];
            if (value < 128)
                data[i] += (char)128;
            if (value > 128)
                data[i] += (char)128;
        }
    }
    return data;
}

void Packetcontrols_UnusedLiterals1()
{
    "";
    "";
}

// Builds a packet and sends it while the socket is open.
bool Client::SendPacket(int action, int family, String data)
{
    int result = 0;
    try
    {
        if (MainForm->client->Active)
        {
            String packet = BuildPacket(client_encryption_multiple, action, family, data);
            if (packet != "")
                MainForm->client->Socket->SendText(packet);
        }
    }
    catch (...)
    {
        result = 0;
    }
    return result;
}

void Client::Unused()
{

}

// Starts reading a packet's data field by field, each field ending at the break byte.
void Client::ReaderInit(String data, char break_byte)
{
    reader_pos = 1;
    reader_data = data;
    reader_len = data.Length();
    reader_break_byte = break_byte;
}

void Packetcontrols_UnusedLiterals2()
{
    "";
    "";
}

// The next field of the packet being read; the break byte after it is skipped.
String Client::ReaderGetBreakString()
{
    String token = "";
    try
    {
        if (reader_len >= 1)
        {
            while (reader_pos <= reader_len)
            {
                if (reader_data[reader_pos] != reader_break_byte)
                    token.Insert(reader_data[reader_pos], token.Length() + 1);
                else
                {
                    reader_pos++;
                    break;
                }
                reader_pos++;
            }
        }
    }
    catch (...)
    {
        token = "";
    }
    return token;
}

void Client::SetSequenceAndEncryption(int sequence, int server_multiple, int client_multiple)
{
    sequence_start = sequence;
    server_encryption_multiple = server_multiple;
    client_encryption_multiple = client_multiple;
}

void Client::SetSequence(int sequence)
{
    sequence_start = sequence;
}

void Packetcontrols_UnusedLiterals3()
{
    "";
    "";
    "";
}

// Builds an encrypted packet: action, family, the sequence byte and the data, with its length in front.
String Client::BuildPacket(int multiple, int action, int family, String data)
{
    String packet = "";
    try
    {
        char action_byte;
        char family_byte;
        char action_code = action;
        char family_code = family;
        char valid = 1;
        if (action_code != 0)
            action_byte = action_code;
        else
            valid = 0;
        if (family_code != 0)
            family_byte = family_code;
        else
            valid = 0;
        // .. an action or a family of 0 builds nothing
        if (valid)
        {
            String header = "";
            String payload = "";
            header = header + (char)action_byte;
            header = header + (char)family_byte;
            payload = data;
            packet = packet + header;
            // the sequence byte counts 0 to 9 on top of sequence_start
            sequence_counter++;
            if (sequence_counter > 9)
                sequence_counter = 0;
            packet = packet + EncodeNumber(sequence_start + sequence_counter, 1);
            packet = packet + payload;
            packet = EncodePacket(multiple, packet);
            int packet_length = packet.Length();
            packet = EncodeNumber(packet_length, 2) + packet;
        }
    }
    catch (...)
    {
        sequence_counter--;
        packet = "";
    }
    return packet;
}

// Decrypts a packet from the server and keeps its action, family and data for GetAction, GetFamily and GetData.
void Client::ReadPacket(String packet)
{
    int action;
    int family;
    if (packet.Length() >= 3)
    {
        String decoded = DecodePacket(server_encryption_multiple, packet);
        packet_action = 250;
        packet_family = 250;
        action = 0;
        family = 0;
        try
        {
            action = DecodeByte((unsigned char)decoded[1]);
            family = DecodeByte((unsigned char)decoded[2]);
        }
        catch (...)
        {
        }
        packet_action = action;
        packet_family = family;
        packet_data = decoded.SubString(3, decoded.Length());
    }
}

int Client::GetFamily()
{
    return packet_family;
}

int Client::GetAction()
{
    return packet_action;
}

String Client::GetData()
{
    return packet_data;
}

int Client::DecodeByte(unsigned char value)
{
    unsigned char byte_value = value;
    int result = byte_value;
    return result;
}
