#ifndef Gfx_editsH
#define Gfx_editsH

#include <Classes.hpp>

// Draws the game's own edit boxes onto the text canvas; it keeps the wrapped lines of a multi-line box.
struct EditCanvas
{
    TStringList *lines;

    EditCanvas();
    ~EditCanvas();
    void DrawEditBoxes(int screen_id);
};

#endif
