#ifndef Ctrl_barberH
#define Ctrl_barberH

// The barber window: a preview of the player with the new hair, and the price.
struct BarberController
{
    bool accept_hovered;
    char close_hovered;
    char is_visible;
    int hovered_column;
    int hovered_row;
    int window_x;
    int window_y;
    bool previous_hovered;
    bool next_hovered;
    bool reset_hovered;
    int npc_session_id;
    int base_price;
    int price_per_level;
    int style_page;
    short style_count;
    int hair_style;
    int hair_color;
    int price;
    int preview_direction;
    char is_dragging;
    int drag_offset_x;
    int drag_offset_y;
    char unused_bytes[4];

    BarberController();
    ~BarberController();
    void Open(int session_id, int styles, int base_cost, int level_cost);
    void Close();
    void MouseDown(int x, int y, int button);
    void MouseMove(int x, int y);
    void MouseUp();
};

#endif
