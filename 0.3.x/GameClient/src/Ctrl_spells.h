#ifndef Ctrl_spellsH
#define Ctrl_spellsH

#include <new>
#include <vector.h>

struct Tooltip;

#include "Itemspell.h"

enum CastTarget
{
    CastTarget_None = 0,
    CastTarget_Npc = 1,
    CastTarget_Player = 2,
    CastTarget_Group = 3
};

// The spell panel: the known spells on a grid that scrolls by rows, the selected spell waiting for a target, and the
// cast in progress. The grid layout is kept in the registry.
struct SpellController
{
    vector<SpellItem> spells;
    vector<SpellItem> known_spells;
    vector<SpellItem> saved_slots;
    Tooltip *tooltip;
    int hover_spell_id;
    int hover_col;
    int hover_row;
    int last_hover_col;
    int last_hover_row;
    char has_hover;
    char hover_alt_frame;
    int hover_text_width;
    char unused_bytes[4];
    bool awaiting_target;
    String selected_spell_name;
    int selected_spell_id;
    int selected_spell_icon;
    int selected_spell_category;
    int selected_spell_type;
    int selected_spell_level;
    int selected_target_kind;
    int clicked_spell_id;
    int clicked_col;
    int clicked_row;
    int selected_text_width;
    int drag_x;
    int drag_y;
    int drag_icon;
    char second_unused_bytes[4];
    int drag_spell_id;
    char is_dragging;
    char drag_started;
    int last_mouse_x;
    int last_mouse_y;
    int drag_delay_ticks;
    char unused_reset_flag;
    char scroll_up_hovered;
    char scroll_down_hovered;
    int train_confirm_ticks;
    int train_spell_id;
    char cast_is_instant;
    int cast_spell_id;
    int cast_target_kind;
    int cast_target_id;
    int cast_ticks_left;
    bool is_casting;
    char third_unused_bytes[23];
    char selection_dirty;
    char is_dirty;
    char drag_dirty;
    char thumb_grabbed;
    int thumb_grab_offset;
    int click_ticks;
    int click_col;
    int click_row;
    int hover_message_delay;
    int use_cooldown;
    char buttons_dirty;
    int top_row;
    int scroll_thumb_offset;
    char scroll_up_held;
    char scroll_down_held;
    int scroll_repeat_delay;
    int blink_phase;

    SpellController();
    ~SpellController();
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
    void AddKnownSpell(int spell_id, int level);
    bool SetSpellLevel(int spell_id, int level);
    bool RemoveSpell(int spell_id);
    char HasSpell(int spell_id);
    char AddSpell(int spell_id, int level);
    void RestoreLayout();
    void UseSlot(int col, int row);
    void UseSpell(int col, int row, int spell_id);
    bool IsCasting();
    void BeginCast(int spell_id, int target_kind, int target_id, char is_instant);
    void CancelCast();
    void ClearSelection();
    int GetBlinkPhase();
    void Reset();
};

#endif
