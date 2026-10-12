#ifndef Ctrl_msgboardH
#define Ctrl_msgboardH

#include <vector.h>
#define POST_MESSAGE_EDIT 21

#include "Sys_msg.h"

// The message board window: the board's post list, one post read in the subject and body edits, or a new post
// being written, with a scroll bar for the list or the body.
struct MsgBoardController
{
    bool action_button_hovered;
    char close_hovered;
    char is_visible;
    int hovered_row;
    int window_x;
    int window_y;
    int view_mode;
    int board_id;
    int current_post_id;
    String current_post_author;
    String current_post_subject;
    String current_post_body;
    char unused_cleared_flag;
    bool is_composing;
    char scroll_up_hovered;
    char scroll_down_hovered;
    char pad_0x2c[4];
    int next_local_post_id;
    char pad_0x34[4];
    vector<Msg> posts;
    char scroll_changed;
    char pad_0x59[7];
    int top_line;
    int scroll_thumb_offset;
    char scroll_up_held;
    char scroll_down_held;
    char thumb_grabbed;
    int thumb_grab_offset;
    char is_dragging;
    int drag_offset_x;
    int drag_offset_y;
    int scroll_repeat_delay;
    char pad_0x80[8];

    MsgBoardController();
    ~MsgBoardController();
    void CreatePostEdits();
    void HidePostEdits();
    void Open(int new_board_id);
    void Close();
    void Tick();
    int GetScrollThumbOffset();
    int GetTopLine();
    void ResetScroll();
    void ClearPosts();
    void SetPostBody(String reply_data);
    void LoadPosts(String reply_data);
    void SubmitPost(String subject, String body);
    int CountPostsByAuthor(String author);
    void DeletePost(int post_id);
    void ReadPost(int index);
    void MouseDown(int x, int y, int button);
    void MouseMove(int x, int y);
    void UpdateScrollHover(int x, int y);
    void DragScrollThumb(int y);
    void ScrollUp();
    void ScrollDown();
    void MouseUp();
};

#endif
