#ifndef Ctrl_notificationH
#define Ctrl_notificationH

enum NotifyStatus
{
    NotifyStatus_Ready = 0,
    NotifyStatus_NotHeld = 1,
    NotifyStatus_NoEffect = 2,
    NotifyStatus_NoMana = 3
};

enum NotifyKind
{
    NotifyKind_Item = 1,
    NotifyKind_Spell = 2
};

struct Notification
{
    bool is_visible;
    int x;
    int y;
    int kind;
    int status;
    int id;
    int icon_size;
    int graphic;
    String amount_text;
    unsigned char amount_kind;
    String second_amount_text;
    int hide_countdown;
    int show_delay;

    Notification(int left, int top);
    ~Notification();
    void Tick();
    void Notify(int new_kind, int new_id);
    void NotifyWithStatus(int new_kind, int new_id, int new_status);
    String FormatAmount(int amount, String prefix);
    void Hide();
    bool IsVisible();
    void Draw();
    void DrawAmounts();
};

#endif
