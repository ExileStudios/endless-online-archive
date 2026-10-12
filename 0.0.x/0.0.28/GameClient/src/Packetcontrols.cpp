#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Ctrl_menu.h"
#include "Packetoperations.h"
#include "Packetproces.h"
#include "Ctrl_credits.h"
#include "Ctrl_game.h"
#pragma package(smart_init)

// Creates the packet sender and processor, clears the connection state and loads the chat word filter.
Client::Client()
{
    int ctor_spare_int;
    int ctor_spare_value;
    sender = new PacketOperator();
    processor = new PacketProcessor();
    spare_string_list = new TStringList();
    send_key = 0;
    receive_key = 0;
    sequence_start = 0;
    player_id = 0;
    session_id = 0;
    init_rejected = false;
    is_initialized = 0;
    is_banned = false;
    account_reply_disconnect = false;
    login_retry_pending = false;
    is_permanent_ban = false;
    ban_minutes = 1;
    login_failure_count = 0;
    version_rejected = false;
    unread_flag_at_20 = false;
    unread_int_at_12 = -1;
    unread_int_at_16 = -1;
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
    DWORD file_size;
    char *buffer;
    String Str;
    try
    {
        file_handle = FileOpen(MainForm->strings->GetDataFilePath(3), 0);
        file_size = FileSeek(file_handle, 0, 2);
        FileSeek(file_handle, 0, 0);
        buffer = new char[file_size + 1];
        FileRead(file_handle, buffer, file_size);
        FileClose(file_handle);
        Str = buffer;
        Str.SetLength(file_size);
        delete[] buffer;
    }
    catch (...)
    {
    }
    std::string file_bytes = Str.c_str();
    Str = UnscrambleBytes(this, 26, file_bytes.begin(), file_bytes.end());
    String filter_word;
    BeginTokens(Str, 58);
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
    DWORD VolumeSerialNumber;
    DWORD MaximumComponentLength;
    DWORD FileSystemFlags;
    try
    {
        randomize();
    }
    catch (...)
    {
    }
    try
    {
        char VolumeNameBuffer[VOLUME_NAME_BUFFER_SIZE];
        char FileSystemNameBuffer[FILE_SYSTEM_NAME_BUFFER_SIZE];
        GetVolumeInformationA("c:\\", VolumeNameBuffer, 1000, &VolumeSerialNumber, &MaximumComponentLength,
            &FileSystemFlags, FileSystemNameBuffer, 1000);
        String serial_text = VolumeSerialNumber;
        if (serial_text.Length() > 32)
            serial_text = serial_text.SubString(1, 32);
        unsigned char marker_byte = 255;
        char marker_char = marker_byte;
        challenge = random(1000000);
        String packet = "";
        packet = packet + marker_char;
        packet = packet + marker_char;
        packet = packet + EncodeNumber(this, challenge, 3);
        packet = packet + EncodeNumber(this, MainForm->version_major, 1);
        packet = packet + EncodeNumber(this, MainForm->version_minor, 1);
        packet = packet + EncodeNumber(this, MainForm->version_build, 1);
        packet = packet + EncodeNumber(this, 112, 1);
        packet = packet + EncodeNumber(this, serial_text.Length(), 1);
        packet = packet + serial_text;
        packet = EncodeNumber(this, packet.Length(), 2) + packet;
        sequence_counter = 0;
        MainForm->client->Socket->SendText(packet);
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
    bool is_raw;
    int init_sequence;
    is_raw = 0;
    if (frame.Length() >= 2)
    {
        try
        {
            if (ByteValue(frame[1]) == 255)
            {
                if (ByteValue(frame[2]) == 255)
                {
                    is_raw = 1;
                    if (frame.Length() == 3)
                    {
                        // A bare rejection: the server refused the connection.
                        if (ByteValue(frame[3]) == 1)
                            init_rejected = true;
                    }
                    if (frame.Length() >= 4)
                    {
                        // A ban, permanent or for the minutes the next byte gives.
                        if (ByteValue(frame[3]) == 3)
                        {
                            is_banned = true;
                            if (ByteValue(frame[4]) >= 2)
                                is_permanent_ban = true;
                            if (frame.Length() >= 5)
                                ban_minutes = ByteValue(frame[5]);
                        }
                    }
                    if (frame.Length() >= 6)
                    {
                        // A version rejection: the server names the version it wants.
                        if (ByteValue(frame[3]) == 1)
                        {
                            version_rejected = true;
                            int version_major = DecodeNumber(String(frame[4]));
                            int version_minor = DecodeNumber(String(frame[5]));
                            int version_build = DecodeNumber(String(frame[6]));
                            MainForm->edits->caret_enabled = false;
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(DialogText_ConnectionRefused, false),
                                MainForm->strings->GetDialogText(DialogText_ConnectionRefused, true) + " " +
                                MainForm->VersionString(version_major, version_minor, version_build) + ".");
                        }
                    }
                    if (frame.Length() >= 12)
                    {
                        // The init reply: sequence start, both keys, the player id and the server's challenge answer.
                        // The client answers with its keys and its id, now encrypted.
                        if (ByteValue(frame[3]) == 2)
                        {
                            int sequence_byte = ByteValue(frame[4]);
                            init_sequence = 7 * sequence_byte + ByteValue(frame[5]);
                            init_sequence = init_sequence - 13;
                            SetConnectionKeys(init_sequence, ByteValue(frame[6]), ByteValue(frame[7]));
                            String player_id_text = frame.SubString(8, 2);
                            server_challenge_hash = DecodeNumber(frame.SubString(10, 3));
                            SetPlayerId(DecodeNumber(player_id_text));
                            String reply = EncodeNumber(this, send_key, 2);
                            reply = reply + EncodeNumber(this, receive_key, 2);
                            reply = reply + EncodeNumber(this, player_id, 2);
                            is_initialized = 1;
                            ShowPendingScreen();
                            SendPacket(AC_CONFIRM, OP_SECURITY, reply);
                        }
                    }
                    // The other kinds are handed to the packet processor as packets of their own.
                    if (frame.Length() >= 3)
                    {
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
        }
        catch (...)
        {
        }
    }
    return is_raw;
}

// Collects received bytes and handles each complete frame: raw frames directly, the others decrypted with the
// receive key and dispatched by family and action.
void Client::ReceiveData(Scktcomp::TCustomWinSocket *socket, String text)
{
    int i;
    receive_buffer.Insert(text, receive_buffer.Length() + 1);
    // A runaway receive buffer closes the connection.
    if (receive_buffer.Length() > 400000)
    {
        socket->Close();
        return;
    }
    while (this->receive_buffer.Length() >= 3)
    {
        int frame_length = 2;
        frame_length += this->DecodeLength(this->receive_buffer.SubString(1, 2));
        if (this->receive_buffer.Length() < frame_length)
            break;
        if (!this->HandleRawPacket(this->receive_buffer.SubString(3, frame_length - 2)))
        {
            String frame = this->receive_buffer.SubString(3, frame_length - 2);
            for (i = 1; i <= frame.Length(); i++)
            {
                int frame_byte = (unsigned char)frame[i];
                // Undoes the sender's FlipHighBits: the high bit of each byte but 128 is flipped back.
                if (frame_byte < 128)
                    frame[i] = frame[i] + 128;
                if (frame_byte > 128)
                    frame[i] = frame[i] + 128;
            }
            std::string frame_bytes = frame.c_str();
            frame = UnscrambleBytes(this, this->receive_key, frame_bytes.begin(), frame_bytes.end());
            int action = this->ByteValue(frame[1]);
            int family = this->ByteValue(frame[2]);
            this->processor->DispatchPacket(action, family, frame.SubString(3, frame.Length() - 2));
        }
        this->receive_buffer.Delete(1, frame_length);
    }
}

// Whether two tiles are within reach of each other, by the sum of the x and y distances.
bool Client::IsWithinReach(int from_x, int from_y, int to_x, int to_y)
{
    bool in_reach = 0;
    int dx = to_x - from_x;
    int dy = to_y - from_y;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (dx + dy <= 2)
        in_reach = 1;
    return in_reach;
}

// Whether a tile is close enough to be heard; the range is longer toward the bottom of the view.
bool Client::IsInSoundRange(int from_x, int from_y, int to_x, int to_y)
{
    bool in_range = 0;
    int dx = to_x - from_x;
    int dy = to_y - from_y;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (from_x < to_x && from_y < to_y)
    {
        if (dx + dy <= 14)
            in_range = 1;
    }
    else
    {
        if (dx + dy <= 11)
            in_range = 1;
    }
    return in_range;
}

// Whether a wall is close enough to the view centre to draw; the range is longer toward the bottom of the view.
bool Client::IsWallInDrawRange(int from_x, int from_y, int to_x, int to_y)
{
    bool in_range = 0;
    int dx = to_x - from_x;
    int dy = to_y - from_y;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (from_x < to_x && from_y < to_y)
    {
        if (dx + dy <= 40 && dx <= from_x + 19 && dy <= from_y + 19)
            in_range = 1;
    }
    else
    {
        if (dx + dy <= 13)
            in_range = 1;
    }
    return in_range;
}

// Whether a map object is close enough to the view centre to draw; the range is longer toward the bottom of the view.
bool Client::IsObjectInDrawRange(int from_x, int from_y, int to_x, int to_y)
{
    bool in_range = 0;
    int dx = to_x - from_x;
    int dy = to_y - from_y;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (from_x < to_x && from_y < to_y)
    {
        if (dx + dy <= 24)
            in_range = 1;
    }
    else
    {
        if (dx + dy <= 15)
            in_range = 1;
    }
    return in_range;
}

// Whether a tall map object is close enough to draw; its range reaches further toward the bottom of the view.
bool Client::IsTallObjectInDrawRange(int from_x, int from_y, int to_x, int to_y)
{
    bool in_range = 0;
    int dx = to_x - from_x;
    int dy = to_y - from_y;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (from_x < to_x && from_y < to_y)
    {
        if (dx + dy <= 32)
            in_range = 1;
    }
    else
    {
        if (dx + dy <= 15)
            in_range = 1;
    }
    return in_range;
}

// Whether a tile is close enough to the view centre to draw, the same distance in every direction.
bool Client::IsInDrawRange(int from_x, int from_y, int to_x, int to_y)
{
    bool in_range = 0;
    int dx = to_x - from_x;
    int dy = to_y - from_y;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (dx + dy <= 14)
        in_range = 1;
    return in_range;
}

// Whether two tiles are the same tile or next to each other along x or y.
bool Client::IsAdjacent(int from_x, int from_y, int to_x, int to_y)
{
    bool is_adjacent = 0;
    int dx = to_x - from_x;
    int dy = to_y - from_y;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (dx + dy <= 1)
        is_adjacent = 1;
    return is_adjacent;
}

// Decodes a frame length from its two leading bytes, each a base-253 digit plus one, lowest first.
unsigned int Client::DecodeLength(String encoded)
{
    int length;
    try
    {
        length = 0;
        for (int i = 1; i <= encoded.Length(); i++)
        {
            unsigned char raw_byte = encoded[i];
            if (raw_byte == 254 || !raw_byte)
                break;
            int digit = raw_byte;
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
String Client::EncodeNumber(Client *self, unsigned int value, int min_length)
{
    unsigned int remaining = value;
    unsigned int quotient;
    char digit_char;
    int i;
    String result = "";
    try
    {
        do
        {
            double exact_quotient = remaining / 253.0;
            quotient = exact_quotient;
            int remainder = remaining % NUMBER_ENCODING_BASE;
            digit_char = remainder + 1;
            result.Insert(digit_char, result.Length() + 1);
            if (quotient >= 1)
                remaining = quotient;
        } while (quotient >= 1);
    }
    catch (...)
    {
        result = "";
    }
    if (result.Length() < min_length)
    {
        char digit_char = -2;
        int pad_count = min_length - result.Length();
        for (i = 0; i < pad_count; i++)
            result.Insert(digit_char, result.Length() + 1);
    }
    return result;
}

// Decodes up to four base-253 digits, lowest first; the filler byte ends the number.
DEFINE_CLIENT_DECODE_NUMBER(Client)

// The answer the server must give to the client's init challenge; the login and account replies check it.
int Client::HashChallenge(int sent_challenge)
{
    sent_challenge++;
    int mod = sent_challenge % 11 + 1;
    int mod_9 = sent_challenge % 9 + 1;
    int mod_2004 = sent_challenge % 2004 + 1;
    int hash = 11092004 - sent_challenge;
    hash = hash % (119 * mod);
    hash = 119 * (mod_9 * hash);
    hash = mod_2004 + hash + 110904;
    return hash;
}

// Scrambles a packet with the send key, then flips the high bits.
String Client::EncryptPacket(int key, String packet)
{
    String result = "";
    std::string packet_bytes = packet.c_str();
    result = ScrambleBytes(this, key, packet_bytes.begin(), packet_bytes.end());
    result = FlipHighBits(this, result);
    return result;
}

void Packetcontrols_UnusedLiterals2()
{
    "";
    "";
}

// Holds back runs of bytes whose code divides by the key and puts each run out reversed, then reads the result
// alternately from the front and the back.
String Client::ScrambleBytes(Client *self, int divisor, char *data_begin, char *data_end)
{
    int divisor_copy;
    String result = "";
    divisor_copy = divisor;
    try
    {
        std::stack<char > held_chars;
        std::deque<char > reordered_chars;
        for (; data_begin != data_end; data_begin++)
        {
            unsigned char current_byte = *data_begin;
            int char_code = current_byte;
            if (!(char_code % divisor_copy))
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
                result.Insert(reordered_chars.front(), result.Length() + 1);
                reordered_chars.pop_front();
            }
            if (!reordered_chars.empty())
            {
                result.Insert(reordered_chars.back(), result.Length() + 1);
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
String Client::UnscrambleBytes(Client *self, int divisor, unsigned char *data_begin, unsigned char *data_end)
{
    int divisor_copy;
    char to_first_list;
    String result = "";
    divisor_copy = divisor;
    try
    {
        std::stack<char > char_stack;
        std::queue<char > char_queue;
        to_first_list = 1;
        while (data_begin != data_end)
        {
            if (to_first_list)
            {
                char_queue.push(*(char *)data_begin);
                to_first_list = 0;
            }
            else
            {
                char_stack.push(*(char *)data_begin);
                to_first_list = 1;
            }
            data_begin++;
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
            if (!(char_code % divisor_copy))
                char_stack.push(current_char);
            else
            {
                while (!char_stack.empty())
                {
                    result.Insert(char_stack.top(), result.Length() + 1);
                    char_stack.pop();
                }
                result.Insert(current_char, result.Length() + 1);
            }
        }
        while (!char_stack.empty())
        {
            result.Insert(char_stack.top(), result.Length() + 1);
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
String Client::DeobfuscateString(Client *self, String text)
{
    char is_mapped;
    int position;
    int char_code;
    String reversed = "";
    String result = "";
    position = text.Length();
    for (; position >= 1; position--)
        reversed = reversed + text[position];
    for (int i = 1; i <= text.Length(); i++)
    {
        char raw_char = reversed[i];
        unsigned char char_byte = raw_char;
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
            is_mapped = 0;
            if (char_code >= 34 && char_code <= 79)
            {
                int low_mirrored_code;
                char_code = 79 - char_code + 34;
                is_mapped = 1;
                low_mirrored_code = char_code;
                raw_char = low_mirrored_code;
                result.Insert(raw_char, result.Length() + 1);
            }
            if (char_code >= 80 && char_code <= 125)
            {
                int high_mirrored_code;
                char_code = 125 - char_code + 80;
                is_mapped = 1;
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
String Client::FlipHighBits(Client *self, String text)
{
    if (text.Length() >= 1)
    {
        for (int i = 1; i <= text.Length(); i++)
        {
            int byte_value = (unsigned char)text[i];
            if (byte_value < 128)
                text[i] = text[i] + 128;
            if (byte_value > 128)
                text[i] = text[i] + 128;
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
            String packet = BuildPacket(this, send_key, action, family, data);
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

// After the init reply, opens the screen the connection was made for: the create-account form or the login form.
void Client::ShowPendingScreen()
{
    if (MainForm->animation->GetScreenMode() == ScreenMode_Menu)
    {
        if (MainForm->animation->menu->connect_for_create_account)
        {
            MainForm->animation->menu->connect_for_create_account = 0;
            if (MainForm->animation->credits->is_running)
                MainForm->animation->credits->Stop();
            MainForm->edits->Clear();
            MainForm->animation->SetSubState(MenuScreen_CreateAccount);
            MainForm->gfx->LoadCreateAccountScreen();
            MainForm->edits->AddEdit(1, 2, ACCOUNT_FORM_EDIT_X, ACCOUNT_NAME_EDIT_Y, 242, EDIT_LINE_HEIGHT);
            MainForm->edits->AddEdit(3, 2, ACCOUNT_FORM_EDIT_X, ACCOUNT_PASSWORD_EDIT_Y, 242, EDIT_LINE_HEIGHT);
            MainForm->edits->AddEdit(4, 2, ACCOUNT_FORM_EDIT_X, ACCOUNT_CONFIRM_EDIT_Y, 242, EDIT_LINE_HEIGHT);
            MainForm->edits->AddEdit(8, 2, ACCOUNT_FORM_EDIT_X, ACCOUNT_EDIT8_Y, 242, EDIT_LINE_HEIGHT);
            MainForm->edits->AddEdit(9, 2, ACCOUNT_FORM_EDIT_X, ACCOUNT_EDIT9_Y, 242, EDIT_LINE_HEIGHT);
            MainForm->edits->AddEdit(10, 2, ACCOUNT_FORM_EDIT_X, ACCOUNT_EDIT10_Y, 221, EDIT_LINE_HEIGHT);
            MainForm->edits->SetPasswordMode(3, 1);
            MainForm->edits->SetPasswordMode(4, 1);
            MainForm->edits->SetCharFilter(1, 2);
            MainForm->edits->SetCharFilter(3, 2);
            MainForm->edits->SetCharFilter(4, 2);
            MainForm->edits->SetCharFilter(8, 3);
            MainForm->edits->SetCharFilter(9, 3);
            MainForm->edits->SetCharFilter(10, 1);
            MainForm->edits->SetMaxLength(1, 16);
            MainForm->edits->SetMaxLength(3, 12);
            MainForm->edits->SetMaxLength(4, 12);
            MainForm->edits->caret_enabled = false;
            MainForm->animation->menu->OpenTextWindow();
            MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(GameText_ItIsVeryImportant), true);
            MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(GameText_LotOfPlayersWho), true);
            MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(GameText_YourInformationWillOnly),
                false);
            MainForm->edits->FocusEdit(1);
        }
        if (MainForm->animation->menu->connect_for_login)
        {
            MainForm->animation->menu->connect_for_login = 0;
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
                MainForm->edits->AddEdit(1, 2, LOGIN_EDIT_X, LOGIN_NAME_EDIT_Y, 141, EDIT_LINE_HEIGHT);
                MainForm->edits->AddEdit(2, 2, LOGIN_EDIT_X, LOGIN_PASSWORD_EDIT_Y, 141, EDIT_LINE_HEIGHT);
                MainForm->edits->SetCharFilter(1, 2);
                MainForm->edits->SetCharFilter(2, 1);
                MainForm->edits->SetPasswordMode(2, 1);
                MainForm->edits->SetMaxLength(1, 16);
                MainForm->edits->SetMaxLength(2, 12);
                MainForm->edits->FocusEdit(1);
            }
            else
            {
                MainForm->animation->SetSubState(MenuScreen_GridAccountName);
                MainForm->edits->AddEdit(1, 2, LOGIN_EDIT_X, GRID_LOGIN_NAME_EDIT_Y, 161, EDIT_LINE_HEIGHT);
                MainForm->edits->SetCharFilter(1, 2);
                MainForm->edits->SetMaxLength(1, 16);
                MainForm->edits->FocusEdit(1);
            }
        }
    }
}

// On a lost connection, says why: in game a message box, on the menu the reason the server gave or a general
// one. Then it resets the connection state and closes the socket.
void Client::HandleDisconnect()
{
    if (!version_rejected)
    {
        // After a completed init the reasons come from the account replies; before it, from the raw init replies.
        if (is_initialized)
        {
            if (MainForm->animation->GetScreenMode() == ScreenMode_Game)
            {
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_ConnectionLost,
                    MainForm->strings->GetDialogText(DialogText_LostConnectionExit, false),
                    MainForm->strings->GetDialogText(DialogText_LostConnectionExit, true));
                MainForm->edits->caret_enabled = false;
            }
            if (MainForm->animation->GetScreenMode() == ScreenMode_Menu)
            {
                MainForm->animation->menu->SetEnteringGame(0);
                MainForm->animation->menu->is_loading_world = false;
                if (!MainForm->animation->credits->is_running)
                {
                    MainForm->edits->Clear();
                    MainForm->gfx->LoadMainMenuScreen(false);
                    MainForm->animation->SetSubState(MenuScreen_Main);
                }
                char message_shown = 0;
                if (init_rejected)
                {
                    is_initialized = 0;
                    init_rejected = false;
                    message_shown = 1;
                    MainForm->edits->caret_enabled = false;
                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                        MainForm->strings->GetDialogText(DialogText_ServerFull, false),
                        MainForm->strings->GetDialogText(DialogText_ServerFull, true));
                }
                if (is_banned)
                {
                    is_banned = false;
                    is_permanent_ban = false;
                    account_reply_disconnect = false;
                    login_retry_pending = false;
                    is_initialized = 0;
                    MainForm->edits->caret_enabled = false;
                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                        MainForm->strings->GetDialogText(DialogText_AccountIsBlocked, false),
                        MainForm->strings->GetDialogText(DialogText_AccountIsBlocked, true));
                    message_shown = 1;
                }
                if (account_reply_disconnect && !message_shown)
                {
                    is_banned = false;
                    is_permanent_ban = false;
                    account_reply_disconnect = false;
                    login_retry_pending = false;
                    is_initialized = 0;
                    MainForm->edits->caret_enabled = false;
                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                        MainForm->strings->GetDialogText(DialogText_RequestDeniedServer, false),
                        MainForm->strings->GetDialogText(DialogText_RequestDeniedServer, true));
                    message_shown = 1;
                }
                // The login is tried again after a wait, with the same name and password.
                if (login_retry_pending && !message_shown)
                {
                    is_banned = false;
                    is_permanent_ban = false;
                    account_reply_disconnect = false;
                    login_retry_pending = false;
                    is_initialized = 0;
                    MainForm->edits->caret_enabled = false;
                    MainForm->animation->menu->retry_ticks = 500;
                    MainForm->animation->menu->retry_account_name = MainForm->animation->menu->account_name;
                    MainForm->animation->menu->retry_password = MainForm->animation->menu->password;
                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                        MainForm->strings->GetDialogText(DialogText_LoginRefusedServer, false),
                        MainForm->strings->GetDialogText(DialogText_LoginRefusedServer, true));
                    message_shown = 1;
                }
                if (!message_shown)
                {
                    is_initialized = 0;
                    MainForm->edits->caret_enabled = false;
                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                        MainForm->strings->GetDialogText(DialogText_LostConnectionLater, false),
                        MainForm->strings->GetDialogText(DialogText_LostConnectionLater, true));
                }
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
            char menu_message_shown = 0;
            if (init_rejected)
            {
                is_initialized = 0;
                init_rejected = false;
                menu_message_shown = 1;
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
                is_initialized = 0;
                MainForm->sfx->PlaySfx(7);
                if (is_permanent_ban)
                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                        MainForm->strings->GetDialogText(DialogText_ConnectionIsBlockedPermanent, false),
                        MainForm->strings->GetDialogText(DialogText_ConnectionIsBlockedPermanent, true));
                else
                {
                    String ban_minutes_text = " " + IntToStr(ban_minutes) +
                        DeobfuscateString(MainForm->net_client, "C,h+X1d2 ");
                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                        MainForm->strings->GetDialogText(DialogText_ConnectionIsBlockedTemporary, false),
                        MainForm->strings->GetDialogText(DialogText_ConnectionIsBlockedTemporary,
                        true) + ban_minutes_text);
                }
                is_permanent_ban = false;
                menu_message_shown = 1;
            }
            if (!menu_message_shown)
            {
                is_initialized = 0;
                MainForm->edits->caret_enabled = false;
                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                    MainForm->strings->GetDialogText(DialogText_CouldNotFindServer, false),
                    MainForm->strings->GetDialogText(DialogText_CouldNotFindServer, true));
            }
        }
    }
    is_initialized = 0;
    version_rejected = false;
    is_banned = false;
    MainForm->animation->menu->connect_for_create_account = 0;
    MainForm->animation->menu->connect_for_login = 0;
    MainForm->client->Active = false;
}

// Garbles chat text for a drunk player: added letters, starred vowels and hiccups, cut to the chat length.
String Client::MakeDrunkText(Client *self, String text)
{
    if (text.Length() > 0)
    {
        int unused_roll = random(10);
        for (int i = 1; i <= text.Length(); i++)
        {
            char handled = 0;
            if (text[i] == 'e' || text[i] == 'a' || text[i] == 'E' || text[i] == 'A')
            {
                if (i + 1 <= text.Length())
                {
                    if (text[i + 1] != 'e' && text[i + 1] != 'a' && text[i + 1] != 'E' && text[i + 1] != 'A')
                        handled = 1;
                }
                if (i == text.Length())
                    handled = 1;
                if (handled)
                {
                    int j_roll = random(10);
                    if (j_roll < 7)
                        text.Insert("j", i + 1);
                }
            }
            if (text[i] == 'u' || text[i] == 'o' || text[i] == 'U' || text[i] == 'O')
            {
                if (i + 1 <= text.Length())
                {
                    if (text[i + 1] != 'u' && text[i + 1] != 'o' && text[i + 1] != 'U' && text[i + 1] != 'O')
                        handled = 1;
                }
                if (i == text.Length())
                    handled = 1;
                if (handled)
                {
                    int w_roll = random(10);
                    if (w_roll < 7)
                        text.Insert("w", i + 1);
                }
            }
            if (text[i] == 'i')
            {
                handled = 1;
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
                    text[j] = 42;
            }
        }
        for (int k = 1; k <= text.Length(); k++)
        {
            if (text[k] != ' ')
                continue;
            int hic_roll = random(10);
            if (hic_roll < 3)
            {
                text.Insert("*hic* ", k + 1);
                k += 6;
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
String Client::CensorText(Client *self, String text)
{
    char censored_any;
    int i;
    String result = "";
    String lower_text = LowerCase(text);
    if (MainForm->config->GetFilterEnabled())
    {
        censored_any = 0;
        for (i = 0; i < self->word_filter->Count; i++)
        {
            if (self->word_filter->Strings[i].Length() <= lower_text.Length())
            {
                for (int j = 1; j <= lower_text.Length(); j++)
                {
                    if (self->word_filter->Strings[i].Length() > 0)
                    {
                        if (self->word_filter->Strings[i][1] == lower_text[j])
                        {
                            if (j + self->word_filter->Strings[i].Length() - 1 <= lower_text.Length())
                            {
                                if (lower_text.SubString(j, self->word_filter->Strings[i].Length()) ==
                                    self->word_filter->Strings[i])
                                {
                                    lower_text.Delete(j, self->word_filter->Strings[i].Length());
                                    lower_text.Insert("****", j);
                                    text.Delete(j, self->word_filter->Strings[i].Length());
                                    text.Insert("****", j);
                                    censored_any = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
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
String Client::BuildPacket(Client *self, int key, int action, int family, String data)
{
    char action_byte;
    char family_byte;
    char is_valid;
    String packet = "";
    try
    {
        char action_char = action;
        char family_char = family;
        is_valid = 1;
        if (action_char)
            action_byte = action_char;
        else
            is_valid = 0;
        if (family_char)
            family_byte = family_char;
        else
            is_valid = 0;
        if (is_valid)
        {
            String header = "";
            String body = "";
            header = header + action_byte;
            header = header + family_byte;
            body = data;
            packet = packet + header;
            self->sequence_counter++;
            // The sequence number cycles through ten steps above the start the server sent.
            if (self->sequence_counter > 9)
                self->sequence_counter = 0;
            packet = packet + EncodeNumber(self, self->sequence_start + self->sequence_counter, 1);
            packet = packet + body;
            packet = self->EncryptPacket(key, packet);
            unsigned int packet_length = packet.Length();
            packet = EncodeNumber(self, packet_length, 2) + packet;
        }
    }
    catch (...)
    {
        self->sequence_counter--;
        packet = "";
    }
    return packet;
}

int Client::ByteValue(unsigned char ch)
{
    unsigned __int8 value = ch;
    return value;
}

// Whether enough time has passed since the last action, counted in hundredths of a second of the day.
char Client::CanAct()
{
    TTimeStamp now;
    int q;
    int last_tick;
    now = DateTimeToTimeStamp(Now());
    q = now.Time / 10;
    int elapsed = q;
    last_tick = last_action_tick;
    elapsed -= last_tick;
    // Past midnight the count starts again, so a late last action counts as long ago.
    if (elapsed < 0 && last_tick > 7500000)
        elapsed = 44;
    if (elapsed < 44)
        return 0;
    pending_action_tick = q;
    return 1;
}

// Records the time CanAct approved as the last action.
void Client::CommitActionTime()
{
    last_action_tick = pending_action_tick;
}

// Whether text is four dot-separated numbers.
bool Client::IsIpAddress(String text)
{
    bool is_ip;
    int digits_in_part;
    int dot_count;
    int result_scratch = 0;
    char is_valid = 1;
    is_ip = 1;
    digits_in_part = 0;
    dot_count = 0;
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
                        is_ip = 0;
                        is_valid = 0;
                    }
                }
            }
            else
            {
                is_ip = 0;
                is_valid = 0;
            }
        }
        if (dot_count != 3)
            is_ip = 0;
    }
    catch (...)
    {
        is_ip = 0;
    }
    return is_ip;
}
