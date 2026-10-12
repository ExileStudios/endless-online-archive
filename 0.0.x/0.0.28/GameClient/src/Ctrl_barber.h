#ifndef Ctrl_barberH
#define Ctrl_barberH

// The barber window: a preview of the player with the new hair, and the price.
struct BarberController
{
    char close_hovered;
    char is_visible;
    int hovered_row;
    int window_x;
    int window_y;
    int npc_session_id;
    int hair_style;
    int hair_color;
    int price;
    int preview_direction;
    char is_dragging;
    int drag_offset_x;
    int drag_offset_y;
    char pad_0x30[4];

    BarberController();
    ~BarberController();
    void Open(int session_id);
    void Close();
    void MouseDown(int x, int y, int button);
    void MouseMove(int x, int y);
    void MouseUp();
};

#endif
