#ifndef Ctrl_statsH
#define Ctrl_statsH

// The stats window: its place on screen, whether it is shown, and the move and close-button state.
struct StatController
{
    char close_hovered;
    char is_visible;
    int window_x;
    int window_y;
    char is_dragging;
    int drag_offset_x;
    int drag_offset_y;
    char pad_0x18[4];

    StatController();
    ~StatController();
    void Open();
    void Close();
    void MouseDown(int x, int y, int button);
    void MouseMove(int x, int y);
    void MouseUp();
};

#endif
