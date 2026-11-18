#ifndef Ctrl_musicianH
#define Ctrl_musicianH

#define MUSICIAN_CELL_SIZE 20

// The musician window: a grid of notes to play on the instrument held.
struct MusicianController
{
    char close_hovered;
    char is_visible;
    char unused_bytes[6];
    int window_x;
    int window_y;
    int hovered_note_col;
    int hovered_note_row;
    int unused_index;
    int note_cooldown;
    char is_dragging;
    int drag_offset_x;
    int drag_offset_y;
    char second_unused_bytes[4];

    MusicianController();
    ~MusicianController();
    void Tick();
    void Open();
    void Close();
    void MouseDown(int x, int y, int button);
    void MouseMove(int x, int y);
    void MouseUp();
};

#endif
