#ifndef Sys_npcsH
#define Sys_npcsH

#include <new>
#include <vector.h>

#include "Sys_npc.h"

#define MAX_NPC_GRAPHICS 240

struct FrameOffset
{
    short x;
    short y;
};

// The npcs on the map, with their animations, frame cells and per-graphic tables.
struct Npcs
{
    FrameOffset frame_offsets[MAX_NPC_GRAPHICS];
    FrameOffset attack_frame_offsets[MAX_NPC_GRAPHICS];
    char has_idle_animation[MAX_NPC_GRAPHICS];
    short head_heights[MAX_NPC_GRAPHICS];
    char hover_label_shown;
    vector<Npc *> npcs;
    int current_index;
    int mouse_x;
    int mouse_y;

    Npcs(char preload_option);
    ~Npcs();
    void Animate();
    void RemoveExpiredNpcs();
    int GetNpcId(int index);
    bool ConsumeRedraw();
    bool IsInViewRange(int center_x, int center_y, int x, int y);
    void SortByPosition();
    bool HasNpc(int index);
    bool HasNpcAt(int x, int y);
    void KillNpc(int index, int damage);
    void KillNpcsById(int npc_id);
    Npc GetNpc(int index);
    void StartWalk(int index, int x, int y, int direction);
    void ShowDamage(int index, int amount, char heal);
    void ShowHealthBar(int index, int percent, int duration);
    void ShowBar(int index, int bar, int percent, int duration);
    void StartEffect(int index, int effect_id);
    void StartAttack(int index, int direction);
    void AddNpc(int index, int npc_id, int graphic, String name, int x, int y, int direction);
    void Clear();
    unsigned int GetCount();
    void ShowBalloon(int index, String text);
    void SetMousePosition(int cursor_x, int cursor_y);
};

#endif
