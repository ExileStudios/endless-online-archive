#ifndef Ctrl_visualdocH
#define Ctrl_visualdocH

struct VisualDocController
{
    bool close_hovered;
    bool is_visible;
    int window_x;
    int window_y;
    bool scrolls_x;
    bool scrolls_y;
    int image_width;
    int image_height;
    int scroll_x;
    int scroll_y;
    int view_left;
    int view_top;
    int view_right;
    int view_bottom;
    int view_width;
    int view_height;
    short arrow_blink[4];
    char unused_bytes[1];
    bool is_dragging_window;
    int drag_offset_x;
    int drag_offset_y;
    bool is_dragging_image;
    int drag_scroll_x;
    int drag_scroll_y;
    int drag_start_x;
    int drag_start_y;

    VisualDocController();
    ~VisualDocController();
    TRect GetArrowRect(int arrow, int dimmed);
    void Tick();
    bool Open(int document_id);
    void Close();
    void MouseDown(int x, int y, int button);
    void MouseMove(int x, int y);
    void MouseUp();
    void Draw();
};

#endif
