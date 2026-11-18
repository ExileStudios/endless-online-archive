#include <vcl.h>
#pragma hdrstop

#include "Ctrl_into.h"

#pragma package(smart_init)

IntroController::IntroController()
{
    logo_sound_played = false;
    clear_screen_pending = false;
    logo_redraw_pending = false;
    is_animating = false;
}

IntroController::~IntroController()
{
}
