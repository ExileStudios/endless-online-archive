#ifndef Ctrl_jukeboxH
#define Ctrl_jukeboxH

#define JUKEBOX_PLAY_TICKS 800

// The jukebox window: the song picked, whether a song is playing, and who asked for it.
struct JukeboxController
{
    char close_hovered;
    char is_visible;
    int hovered_row;
    int window_x;
    int window_y;
    int open_reply_number;
    char is_playing;
    int play_ticks_left;
    String requester_name;
    int selected_song;
    char is_dragging;
    int drag_offset_x;
    int drag_offset_y;
    char pad_0x30[4];

    JukeboxController();
    ~JukeboxController();
    void Tick();
    void Open(int reply_number, char playing_flag, int reserved, String requester);
    void Close();
    void MouseDown(int x, int y, int button);
    void MouseMove(int x, int y);
    void MouseUp();
};

#endif
