#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Ctrl_menu.h"
#include "Packetoperations.h"
#include "Packetproces.h"
#include "Ctrl_credits.h"
#include "Ctrl_game.h"
#include <stdlib.h>

#pragma package(smart_init)

// Creates the packet sender and processor, clears the connection state and loads the chat word filter.
Client::Client()
{
    sender = new PacketOperator();
    processor = new PacketProcessor();
    spare_string_list = new TStringList();
    send_key = 0;
    receive_key = 0;
    sequence_start = 0;
    player_id = 0;
    session_id = 0;
    init_rejected = false;
    is_initialized = false;
    is_banned = false;
    account_reply_disconnect = false;
    login_retry_pending = false;
    lookups_refused = false;
    server_busy = false;
    is_permanent_ban = false;
    ban_minutes = 1;
    login_failure_count = 0;
    version_rejected = false;
    unused_flag = false;
    time_offset = -1;
    date_offset = -1;
    last_action_tick = 9000000;
    admin_level = 0;
    word_filter = new TStringList();
    LoadWordFilter();
}

Client::~Client()
{
}

// Reads the word filter data file, unscrambles it and splits it into the filter words.
void Client::LoadWordFilter()
{
    int file_handle;
    int file_size;
    char *buffer;
    String str;
    try
    {
        file_handle = FileOpen(MainForm->files->GetDataFilePath(DataFile_WordFilter), fmOpenRead);
        file_size = FileSeek(file_handle, 0, 2);
        FileSeek(file_handle, 0, 0);
        buffer = new char[file_size + 1];
        FileRead(file_handle, buffer, file_size);
        FileClose(file_handle);
        str = buffer;
        str.SetLength(file_size);
        delete[] buffer;
    }
    catch (...)
    {
    }
    string file_bytes = str.c_str();
    str = UnscrambleBytes(26, file_bytes.begin(), file_bytes.end());
    String filter_word;
    BeginTokens(str, 58);
    do
    {
        filter_word = NextToken();
        if (filter_word.Length() >= 1)
            word_filter->Add(filter_word);
    } while (filter_word.Length() >= 1);
}

// Sends the first packet of a connection, unencrypted: a random challenge, the client version and the serial
// number of drive C.
void Client::SendInitRequest()
{
    unsigned long volume_serial_number;
    unsigned long maximum_component_length;
    unsigned long file_system_flags;
    char volume_name_buffer[VOLUME_NAME_BUFFER_SIZE];
    char file_system_name_buffer[FILE_SYSTEM_NAME_BUFFER_SIZE];
    try
    {
        randomize();
    }
    catch (...)
    {
    }
    try
    {
        GetVolumeInformationA("c:\\", volume_name_buffer, 1000, &volume_serial_number, &maximum_component_length,
            &file_system_flags, file_system_name_buffer, 1000);
        String serial_text = volume_serial_number;
        if (serial_text.Length() > 32)
            serial_text = serial_text.SubString(1, 32);
        unsigned char marker_byte = 255;
        char marker_char = marker_byte;
        challenge = random(1000000);
        String packet = "";
        packet = packet + marker_char;
        packet = packet + marker_char;
        packet = packet + EncodeNumber(challenge, 3);
        packet = packet + EncodeNumber(MainForm->version_major, 1);
        packet = packet + EncodeNumber(MainForm->version_minor, 1);
        packet = packet + EncodeNumber(MainForm->version_build, 1);
        packet = packet + EncodeNumber(112, 1);
        packet = packet + EncodeNumber(serial_text.Length(), 1);
        packet = packet + serial_text;
        packet = EncodeNumber(packet.Length(), 2) + packet;
        sequence_counter = 0;
        ((TCustomWinSocket *)MainForm->client->Socket)->SendText(packet);
    }
    catch (...)
    {
    }
}

void Client::SetAdminLevel(int new_admin_level)
{
    admin_level = new_admin_level;
}

int Client::GetAdminLevel()
{
    return admin_level;
}

void Client::SetPlayerId(int new_player_id)
{
    player_id = new_player_id;
}

int Client::GetPlayerId()
{
    return player_id;
}

// Handles a frame that starts with two marker bytes: the server's unencrypted replies about the connection itself.
// Returns whether the frame was one of these.
bool Client::HandleRawPacket(String frame)
{
    int init_sequence;
    bool is_raw = false;
    if (frame.Length() >= 2)
    {
        try
        {
            if (ByteValue(frame[1]) == 255 && ByteValue(frame[2]) == 255)
            {
                is_raw = true;
                // A bare rejection: the server refused the connection.
                if (frame.Length() == 3 && ByteValue(frame[3]) == 1)
                    init_rejected = true;
                // A ban, permanent or for the minutes the next byte gives.
                if (frame.Length() >= 4 && ByteValue(frame[3]) == 3)
                {
                    is_banned = true;
                    if (ByteValue(frame[4]) >= 2)
                        is_permanent_ban = true;
                    if (frame.Length() >= 5)
                        ban_minutes = ByteValue(frame[5]);
                }
                // A protocol rejection: the server names the protocol version it speaks.
                if (frame.Length() >= 4 && ByteValue(frame[3]) == 20)
                {
                    version_rejected = true;
                    int version_major = DecodeNumber(frame[4]);
                    MainForm->edits->caret_enabled = false;
                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                        MainForm->strings->GetDialogText(DialogText_ConnectionRefused, false),
                        "Incompatible server protocol ( " + IntToStr(version_major) +
                            " ) Use the latest client to connect this server.");
                }
                // A version rejection: the server names the version it wants.
                if (frame.Length() >= 6 && ByteValue(frame[3]) == 1)
                {
                    version_rejected = true;
                    int version_major = DecodeNumber(frame[4]);
                    int version_minor = DecodeNumber(frame[5]);
                    int version_build = DecodeNumber(frame[6]);
                    MainForm->edits->caret_enabled = false;
                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                        MainForm->strings->GetDialogText(DialogText_ConnectionRefused, false),
                        MainForm->strings->GetDialogText(DialogText_ConnectionRefused, true) + " " +
                            MainForm->VersionString(version_major, version_minor, version_build, 48) + ".");
                }
                // The init reply: sequence start, both keys, the player id and the server's challenge answer.
                // The client answers with its keys and its id, now encrypted.
                if (frame.Length() >= 12 && ByteValue(frame[3]) == 2)
                {
                    int key_seed = ByteValue(frame[4]);
                    init_sequence = ByteValue(frame[5]) + 7 * key_seed;
                    key_seed = init_sequence;
                    key_seed = key_seed - 13;
                    SetConnectionKeys(key_seed, ByteValue(frame[6]), ByteValue(frame[7]));
                    String player_id_text = frame.SubString(8, 2);
                    server_challenge_hash = DecodeNumber(frame.SubString(10, 3));
                    SetPlayerId(DecodeNumber(player_id_text));
                    String reply = EncodeNumber(send_key, 2);
                    reply = reply + EncodeNumber(receive_key, 2);
                    reply = reply + EncodeNumber(player_id, 2);
                    is_initialized = true;
                    ShowPendingScreen();
                    SendPacket(AC_CONFIRM, OP_SECURITY, reply);
                }
                // The other kinds are handed to the packet processor as packets of their own.
                if (frame.Length() >= 3)
                {
                    // A bare rejection: the server refused the connection.
                    // The init reply: sequence start, both keys, the player id and the server's challenge answer.
                    // The client answers with its keys and its id, now encrypted.
                    // A ban, permanent or for the minutes the next byte gives.
                    if (ByteValue(frame[3]) == 4)
                        processor->DispatchPacket(AC_SET, OP_MAP, frame.SubString(4, frame.Length() - 3));
                    if (ByteValue(frame[3]) == 5)
                        processor->DispatchPacket(AC_SYN1, OP_PLAY, frame.SubString(4, frame.Length() - 3));
                    if (ByteValue(frame[3]) == 6)
                        processor->DispatchPacket(AC_SYN2, OP_PLAY, frame.SubString(4, frame.Length() - 3));
                    if (ByteValue(frame[3]) == 7)
                        processor->DispatchPacket(AC_SYN3, OP_PLAY, frame.SubString(4, frame.Length() - 3));
                    if (ByteValue(frame[3]) == 8)
                        processor->DispatchPacket(AC_SYN4, OP_PLAY, frame.SubString(4, frame.Length() - 3));
                    if (ByteValue(frame[3]) == 12)
                        processor->DispatchPacket(AC_SYN5, OP_PLAY, frame.SubString(4, frame.Length() - 3));
                    if (ByteValue(frame[3]) == 9)
                        processor->DispatchPacket(AC_LIST, OP_PLAYERS, frame.SubString(4, frame.Length() - 3));
                    if (ByteValue(frame[3]) == 10)
                        processor->DispatchPacket(AC_NEW, OP_MAP, frame.SubString(4, frame.Length() - 3));
                    if (ByteValue(frame[3]) == 11)
                        processor->DispatchPacket(AC_RESULT, OP_PLAYERS, frame.SubString(4, frame.Length() - 3));
                }
            }
        }
        catch (...)
        {
        }
    }
    return is_raw;
}

// Collects received bytes and handles each complete frame: raw frames directly, the others decrypted with the
// receive key and dispatched by family and action.
void Client::ReceiveData(TCustomWinSocket *socket, String text)
{
    receive_buffer.Insert(text, receive_buffer.Length() + 1);
    // A runaway receive buffer closes the connection.
    if (receive_buffer.Length() > 400000)
    {
        socket->Close();
        return;
    }
    while (receive_buffer.Length() >= 3)
    {
        int frame_length = 2;
        frame_length = DecodeLength(receive_buffer.SubString(1, 2)) + frame_length;
        if (receive_buffer.Length() < frame_length)
            break;
        if (!HandleRawPacket(receive_buffer.SubString(3, frame_length - 2)))
        {
            String frame = receive_buffer.SubString(3, frame_length - 2);
            for (int i = 1; i <= frame.Length(); i++)
            {
                int frame_byte = (unsigned char)frame[i];
                // Undoes the sender's FlipHighBits: the high bit of each byte but 128 is flipped back.
                if (frame_byte < 128)
                    frame[i] = frame[i] - 128;
                if (frame_byte > 128)
                    frame[i] = frame[i] - 128;
            }
            string frame_bytes = frame.c_str();
            frame = UnscrambleBytes(receive_key, frame_bytes.begin(), frame_bytes.end());
            int action = ByteValue(frame[1]);
            int family = ByteValue(frame[2]);
            processor->DispatchPacket(action, family, frame.SubString(3, frame.Length() - 2));
        }
        receive_buffer.Delete(1, frame_length);
    }
}

// Whether two tiles are within reach of each other, by the sum of the x and y distances.
bool Client::IsWithinReach(int from_x, int from_y, int to_x, int to_y)
{
    bool in_reach = false;
    int dx = to_x - from_x;
    int dy = to_y - from_y;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (dx + dy <= 2)
        in_reach = true;
    return in_reach;
}

// Whether a tile is close enough to be heard; the range is longer toward the bottom of the view.
bool Client::IsInSoundRange(int from_x, int from_y, int to_x, int to_y)
{
    bool in_range = false;
    int dx = to_x - from_x;
    int dy = to_y - from_y;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (from_x < to_x && from_y < to_y)
    {
        if (dx + dy <= 14)
            in_range = true;
    }
    else if (dx + dy <= 12)
        in_range = true;
    return in_range;
}

// Whether a wall is close enough to the view centre to draw; the range is longer toward the bottom of the view.
bool Client::IsWallInDrawRange(int from_x, int from_y, int to_x, int to_y)
{
    bool in_range = false;
    int dx = to_x - from_x;
    int dy = to_y - from_y;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (from_x < to_x && from_y < to_y)
    {
        if (dx + dy <= 40 && dx <= from_x + 19 && dy <= from_y + 19)
            in_range = true;
    }
    else if (dx + dy <= 13)
        in_range = true;
    return in_range;
}

// Whether a map object is close enough to the view centre to draw; the range is longer toward the bottom of the view.
bool Client::IsObjectInDrawRange(int from_x, int from_y, int to_x, int to_y)
{
    bool in_range = false;
    int dx = to_x - from_x;
    int dy = to_y - from_y;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (from_x < to_x && from_y < to_y)
    {
        if (dx + dy <= 24)
            in_range = true;
    }
    else if (dx + dy <= 15)
        in_range = true;
    return in_range;
}

// Whether a tall map object is close enough to draw; its range reaches further toward the bottom of the view.
bool Client::IsTallObjectInDrawRange(int from_x, int from_y, int to_x, int to_y)
{
    bool in_range = false;
    int dx = to_x - from_x;
    int dy = to_y - from_y;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (from_x < to_x && from_y < to_y)
    {
        if (dx + dy <= 32)
            in_range = true;
    }
    else if (dx + dy <= 15)
        in_range = true;
    return in_range;
}

// Whether a tile is close enough to the view centre to draw, the same distance in every direction.
bool Client::IsInDrawRange(int from_x, int from_y, int to_x, int to_y)
{
    bool in_range = false;
    int dx = to_x - from_x;
    int dy = to_y - from_y;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (dx + dy <= 14)
        in_range = true;
    return in_range;
}

// The direction from one tile to the next one beside it, 1 to 4; 0 when they are not side by side.
char Client::GetAdjacentDirection(int from_x, int from_y, int to_x, int to_y)
{
    int dx = to_x - from_x;
    int dy = to_y - from_y;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (dx + dy <= 1)
    {
        if (from_x < to_x)
            return 2;
        if (from_x > to_x)
            return 4;
        if (from_y < to_y)
            return 3;
        if (from_y > to_y)
            return 1;
    }
    return 0;
}

// Decodes a frame length from its two leading bytes, each a base-253 digit plus one, lowest first.
unsigned int Client::DecodeLength(String encoded)
{
    int digit;
    int length = 0;
    try
    {
        for (int i = 1; i <= encoded.Length(); i++)
        {
            unsigned char raw_byte = encoded[i];
            if (raw_byte == 254 || raw_byte == 0)
                break;
            digit = raw_byte;
            digit--;
            if (i == 1)
                length += digit;
            if (i == 2)
                length += NUMBER_ENCODING_BASE * digit;
        }
    }
    catch (...)
    {
        length = 0;
    }
    return length;
}

void Packetcontrols_UnusedLiterals1()
{
    "";
    "";
    "";
    "";
}

// Encodes a number as base-253 digits plus one, lowest first, padded to min_length with the filler byte.
DEFINE_ENCODE_NUMBER(Client)

// Decodes up to four base-253 digits, lowest first; the filler byte ends the number.
int Client::DecodeNumber(String encoded)
{
    int digit;
    int result = 0;
    try
    {
        for (int i = 1; i <= encoded.Length(); i++)
        {
            unsigned char exact_quotient = encoded[i];
            if (exact_quotient == 254)
                break;
            digit = exact_quotient;
            digit--;
            if (i == 1)
                result += digit;
            if (i == 2)
                result += NUMBER_ENCODING_BASE * digit;
            if (i == 3)
                result += 64009 * digit;
            if (i == 4)
                result += digit * 0xF71AE5;
        }
    }
    catch (...)
    {
        result = 0;
    }
    return result;
}

// The answer the server must give to the client's init challenge; the login and account replies check it.
int Client::HashChallenge(int sent_challenge)
{
    int hash = sent_challenge + 1;
    int mod = hash % 11 + 1;
    int factor = hash % 9 + 1;
    int addend = hash % 2004 + 1;
    hash = 11092004 - hash;
    hash = hash % (119 * mod);
    hash = 119 * (factor * hash);
    hash = addend + hash + 110904;
    return hash;
}

// Scrambles a packet with the send key, then flips the high bits.
String Client::EncryptPacket(int key, String packet)
{
    String result = "";
    string packet_bytes = packet.c_str();
    result = ScrambleBytes(key, packet_bytes.begin(), packet_bytes.end());
    result = FlipHighBits(result);
    return result;
}

void Packetcontrols_UnusedLiterals2()
{
    "";
    "";
}

// Holds back runs of bytes whose code divides by the key and puts each run out reversed, then reads the result
// alternately from the front and the back.
String Client::ScrambleBytes(int divisor, char *data_begin, char *data_end)
{
    String result = "";
    int divisor_copy = divisor;
    try
    {
        stack<char> held_chars;
        deque<char> reordered_chars;
        for (; data_begin != data_end; data_begin++)
        {
            unsigned char current_byte = *data_begin;
            int char_code = current_byte;
            if (char_code % divisor_copy == 0)
                held_chars.push(*data_begin);
            else
            {
                while (!held_chars.empty())
                {
                    reordered_chars.push_back(held_chars.top());
                    held_chars.pop();
                }
                reordered_chars.push_back(*data_begin);
            }
        }
        while (!held_chars.empty())
        {
            reordered_chars.push_back(held_chars.top());
            held_chars.pop();
        }
        while (!reordered_chars.empty())
        {
            if (!reordered_chars.empty())
            {
                result.Insert((char)reordered_chars.front(), result.Length() + 1);
                reordered_chars.pop_front();
            }
            if (!reordered_chars.empty())
            {
                result.Insert((char)reordered_chars.back(), result.Length() + 1);
                reordered_chars.pop_back();
            }
        }
    }
    catch (...)
    {
        result = "";
    }
    return result;
}

// Undoes ScrambleBytes: takes the bytes at odd places in order and the others from the end, then reverses the runs
// of bytes whose code divides by the key again.
String Client::UnscrambleBytes(int divisor, char *data_begin, char *data_end)
{
    String result = "";
    int divisor_copy = divisor;
    try
    {
        stack<char> char_stack;
        queue<char> char_queue;
        char to_first_list = true;
        for (; data_begin != data_end; data_begin++)
        {
            if (to_first_list)
            {
                char_queue.push(*data_begin);
                to_first_list = false;
            }
            else
            {
                char_stack.push(*data_begin);
                to_first_list = true;
            }
        }
        while (!char_stack.empty())
        {
            char_queue.push(char_stack.top());
            char_stack.pop();
        }
        char_stack.empty();
        while (!char_queue.empty())
        {
            char current_char = char_queue.front();
            char_queue.pop();
            unsigned char char_byte = current_char;
            int char_code = char_byte;
            if (char_code % divisor_copy == 0)
                char_stack.push(current_char);
            else
            {
                while (!char_stack.empty())
                {
                    result.Insert((char)char_stack.top(), result.Length() + 1);
                    char_stack.pop();
                }
                result.Insert(current_char, result.Length() + 1);
            }
        }
        while (!char_stack.empty())
        {
            result.Insert((char)char_stack.top(), result.Length() + 1);
            char_stack.pop();
        }
    }
    catch (...)
    {
        result = "";
    }
    return result;
}

// Decodes a text kept obfuscated in the program: reversed, with each character mirrored within its range by position.
String Client::DeobfuscateString(String text)
{
    unsigned char char_byte;
    int mirrored_code;
    int char_code;
    String reversed = "";
    String result = "";
    int position = text.Length();
    for (; position >= 1; position--)
        reversed = reversed + text[position];
    for (int i = 1; i <= text.Length(); i++)
    {
        char raw_char = reversed[i];
        char_byte = raw_char;
        char_code = char_byte;
        if (i % 2 == (text.Length() + 1) % 2)
        {
            if (char_code >= 34 && char_code <= 125)
            {
                int mirrored_code;
                char_code = 125 - char_code + 34;
                mirrored_code = char_code;
                raw_char = mirrored_code;
                result.Insert(raw_char, result.Length() + 1);
            }
            else
                result.Insert(reversed[i], result.Length() + 1);
        }
        else
        {
            char is_mapped = false;
            if (char_code >= 34 && char_code <= 79)
            {
                int low_mirrored_code;
                char_code = 79 - char_code + 34;
                is_mapped = true;
                low_mirrored_code = char_code;
                raw_char = low_mirrored_code;
                result.Insert(raw_char, result.Length() + 1);
            }
            if (char_code >= 80 && char_code <= 125)
            {
                int high_mirrored_code;
                char_code = 125 - char_code + 80;
                is_mapped = true;
                high_mirrored_code = char_code;
                raw_char = high_mirrored_code;
                result.Insert(raw_char, result.Length() + 1);
            }
            if (!is_mapped)
                result.Insert(reversed[i], result.Length() + 1);
        }
    }
    return result;
}

// Flips the high bit of every byte except 128, so a sent packet holds no zero byte.
String Client::FlipHighBits(String text)
{
    if (text.Length() >= 1)
    {
        for (int i = 1; i <= text.Length(); i++)
        {
            int byte_value = (unsigned char)text[i];
            if (byte_value < 128)
                text[i] = text[i] - 128;
            if (byte_value > 128)
                text[i] = text[i] - 128;
        }
    }
    return text;
}

// Builds and sends one packet with the send key while the socket is active.
bool Client::SendPacket(int action, int family, String data)
{
    int result;
    try
    {
        result = 0;
        if (MainForm->client->Active)
        {
            String packet = BuildPacket(send_key, action, family, data);
            if (packet != "")
                ((TCustomWinSocket *)MainForm->client->Socket)->SendText(packet);
        }
    }
    catch (...)
    {
        result = 0;
    }
    return result;
}

// Once connected, shows the screen the connection was made for: the create account form, the account recover text,
// or the login screen with the saved account name filled in.
void Client::ShowPendingScreen()
{
    if (MainForm->animation->GetScreenMode() == ScreenMode_Menu)
    {
        if (MainForm->animation->menu->connect_for_login)
            MainForm->animation->menu->connect_for_login = false;
        if (MainForm->animation->menu->connect_for_create_account)
        {
            MainForm->animation->menu->connect_for_create_account = 0;
            if (MainForm->animation->credits->is_running)
                MainForm->animation->credits->Stop();
            MainForm->edits->Clear();
            MainForm->animation->SetSubState(MenuScreen_CreateAccount);
            MainForm->gfx->LoadCreateAccountScreen();
            MainForm->edits->AddEdit(EditId_AccountName, EditGroup_Menu, ACCOUNT_FORM_EDIT_X, ACCOUNT_NAME_EDIT_Y, 242,
                EDIT_LINE_HEIGHT);
            MainForm->edits->AddEdit(EditId_AccountPassword, EditGroup_Menu, ACCOUNT_FORM_EDIT_X,
                ACCOUNT_PASSWORD_EDIT_Y, 242, EDIT_LINE_HEIGHT);
            MainForm->edits->AddEdit(EditId_ConfirmPassword, EditGroup_Menu, ACCOUNT_FORM_EDIT_X,
                ACCOUNT_CONFIRM_EDIT_Y, 242, EDIT_LINE_HEIGHT);
            MainForm->edits->AddEdit(EditId_AccountDetailA, EditGroup_Menu, ACCOUNT_FORM_EDIT_X, ACCOUNT_EDIT8_Y, 242,
                EDIT_LINE_HEIGHT);
            MainForm->edits->AddEdit(EditId_AccountDetailB, EditGroup_Menu, ACCOUNT_FORM_EDIT_X, ACCOUNT_EDIT9_Y, 242,
                EDIT_LINE_HEIGHT);
            MainForm->edits->AddEdit(EditId_Email, EditGroup_Menu, ACCOUNT_FORM_EDIT_X, ACCOUNT_EDIT10_Y, 221,
                EDIT_LINE_HEIGHT);

            MainForm->edits->SetPasswordMode(EditId_AccountPassword, true);
            MainForm->edits->SetPasswordMode(EditId_ConfirmPassword, true);
            MainForm->edits->SetShowStrength(EditId_AccountPassword, true);
            MainForm->edits->SetAllowSpaces(EditId_AccountName, false);
            MainForm->edits->SetAllowSpaces(EditId_AccountPassword, false);
            MainForm->edits->SetAllowSpaces(EditId_ConfirmPassword, false);
            MainForm->edits->SetCharFilter(EditId_AccountName, EditCharFilter_Alphanumeric);
            MainForm->edits->SetCharFilter(EditId_AccountPassword, EditCharFilter_Password);
            MainForm->edits->SetCharFilter(EditId_ConfirmPassword, EditCharFilter_Password);
            MainForm->edits->SetCharFilter(EditId_AccountDetailA, EditCharFilter_Letters);
            MainForm->edits->SetCharFilter(EditId_AccountDetailB, EditCharFilter_Letters);
            MainForm->edits->SetCharFilter(EditId_Email, EditCharFilter_Email);
            MainForm->edits->SetMaxLength(EditId_AccountName, 16);
            MainForm->edits->SetMaxLength(EditId_AccountPassword, 16);
            MainForm->edits->SetMaxLength(EditId_ConfirmPassword, 16);
            MainForm->edits->caret_enabled = false;
            MainForm->animation->menu->OpenTextWindow();
            MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(GameText_ItIsVeryImportant), true);
            MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(GameText_LotOfPlayersWho), true);
            MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(GameText_YourInformationWillOnly),
                false);
            MainForm->edits->FocusEdit(EditId_AccountName);
        }
        if (MainForm->animation->menu->connect_for_recovery)
        {
            MainForm->animation->menu->connect_for_recovery = false;
            if (MainForm->animation->credits->is_running)
                MainForm->animation->credits->Stop();
            MainForm->edits->Clear();
            MainForm->net_client->login_failure_count = 0;
            MainForm->gfx->LoadLoginScreen();
            // Without the soft keypad the login form has a name box and a password box.
            // With it, only the name box; the password is entered on the keypad screen.
            if (MainForm->config->GetSkipSoftpad())
            {
                MainForm->animation->SetSubState(MenuScreen_Login);
                MainForm->edits->AddEdit(EditId_AccountName, EditGroup_Menu, LOGIN_EDIT_X, 309, 141, EDIT_LINE_HEIGHT);
                MainForm->edits->AddEdit(EditId_Password, EditGroup_Menu, LOGIN_EDIT_X, 340, 141, EDIT_LINE_HEIGHT);
                MainForm->edits->SetCharFilter(EditId_AccountName, EditCharFilter_Alphanumeric);
                MainForm->edits->SetCharFilter(EditId_Password, EditCharFilter_Password);
                MainForm->edits->SetAllowSpaces(EditId_AccountName, false);
                MainForm->edits->SetAllowSpaces(EditId_Password, false);
                MainForm->edits->SetPasswordMode(EditId_Password, true);
                MainForm->edits->SetMaxLength(EditId_AccountName, 16);
                MainForm->edits->SetMaxLength(EditId_Password, 16);
                if (MainForm->config->GetAutoAccountName().Length() < 2)
                    MainForm->edits->FocusEdit(EditId_AccountName);
                else
                {
                    MainForm->edits->SetText(EditId_AccountName, MainForm->config->GetAutoAccountName());
                    MainForm->edits->FocusEdit(EditId_Password);
                }
            }
            else
            {
                MainForm->animation->SetSubState(MenuScreen_GridAccountName);
                MainForm->edits->AddEdit(EditId_AccountName, EditGroup_Menu, LOGIN_EDIT_X, GRID_LOGIN_NAME_EDIT_Y,
                    161, EDIT_LINE_HEIGHT);
                MainForm->edits->SetCharFilter(EditId_AccountName, EditCharFilter_Letters);
                MainForm->edits->SetAllowSpaces(EditId_AccountName, false);
                MainForm->edits->SetMaxLength(EditId_AccountName, 16);
                MainForm->edits->FocusEdit(EditId_AccountName);
            }
        }
    }
}

// On a lost connection, says why: in game a message box, on the menu the reason the server gave, such as a ban, too
// many account lookups or a busy server, or a general one.
void Client::HandleDisconnect()
{
    if (!version_rejected)
    {
        // After the init the reason comes from the account replies; before it, from the raw init replies.
        if (is_initialized)
        {
            if (MainForm->animation->GetScreenMode() == ScreenMode_Game)
            {
                MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_ConnectionLost,
                    MainForm->strings->GetDialogText(DialogText_LostConnectionExit, false),
                    MainForm->strings->GetDialogText(DialogText_LostConnectionExit, true));
                MainForm->edits->caret_enabled = false;
            }
            if (MainForm->animation->GetScreenMode() == ScreenMode_Menu)
            {
                MainForm->animation->menu->SetEnteringGame(false);
                MainForm->animation->menu->is_loading_world = false;
                if (!MainForm->animation->credits->is_running)
                {
                    MainForm->edits->Clear();
                    MainForm->gfx->LoadMainMenuScreen(false);
                    MainForm->animation->SetSubState(MenuScreen_Main);
                }
                char message_shown = false;
                if (init_rejected)
                {
                    MainForm->edits->caret_enabled = false;
                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                        MainForm->strings->GetDialogText(DialogText_ServerFull, false),
                        MainForm->strings->GetDialogText(DialogText_ServerFull, true));
                    message_shown = true;
                }
                if (is_banned)
                {
                    MainForm->edits->caret_enabled = false;
                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                        MainForm->strings->GetDialogText(DialogText_AccountIsBlocked, false),
                        MainForm->strings->GetDialogText(DialogText_AccountIsBlocked, true));
                    message_shown = true;
                }
                if (account_reply_disconnect && !message_shown)
                {
                    MainForm->edits->caret_enabled = false;
                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                        MainForm->strings->GetDialogText(DialogText_RequestDeniedServer, false),
                        MainForm->strings->GetDialogText(DialogText_RequestDeniedServer, true));
                    message_shown = true;
                }
                if (login_retry_pending && !message_shown)
                {
                    MainForm->edits->caret_enabled = false;
                    MainForm->animation->menu->retry_ticks = 500;
                    MainForm->animation->menu->retry_account_name = MainForm->animation->menu->account_name;
                    MainForm->animation->menu->retry_password = MainForm->animation->menu->password;
                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                        MainForm->strings->GetDialogText(DialogText_LoginRefusedServer, false),
                        MainForm->strings->GetDialogText(DialogText_LoginRefusedServer, true));
                    message_shown = true;
                }
                if (lookups_refused && !message_shown)
                {
                    MainForm->edits->caret_enabled = false;
                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain, "Login Refused",
                        "Too many account lookups from this connection. Try again in a few minutes.");
                    message_shown = true;
                }
                if (server_busy && !message_shown)
                {
                    MainForm->edits->caret_enabled = false;
                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain, "Server Busy",
                        "The server is very busy at this moment. Please try again shortly.");
                    message_shown = true;
                }
                if (!message_shown)
                {
                    MainForm->edits->caret_enabled = false;
                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                        MainForm->strings->GetDialogText(DialogText_LostConnectionLater, false),
                        MainForm->strings->GetDialogText(DialogText_LostConnectionLater, true));
                }
                is_banned = false;
                is_permanent_ban = false;
                account_reply_disconnect = false;
                login_retry_pending = false;
                lookups_refused = false;
                server_busy = false;
                is_initialized = false;
                MainForm->animation->menu->is_form_open = false;
                MainForm->animation->menu->is_text_window_open = false;
            }
        }
        else if (MainForm->animation->GetScreenMode() == ScreenMode_Menu)
        {
            if (!MainForm->animation->credits->is_running)
            {
                MainForm->edits->Clear();
                MainForm->gfx->LoadMainMenuScreen(false);
                MainForm->animation->SetSubState(MenuScreen_Main);
            }
            char menu_message_shown = false;
            if (init_rejected)
            {
                is_initialized = false;
                init_rejected = false;
                menu_message_shown = true;
                MainForm->edits->caret_enabled = false;
                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                    MainForm->strings->GetDialogText(DialogText_ServerFull, false),
                    MainForm->strings->GetDialogText(DialogText_ServerFull, true));
            }
            if (is_banned)
            {
                is_banned = false;
                account_reply_disconnect = false;
                login_retry_pending = false;
                is_initialized = false;
                MainForm->sfx->PlaySfx(SfxSound_Disconnect);
                if (is_permanent_ban)
                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                        MainForm->strings->GetDialogText(DialogText_ConnectionIsBlockedPermanent, false),
                        MainForm->strings->GetDialogText(DialogText_ConnectionIsBlockedPermanent, true));
                else
                {
                    String ban_minutes_text =
                        " " + IntToStr(ban_minutes) + MainForm->net_client->DeobfuscateString("C,h+X1d2 ");
                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                        MainForm->strings->GetDialogText(DialogText_ConnectionIsBlockedTemporary, false),
                        MainForm->strings->GetDialogText(DialogText_ConnectionIsBlockedTemporary, true) +
                            ban_minutes_text);
                }
                is_permanent_ban = false;
                menu_message_shown = true;
            }
            if (!menu_message_shown)
            {
                is_initialized = false;
                MainForm->edits->caret_enabled = false;
                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                    MainForm->strings->GetDialogText(DialogText_CouldNotFindServer, false),
                    MainForm->strings->GetDialogText(DialogText_CouldNotFindServer, true));
            }
        }
    }
    is_initialized = false;
    version_rejected = false;
    is_banned = false;
    MainForm->animation->menu->connect_for_create_account = 0;
    MainForm->animation->menu->connect_for_recovery = false;
    MainForm->animation->menu->connect_for_login = false;
    MainForm->client->Active = false;
}

// Garbles chat text for a drunk player: added letters, starred vowels and hiccups, cut to the chat length.
String Client::MakeDrunkText(String text)
{
    int unused_roll;
    if (text.Length() > 0)
    {
        unused_roll = random(10);
        for (int i = 1; i <= text.Length(); i++)
        {
            char handled = false;
            if (text[i] == 'e' || text[i] == 'a' || text[i] == 'E' || text[i] == 'A')
            {
                if (i + 1 <= text.Length() && text[i + 1] != 'e' && text[i + 1] != 'a')
                {
                    if ((text[i + 1] != 'E'))
                    {
                        if ((text[i + 1] != 'A'))
                            handled = true;
                    }
                }
                if (i == text.Length())
                    handled = true;
                if (handled)
                {
                    int j_roll = random(10);
                    if (j_roll < 7)
                        text.Insert("j", i + 1);
                }
            }
            if (text[i] == 'u' || text[i] == 'o' || text[i] == 'U' || text[i] == 'O')
            {
                if ((i + 1 <= text.Length()))
                {
                    if ((text[i + 1] != 'u'))
                    {
                        if ((text[i + 1] != 'o'))
                        {
                            if ((text[i + 1] != 'U'))
                            {
                                if ((text[i + 1] != 'O'))
                                    handled = true;
                            }
                        }
                    }
                }
                if (i == text.Length())
                    handled = true;
                if (handled)
                {
                    int w_roll = random(10);
                    if (w_roll < 7)
                        text.Insert("w", i + 1);
                }
            }
            if (text[i] == 'i')
            {
                handled = true;
                int u_roll = random(10);
                if (u_roll < 4)
                    text.Insert("u", i + 1);
            }
            if (!handled && text[i] != ' ')
            {
                int double_roll = random(10);
                if (double_roll < 4)
                    text.Insert(text[i], i + 1);
            }
        }
        for (int j = 1; j <= text.Length(); j++)
        {
            if (text[j] == 'u' || text[j] == 'o' || text[j] == 'U' || text[j] == 'O' || text[j] == 'e' ||
                text[j] == 'a' || text[j] == 'E' || text[j] == 'A' || text[j] == 'I' || text[j] == 'i')
            {
                int star_roll = random(24);
                if (star_roll < 2)
                    text[j] = '*';
            }
        }
        for (int k = 1; k <= text.Length(); k++)
        {
            if (text[k] == ' ')
            {
                int hic_roll = random(10);
                if (hic_roll < 3)
                {
                    text.Insert("*hic* ", k + 1);
                    k += 6;
                }
            }
        }
    }
    if (text.Length() >= 128)
    {
        text = text.SubString(1, 126);
        text.Insert("..", text.Length() + 1);
    }
    return text;
}

// The text with its first letter in capitals.
DEFINE_CAPITALIZE(Client)

// Replaces each filter word in the text with stars when the filter is on; the strict filter drops the whole text.
String Client::CensorText(String text)
{
    String result = "";
    String lower_text = LowerCase(text);
    if (MainForm->config->GetFilterEnabled())
    {
        char censored_any = false;
        for (int i = 0; i < word_filter->Count; i++)
        {
            if (word_filter->Strings[i].Length() <= lower_text.Length())
            {
                for (int j = 1; j <= lower_text.Length(); j++)
                {
                    if (word_filter->Strings[i].Length() > 0)
                    {
                        if (word_filter->Strings[i][1] == lower_text[j])
                        {
                            if (word_filter->Strings[i].Length() + j - 1 <= lower_text.Length())
                            {
                                if (lower_text.SubString(j, word_filter->Strings[i].Length()) ==
                                    word_filter->Strings[i])
                                {
                                    lower_text.Delete(j, word_filter->Strings[i].Length());
                                    lower_text.Insert("****", j);
                                    text.Delete(j, word_filter->Strings[i].Length());
                                    text.Insert("****", j);
                                    censored_any = true;
                                }
                            }
                        }
                    }
                }
            }
        }
        // With the strict filter on, a line with any filtered word is dropped whole.
        if (MainForm->config->GetStrictFilter() && censored_any)
            text = "";
    }
    result = text;
    return result;
}

// Starts splitting text at separator; NextToken and RemainingTokens read from it.
void Client::BeginTokens(String text, char separator)
{
    token_position = 1;
    token_text = text;
    token_length = text.Length();
    token_separator = separator;
}

// The rest of the text after the tokens read so far.
String Client::RemainingTokens()
{
    String rest = "";
    try
    {
        if (token_length >= 1)
        {
            while (token_position <= token_length)
            {
                rest.Insert(token_text[token_position], rest.Length() + 1);
                token_position++;
            }
        }
    }
    catch (...)
    {
        rest = "";
    }
    return rest;
}

// The next token, up to the separator or the end of the text; empty at the end.
String Client::NextToken()
{
    String token = "";
    try
    {
        if (token_length >= 1)
        {
            while (token_position <= token_length)
            {
                if (token_text[token_position] != token_separator)
                    token.Insert(token_text[token_position], token.Length() + 1);
                else
                {
                    token_position++;
                    break;
                }
                token_position++;
            }
        }
    }
    catch (...)
    {
        token = "";
    }
    return token;
}

void Client::SetConnectionKeys(int new_sequence_start, int new_receive_key, int new_send_key)
{
    sequence_start = new_sequence_start;
    receive_key = new_receive_key;
    send_key = new_send_key;
}

void Client::SetSequenceStart(int new_sequence_start)
{
    sequence_start = new_sequence_start;
}

void Client::SetSessionId(int new_session_id)
{
    session_id = new_session_id;
}

int Client::GetSessionId()
{
    return session_id;
}

void Packetcontrols_UnusedLiterals3()
{
    "";
    "";
    "";
}

// Builds one encrypted packet: action, family, the rolling sequence number and the data, then its length in front.
String Client::BuildPacket(int key, int action, int family, String data)
{
    char is_valid;
    String packet = "";
    try
    {
        char action_char = action;
        char family_char = family;
        is_valid = true;
        char action_byte;
        if (action_char)
            action_byte = action_char;
        else
            is_valid = false;
        char family_byte;
        if (family_char)
            family_byte = family_char;
        else
            is_valid = false;
        if (is_valid)
        {
            String header = "";
            String body = "";
            header = header + action_byte;
            header = header + family_byte;
            body = data;
            packet = packet + header;
            sequence_counter++;
            // The sequence number cycles through ten steps above the start the server sent.
            if (sequence_counter > 9)
                sequence_counter = 0;
            packet = packet + EncodeNumber(sequence_start + sequence_counter, 1);
            packet = packet + body;
            packet = EncryptPacket(key, packet);
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

int Client::ByteValue(unsigned char ch)
{
    unsigned char value = ch;
    return value;
}

// Whether enough time has passed since the last action, counted in hundredths of a second of the day.
char Client::CanAct()
{
    int elapsed;
    TTimeStamp now = DateTimeToTimeStamp(Now());
    int now_tick = now.Time / 10;
    elapsed = now_tick - last_action_tick;
    if (elapsed < 0)
    {
        if (last_action_tick > 7500000)
            elapsed = 44;
    }
    if (elapsed < 44)
        return false;
    pending_action_tick = now_tick;
    return true;
}

// Records the time CanAct approved as the last action.
void Client::CommitActionTime()
{
    last_action_tick = pending_action_tick;
}

// Whether text is four dot-separated numbers.
bool Client::IsIpAddress(String text)
{
    bool is_ip = true;
    char is_valid = true;
    int digits_in_part = 0;
    int dot_count = 0;
    try
    {
        for (int i = 1; i <= text.Length() && is_valid; i++)
        {
            if (text[i] == '1' || text[i] == '2' || text[i] == '3' || text[i] == '4' || text[i] == '5' ||
                text[i] == '6' || text[i] == '7' || text[i] == '8' || text[i] == '9' || text[i] == '0' ||
                text[i] == '.')
            {
                if (text[i] == '.')
                {
                    digits_in_part = 0;
                    dot_count++;
                }
                else
                {
                    digits_in_part++;
                    if (digits_in_part > 3)
                    {
                        is_ip = false;
                        is_valid = false;
                    }
                }
            }
            else
            {
                is_ip = false;
                is_valid = false;
            }
        }
        if (dot_count != 3)
            is_ip = false;
    }
    catch (...)
    {
        is_ip = false;
    }
    return is_ip;
}
