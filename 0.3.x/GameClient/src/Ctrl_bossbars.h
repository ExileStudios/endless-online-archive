#ifndef Ctrl_bossbarsH
#define Ctrl_bossbarsH

struct BossBar;

struct BossBars
{
    BossBar *bars[3];
    int x;
    int y;

    BossBars(int left, int top);
    ~BossBars();
    void HideAll();
    void RenderNames();
    void Update();
    void ApplyHealth(int npc_index, int percent, char was_hit);
    int FindFreeBar();
    int FindBar(int npc_index);
    int ScanNpcs();
    bool IsAnyVisible();
    void Draw();
};

#endif
