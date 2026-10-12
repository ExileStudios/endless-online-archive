#ifndef Ctrl_menuH
#define Ctrl_menuH

#include <Classes.hpp>
#define DEFAULT_FORM_X 140
#define DEFAULT_FORM_Y 120
#define DEFAULT_MESSAGE_BOX_X 175
#define DEFAULT_MESSAGE_BOX_Y 166
#define MESSAGE_BOX_LINE_COUNT 2

enum DraggedWindow
{
    DraggedWindow_MessageBox = 1,
    DraggedWindow_Form = 2,
    DraggedWindow_TextWindow = 3
};

enum MessageBoxKind
{
    MessageBoxKind_Plain = 1,
    MessageBoxKind_Progress = 2,
    MessageBoxKind_Confirm = 3
};

// The title screens before the game: account form, text window, message box and password letter pad.
// It also tracks which map and data files still have to be downloaded.
struct MenuController
{
    char connect_for_create_account;
    char connect_for_login;
    int new_gender;
    int new_hair_style;
    int new_hair_color;
    int new_skin;
    bool create_account_button_hover;
    bool play_button_hover;
    bool credits_button_hover;
    bool exit_button_hover;
    bool message_close_button_hover;
    bool message_confirm_button_hover;
    bool form_ok_button_hover;
    bool form_cancel_button_hover;
    bool text_window_ok_button_hover;
    bool text_scroll_up_hover;
    bool text_scroll_down_hover;
    bool account_create_button_hover;
    bool account_cancel_button_hover;
    bool change_password_button_hover;
    bool new_character_button_hover;
    bool first_slot_login_hover;
    bool second_slot_login_hover;
    bool third_slot_login_hover;
    bool first_slot_delete_hover;
    bool second_slot_delete_hover;
    bool third_slot_delete_hover;
    bool gender_arrow_hover;
    bool hair_style_arrow_hover;
    bool hair_color_arrow_hover;
    bool skin_arrow_hover;
    bool corner_close_hover;
    bool login_ok_button_hover;
    bool login_cancel_button_hover;
    bool unread_flag_at_48;
    int unread_int_at_52;
    bool create_account_enabled;
    bool login_enabled;
    bool change_password_enabled;
    bool create_character_enabled;
    int character_select_cooldown;
    int delete_character_id;
    int delete_armed_slot;
    int selected_character_id;
    bool is_message_open;
    bool message_shows_image;
    int message_x;
    int message_y;
    String message_title;
    String message_lines[MESSAGE_BOX_LINE_COUNT];
    int message_kind;
    TStringList *text_lines;
    bool is_text_window_open;
    int text_scroll_offset;
    int text_scrollbar_position;
    char scrolling_up;
    char scrolling_down;
    int scroll_repeat_delay;
    int text_window_x;
    int text_window_y;
    bool is_form_open;
    int form_x;
    int form_y;
    int form_kind;
    bool is_loading_world;
    int loading_frame;
    int loading_tip;
    String account_name;
    String password;
    int grid_column;
    int grid_row;
    int cursor_blink;
    int letter_delay_ticks;
    String retry_account_name;
    String retry_password;
    int retry_ticks;
    bool up_key_lit;
    bool down_key_lit;
    bool left_key_lit;
    bool right_key_lit;
    bool space_key_lit;
    bool select_key_lit;
    bool delete_key_lit;
    char is_dragging_window;
    int dragged_window;
    int drag_offset_x;
    int drag_offset_y;
    char is_dragging_thumb;
    int thumb_grab_offset;
    int request_delay_ticks;
    char is_entering_game;
    char map_needed;
    char item_file_needed;
    char reserved_file_needed;
    char class_file_needed;
    char npc_file_needed;
    char skill_file_needed;

    MenuController();
    ~MenuController();
    void OnTimer();
    void OpenFormWindow(int kind);
    void OpenTextWindow();
    void AddWrappedText(String text, bool add_blank_line);
    int GetScrollOffset();
    int GetScrollbarPosition();
    void ShowMessageBox(int kind, String title, String text);
    void SetEnteringGame(char entering);
    char IsEnteringGame();
    void StartRequestDelay();
    bool IsRequestDelayDone();
    void SetMapNeeded(char needed);
    void SetReservedFileNeeded(char needed);
    void SetClassFileNeeded(char needed);
    void SetItemFileNeeded(char needed);
    void SetNpcFileNeeded(char needed);
    void SetSkillFileNeeded(char needed);
    char IsMapNeeded();
    char IsReservedFileNeeded();
    char IsClassFileNeeded();
    char IsItemFileNeeded();
    char IsNpcFileNeeded();
    char IsSkillFileNeeded();
    void OnMouseDown(int mouse_x, int mouse_y);
    void OnMouseMove(int mouse_x, int mouse_y);
    void UpdateScrollbarHover(int mouse_x, int mouse_y);
    void DragScrollbar(int mouse_y);
    void ScrollUp();
    void ScrollDown();
    void AddSelectedLetter();
    void AddSpace();
    void DeleteLastLetter();
    void StopDragging();
};

#endif
