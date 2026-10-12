#ifndef MsgboardcontrolH
#define MsgboardcontrolH

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
#define BOARD_COUNT 8
#define BOARD_POST_SLOTS 32

struct MsgBoard;

#include "Msgboard.h"

struct MsgBoardController;

// The 8 message boards, kept in pub/dbb001.ebf.
struct MsgBoardController
{
    int post_limit;
    int last_post_id;
    vector<MsgBoard> boards[BOARD_COUNT];
    String misc_text;
    int read_pos;
    int read_len;
    char read_break;
    char *encode_scratch;
    char board_enabled[BOARD_COUNT];
    String board_names[8];
    int poster_lengths[BOARD_POST_SLOTS];
    int subject_lengths[32];
    int message_lengths[32];
    int extra_lengths[BOARD_COUNT];
    char pad_0x2e4[96];
    String extra_strings[32];

    MsgBoardController();
    ~MsgBoardController();
    bool LoadBoards();
    void SaveBoards();
    void ClearBoard(int board);
    void DeletePost(int board, int post_id);
    int CountPosts(int board, String author);
    void AddPost(int board, String poster, String subject, String message, char flag);
    String GetBoard(int board);
    String GetPost(int board, int post_id);
    void BuildBoardList(signed int board);
    String BuildBoardData(int board);
    void LoadBoard(int board, String data);
    void SetDecodeSource(String data, char delimiter);
    String ReadToken();
    String ReadRest();
    int DecodeNumber(String value);
    String EncodeNumber(unsigned int value, signed int width);
};

#endif
