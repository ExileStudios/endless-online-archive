#ifndef Ctrl_bankH
#define Ctrl_bankH

// The banker's window: the gold in the bank and the locker upgrade level.
struct BankController
{
    char close_hovered;
    char is_visible;
    int hovered_row;
    int window_x;
    int window_y;
    int npc_session_id;
    int bank_gold;
    int locker_upgrade_level;
    String reserved_text;
    char is_dragging;
    int drag_offset_x;
    int drag_offset_y;
    char pad_0x2c[4];

    BankController();
    ~BankController();
    void Open(int session_id, int gold_in_bank, int upgrade_level, String unused_text);
    void Close();
    void MouseDown(int x, int y, int button);
    void MouseMove(int x, int y);
    void MouseUp();
};

#endif
