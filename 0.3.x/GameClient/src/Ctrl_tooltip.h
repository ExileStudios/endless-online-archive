#ifndef Ctrl_tooltipH
#define Ctrl_tooltipH

enum TooltipAlign
{
    TooltipAlign_Below = 0,
    TooltipAlign_Above = 1,
    TooltipAlign_Centered = 2
};

struct Tooltip
{
    int element;
    int title_width;
    String lines[9];
    unsigned char line_colors[12];
    int line_count;
    int scroll_offset;
    int fixed_lines;
    int width;
    int x;
    int y;
    int shown_id;
    int amount;
    unsigned char shown_kind;
    int item_type;
    int transform_skin;
    int anchor_x;
    int anchor_y;
    int flip_edge_x;
    int flip_gap;
    int flip_extra;
    int bottom_limit;
    int bottom_margin;
    int bottom_extra;
    int raise;
    int vertical_align;
    unsigned char pad_y;
    unsigned char pad_x;
    int default_color;
    int colors[1];
    int color_1;
    int color_2;
    int color_3;
    int color_4;
    int color_5;
    int color_6;
    int color_7;
    int color_8;
    int color_9;
    char show_cursed;

    Tooltip(int left, int top, int fixed);
    ~Tooltip();
    bool Hide();
    void UpdateItemAmount(int item_id, int new_amount);
    void ClearLines();
    void ShowItem(int item_id);
    void ShowItemCount(int item_id, int count);
    void ShowSpell(int spell_id);
    void SetFlipExtra(int extra);
    void SetAnchorX(int value);
    void SetAnchorY(int value);
    void SetRaise(int value);
    void SetFlipEdge(int edge_x, int gap);
    void SetBottomLimit(int limit, int margin);
    void SetVerticalAlign(int align);
    void SetShowCursed(char show);
    void DrawFrame();
    void DrawString();
    void Measure();
    bool ScrollDown();
    bool ScrollUp();
    bool IsVisible();
};

#endif
