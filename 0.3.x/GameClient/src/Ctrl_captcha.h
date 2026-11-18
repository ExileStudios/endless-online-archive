#ifndef Ctrl_captchaH
#define Ctrl_captchaH

struct CaptchaController
{
    bool ok_hovered;
    bool is_visible;
    int window_x;
    int window_y;
    char letter_offsets[7];
    unsigned char frame;
    unsigned char hidden_letter;
    unsigned char second_hidden_letter;
    unsigned char hide_counter;
    unsigned char decoy_counter;
    int captcha_id;
    String answer;
    String shifted_answer;
    int reward;
    char shift;
    unsigned char difficulty;
    unsigned char misses;
    bool music_paused;
    char unused_bytes[1];
    bool is_dragging;
    int drag_offset_x;
    int drag_offset_y;

    CaptchaController();
    ~CaptchaController();
    void Update();
    void Draw();
    bool Show(int new_captcha_id, int new_reward);
    String RandomAnswer();
    bool Open(int new_captcha_id, String new_answer, int new_reward);
    void SetAnswer(int id, String text);
    void Close();
    void Submit();
    void MouseDown(int x, int y, int button);
    void MouseMove(int x, int y);
    void MouseUp();
};

#endif
