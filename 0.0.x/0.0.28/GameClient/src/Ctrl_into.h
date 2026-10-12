#ifndef Ctrl_intoH
#define Ctrl_intoH

// The flags of the intro animation, which the animation controller sets as the intro plays.
struct IntroController
{
    bool logo_sound_played;
    bool clear_screen_pending;
    bool logo_redraw_pending;
    bool is_animating;

    IntroController();
    ~IntroController();
};

#endif
