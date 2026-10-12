#ifndef Ctrl_skillsH
#define Ctrl_skillsH

#include <vector.h>
#define GRID_THUMB_TRAVEL 68.0
#define SLOT_GRID_CELL_HEIGHT 52
#define SLOT_GRID_CELL_WIDTH 45

#include "Itemskill.h"

// The skill panel: the known skills on a grid that scrolls by rows, and the skill selected for training.
// The grid layout is kept in the registry.
struct SkillController
{
    vector<SkillItem> skills;
    vector<SkillItem> unused_items_a;
    vector<SkillItem> unused_items_b;
    String hover_text;
    int hover_skill_id;
    int hover_col;
    int hover_row;
    int last_hover_col;
    int last_hover_row;
    char has_hover;
    char hover_alt_frame;
    int hover_text_width;
    char pad_0x80[4];
    bool has_selection;
    String selected_skill_name;
    int selected_skill_id;
    int selected_skill_icon;
    int selected_skill_category;
    int selected_skill_level;
    int clicked_skill_id;
    int clicked_col;
    int clicked_row;
    int selected_text_width;
    int drag_x;
    int drag_y;
    int drag_icon;
    char pad_0xb8[4];
    int drag_skill_id;
    char is_dragging;
    char drag_started;
    int last_mouse_x;
    int last_mouse_y;
    int drag_delay_ticks;
    char unused_reset_flag;
    char scroll_up_hovered;
    char scroll_down_hovered;
    int train_confirm_ticks;
    int train_skill_id;
    char pad_0xdc[20];
    char selection_dirty;
    char is_dirty;
    char drag_dirty;
    char thumb_grabbed;
    int thumb_grab_offset;
    int click_ticks;
    int click_col;
    int click_row;
    int hover_message_delay;
    char buttons_dirty;
    int top_row;
    int scroll_thumb_offset;
    char scroll_up_held;
    char scroll_down_held;
    int scroll_repeat_delay;
    int blink_phase;

    SkillController();
    ~SkillController();
    int GetTopRow();
    int GetScrollThumbOffset();
    void Tick();
    void ScrollUp();
    void ScrollDown();
    void DragScrollThumb(int y);
    void MouseMove(int x, int y);
    void UpdateScrollHover(int x, int y);
    void UpdateHover(int x, int y);
    void MouseDown(int x, int y, int button);
    void MouseUp();
    bool TestAndClearButtonsDirty();
    bool TestAndClearDirty();
    bool TestAndClearDragDirty();
    void MarkSelectionDirty();
    void MarkDirty();
    void MarkDragDirty();
    void CancelDrag();
    void SaveLayout();
    bool SetSkillLevel(int skill_id, int level);
    bool RemoveSkill(int skill_id);
    char HasSkill(int skill_id);
    int GetBlinkPhase();
    void Reset();
};

// Follows a grabbed scroll thumb, kept on its track, and picks the top row from its position.
// Shared body: a class defines its DragScrollThumb with DEFINE_SKILL_DRAG_SCROLL_THUMB(TheClass); each class has its
// own thumb_grabbed, thumb_grab_offset, top_row, scroll_thumb_offset, buttons_dirty, is_dirty.
#define DEFINE_SKILL_DRAG_SCROLL_THUMB(Class) \
void Class::DragScrollThumb(int y) \
{ \
    double thumb_fraction; \
    double scaled_row; \
    int __slot1; \
    int thumb_offset; \
    int new_top_row; \
    if (thumb_grabbed) \
    { \
        thumb_offset = y - thumb_grab_offset; \
        int row_count = 4; \
        if (thumb_offset < 0) \
            thumb_offset = 0; \
        if (thumb_offset > 68) \
            thumb_offset = 68; \
        if (row_count > 2) \
        { \
            thumb_fraction = thumb_offset / GRID_THUMB_TRAVEL; \
            scaled_row = (row_count - 2.0) * thumb_fraction; \
            new_top_row = scaled_row; \
            if (new_top_row != top_row) \
                top_row = new_top_row; \
        } \
        scroll_thumb_offset = thumb_offset; \
        buttons_dirty = 1; \
        is_dirty = 1; \
    } \
}

// Scrolls the grid down one row and moves the scroll thumb to match.
// Shared body: a class defines its ScrollDown with DEFINE_SKILL_SCROLL_DOWN(TheClass); each class has its own top_row,
// scroll_thumb_offset, buttons_dirty, is_dirty.
#define DEFINE_SKILL_SCROLL_DOWN(Class) \
void Class::ScrollDown() \
{ \
    double scroll_fraction; \
    double thumb_pos; \
    int row_count = 4; \
    if (top_row < row_count - 2) \
    { \
        top_row++; \
        scroll_fraction = (double)top_row / (double)(row_count - 2); \
        thumb_pos = GRID_THUMB_TRAVEL * scroll_fraction; \
        scroll_thumb_offset = thumb_pos; \
        buttons_dirty = 1; \
        is_dirty = 1; \
    } \
}

// Scrolls the grid up one row and moves the scroll thumb to match.
// Shared body: a class defines its ScrollUp with DEFINE_SKILL_SCROLL_UP(TheClass); each class has its own top_row,
// scroll_thumb_offset, buttons_dirty, is_dirty.
#define DEFINE_SKILL_SCROLL_UP(Class) \
void Class::ScrollUp() \
{ \
    double scroll_fraction; \
    double thumb_pos; \
    int row_count = 4; \
    if (row_count > 2 && top_row > 0) \
    { \
        top_row--; \
        scroll_fraction = (double)top_row / (double)(row_count - 2); \
        thumb_pos = GRID_THUMB_TRAVEL * scroll_fraction; \
        scroll_thumb_offset = thumb_pos; \
        buttons_dirty = 1; \
        is_dirty = 1; \
    } \
}

#endif
