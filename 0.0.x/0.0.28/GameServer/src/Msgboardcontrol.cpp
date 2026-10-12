#include <vcl.h>
#pragma hdrstop

#include "Msgboardcontrol.h"
#include "Msgboard.h"

#pragma package(smart_init)

#include <fstream.h>
#include "Constants.h"

// Loads the boards; a board shows at most post_limit ( 24 ) posts.
MsgBoardController::MsgBoardController()
{
    encode_scratch = new char[8];
    post_limit = 24;
    last_post_id = 0;
    for (int i = 0; i < BOARD_COUNT; i++)
    {
        boards[i].clear();
        board_enabled[i] = 1;
    }
    LoadBoards();
}

// Saves the boards on the way out.
MsgBoardController::~MsgBoardController()
{
    SaveBoards();
    for (int i = 0; i < BOARD_COUNT; i++)
    {
        boards[i].clear();
        board_enabled[i] = 1;
    }
}

// Reads pub/dbb001.ebf: the 8 boards' lengths ( 4 bytes each ), then each board's data.
bool MsgBoardController::LoadBoards()
{
    String path;
    String unused;
    int file_handle;
    try
    {
        int size;
        char *file_buffer;
        path = "./pub/dbb001.ebf";
        file_handle = FileOpen(path.c_str(), 0);
        if (file_handle < 0)
            return false;
        size = FileSeek(file_handle, 0, 2);
        FileSeek(file_handle, 0, 0);
        file_buffer = new char[size + 1];
        FileRead(file_handle, file_buffer, size);
        FileClose(file_handle);
        path = file_buffer;
        path.SetLength(size);
        delete[] file_buffer;
        for (int i = 0; i < BOARD_COUNT; i++)
        {
            boards[i].clear();
            board_enabled[i] = 1;
        }
        for (int i = 0; i < BOARD_COUNT; i++)
        {
            extra_lengths[i] = DecodeNumber(path.SubString(1, 4));
            path.Delete(1, 4);
        }
        for (int i = 0; i < BOARD_COUNT; i++)
        {
            extra_strings[i] = path.SubString(1, extra_lengths[i]);
            path.Delete(1, extra_lengths[i]);
        }
        for (int i = 0; i < BOARD_COUNT; i++)
            LoadBoard(i + 1, extra_strings[i]);
    }
    catch (...)
    {
        FileClose(file_handle);
        return false;
    }
    return true;
}

// Writes the 8 boards back to pub/dbb001.ebf.
void MsgBoardController::SaveBoards()
{
    int i;
    String lengths = "";
    String contents = "";
    String board_data = "";
    for (i = 1; i <= 8; i++)
    {
        board_data = BuildBoardData(i);
        lengths.Insert(EncodeNumber(board_data.Length(), 4), lengths.Length() + 1);
        contents.Insert(board_data, contents.Length() + 1);
    }
    lengths.Insert(contents, lengths.Length() + 1);
    String path = "./pub/dbb001.ebf";
    std::ofstream file;
    file.open(path.c_str(), std::ios::binary);
    file << lengths.c_str();
    file.close();
}

void MsgBoardController::ClearBoard(int board)
{
    if (board >= 1 && board <= BOARD_COUNT)
    {
        board = board - 1;
        boards[board].clear();
        board_enabled[board] = 1;
    }
}

// Removes a post from a board.
void MsgBoardController::DeletePost(int board, int post_id)
{
    if (board >= 1 && board <= BOARD_COUNT)
    {
        board = board - 1;
        for (vector<MsgBoard>::iterator it = boards[board].begin(); it != boards[board].end(); it++)
        {
            if (it->id == post_id)
            {
                boards[board].erase(it);
                board_enabled[board] = 1;
                return;
            }
        }
    }
}

// How many of the board's posts are the author's.
int MsgBoardController::CountPosts(int board, String author)
{
    int count = 0;
    if (board >= 1 && board <= BOARD_COUNT)
    {
        board = board - 1;
        for (vector<MsgBoard>::iterator it = boards[board].begin(); it != boards[board].end(); it++)
        {
            if (author == it->poster)
                count++;
        }
    }
    return count;
}

// Adds a post: first on the board ( flag 0 ), or last ( flag 1, when loading ); a board keeps post_limit + 4.
void MsgBoardController::AddPost(int board, String poster, String subject, String message, char flag)
{
    if (board >= 1 && board <= BOARD_COUNT)
    {
        last_post_id++;
        if (last_post_id > 40000)
            last_post_id = 0;
        MsgBoard post(last_post_id, poster, subject, message);
        board = board - 1;
        if (!flag)
            boards[board].insert(boards[board].begin(), post);
        else
            boards[board].insert(boards[board].end(), post);
        board_enabled[board] = 1;
        if (boards[board].size() > (unsigned int)(post_limit + 4))
            boards[board].resize(post_limit + 4);
    }
}

// The board's list as the client gets it; it is built again only after a change.
String MsgBoardController::GetBoard(int board)
{
    String result;
    if (board >= 1 && board <= 8)
    {
        if (board_enabled[board - 1])
        {
            BuildBoardList(board);
            board_enabled[board - 1] = 0;
        }
        result = board_names[board - 1];
    }
    return result;
}

// A post's id and message.
String MsgBoardController::GetPost(int board, int post_id)
{
    String result;
    vector<MsgBoard>::iterator it;
    if (board >= 1 && board <= BOARD_COUNT)
    {
        board = board - 1;
        int count = 0;
        for (it = boards[board].begin(); it != boards[board].end() && count < post_limit; it++)
        {
            if (it->id == post_id)
            {
                result = EncodeNumber(it->id, 2);
                result.Insert(it->message, result.Length() + 1);
                break;
            }
        }
    }
    return result;
}

// Builds the board's list: its number and count, then each post's id, poster and subject.
void MsgBoardController::BuildBoardList(signed int board)
{
    int count;
    vector<MsgBoard>::iterator it;
    if (board >= 1 && board <= BOARD_COUNT)
    {
        count = (int)boards[board - 1].size();
        if (count > post_limit)
            count = post_limit;
        String result = EncodeNumber(board, 1);
        result.Insert(EncodeNumber(count, 1), result.Length() + 1);
        board = board - 1;
        int i = 0;
        for (it = boards[board].begin(); it != boards[board].end() && i < post_limit; it++)
        {
            result.Insert(EncodeNumber(it->id, 2), result.Length() + 1);
            result.Insert((char)EO_BREAK_BYTE, result.Length() + 1);
            result.Insert(it->poster, result.Length() + 1);
            result.Insert((char)EO_BREAK_BYTE, result.Length() + 1);
            result.Insert(it->subject, result.Length() + 1);
            result.Insert((char)EO_BREAK_BYTE, result.Length() + 1);
            i++;
        }
        board_names[board] = result;
    }
}

// A board the way it is saved: the count, the three lengths of each post, then all their texts.
String MsgBoardController::BuildBoardData(int board)
{
    String text = "";
    String result = "";
    if (board >= 1 && board <= BOARD_COUNT)
    {
        board = board - 1;
        result = EncodeNumber((int)boards[board].size(), 2);
        result.Insert((char)EO_BREAK_BYTE, result.Length() + 1);
        vector<MsgBoard>::iterator it;
        for (it = boards[board].begin(); it != boards[board].end(); it++)
        {
            String poster = it->poster;
            String subject = it->subject;
            String message = it->message;
            text.Insert(poster, text.Length() + 1);
            text.Insert(subject, text.Length() + 1);
            text.Insert(message, text.Length() + 1);
            result.Insert(EncodeNumber(poster.Length(), 2), result.Length() + 1);
            result.Insert((char)EO_BREAK_BYTE, result.Length() + 1);
            result.Insert(EncodeNumber(subject.Length(), 2), result.Length() + 1);
            result.Insert((char)EO_BREAK_BYTE, result.Length() + 1);
            result.Insert(EncodeNumber(message.Length(), 2), result.Length() + 1);
            result.Insert((char)EO_BREAK_BYTE, result.Length() + 1);
        }
        result.Insert(text, result.Length() + 1);
    }
    return result;
}

// Reads one board's saved data back into posts.
void MsgBoardController::LoadBoard(int board, String data)
{
    if (board >= 1 && board <= BOARD_COUNT && data.Length() > 0)
    {
        ClearBoard(board);
        board = board - 1;
        board_enabled[board] = 1;
        SetDecodeSource(data, -1);
        int count = DecodeNumber(ReadToken());
        int total = 0;
        for (int i = 0; i < count && i < BOARD_POST_SLOTS; i++)
        {
            int poster_length = DecodeNumber(ReadToken());
            int subject_length = DecodeNumber(ReadToken());
            int message_length = DecodeNumber(ReadToken());
            poster_lengths[i] = poster_length; subject_lengths[i] = subject_length; message_lengths[i] = message_length;
            total = total + poster_length + subject_length + message_length;
        }
        String text = ReadRest();
        if (text.Length() >= total && total > 0)
        {
            for (int j = 0; j < count && j < BOARD_POST_SLOTS; j++)
            {
                String poster = text.SubString(1, poster_lengths[j]);
                text.Delete(1, poster_lengths[j]);
                String subject = text.SubString(1, subject_lengths[j]);
                text.Delete(1, subject_lengths[j]);
                String message = text.SubString(1, message_lengths[j]);
                text.Delete(1, message_lengths[j]);
                AddPost(board + 1, poster, subject, message, 1);
            }
        }
    }
}

// Starts reading data field by field, each field ending at the delimiter.
void MsgBoardController::SetDecodeSource(String data, char delimiter)
{
    read_pos = 1;
    misc_text = data;
    read_len = data.Length();
    read_break = delimiter;
}

void Msgboardcontrol_UnusedLiterals1()
{
    "";
    "";
    "";
}

// The next field; the delimiter after it is skipped.
String MsgBoardController::ReadToken()
{
    String result = "";
    try
    {
        if (read_len >= 1)
        {
            while (read_pos <= read_len)
            {
                if (misc_text[read_pos] != read_break)
                    result.Insert(misc_text[read_pos], result.Length() + 1);
                else
                {
                    read_pos++;
                    break;
                }
                read_pos++;
            }
        }
    }
    catch (...)
    {
        result = "";
    }
    return result;
}

// Everything that is left to read.
String MsgBoardController::ReadRest()
{
    String result = "";
    try
    {
        if (read_len >= 1)
        {
            while (read_pos <= read_len)
            {
                result.Insert(misc_text[read_pos], result.Length() + 1);
                read_pos++;
            }
        }
    }
    catch (...)
    {
        result = "";
    }
    return result;
}

DEFINE_MAP_DECODE_NUMBER(MsgBoardController)

// Writes a number the protocol's way, base 253 and padded with 254; a number too big for width gets more bytes.
DEFINE_ENCODE_NUMBER(MsgBoardController)
