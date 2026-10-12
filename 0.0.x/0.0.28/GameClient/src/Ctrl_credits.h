#ifndef Ctrl_creditsH
#define Ctrl_creditsH

#include <Classes.hpp>

// The credits that scroll on the title screen, read from data\dat001.edf.
struct CreditsController
{
    TStringList *shown_styles;
    TStringList *shown_y;
    TStringList *shown_lines;
    TStringList *credit_texts;
    TStringList *credit_styles;
    bool is_running;
    int top_y;
    int start_y;
    int tick_counter;
    int next_line_index;
    int data_file_size;
    int vowel_count;

    CreditsController();
    ~CreditsController();
    void LoadCredits();
    bool VerifyCredits();
    void Stop();
    void Start();
};

#endif
