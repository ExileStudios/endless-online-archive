#ifndef Gfx_gameH
#define Gfx_gameH

#define MSGBOARD_MODE_WRITE 2

#define BUTTON_COLUMN_PITCH 91
#define BUTTON_ROW_PITCH 29
class TDDBltFX;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The game screen's boxes and windows: the message, text, input and amount boxes, the input form, the npc and
// player windows (chest, shop, bank, locker, guild, inn, law, learn, quests, board, trade, book, paperdoll and
// others), the shared button sheet, and the clipped blits that draw into the map view.
struct GameGraphics
{
    TDx_DrawSurface *message_box_surface;
    TDx_DrawSurface *text_box_surface;
    TDx_DrawSurface *input_box_surface;
    TDx_DrawSurface *list_box_surface;
    TDx_DrawSurface *extra_surface;
    TDx_DrawSurface *button_sheet;
    TDx_DrawSurface *paperdoll_surface;
    TDx_DrawSurface *book_surface;
    TDDBltFX *black_fill_fx;
    TDDBltFX *view_fill_fx;
    Windows::TRect view_clip_rect;
    Windows::TRect view_rect;
    Windows::TRect message_box_rect;
    Windows::TRect text_box_rect;
    Windows::TRect input_box_rect;
    Windows::TRect player_menu_shadow_rect;
    Windows::TRect player_menu_rect;
    Windows::TRect book_rect;
    Windows::TRect list_scrollbar_rect;
    Windows::TRect amount_box_rect;
    Windows::TRect junk_title_rect;
    Windows::TRect amount_title_rect;
    Windows::TRect trade_title_rect;
    Windows::TRect shop_title_rect;
    Windows::TRect bank_title_rect;
    Windows::TRect tall_row_highlight_rect;
    Windows::TRect item_row_highlight_rect;
    int short_highlight_left;
    int short_highlight_top;
    int short_highlight_right;
    int short_highlight_bottom;
    Windows::TRect row_highlight_rect;
    Windows::TRect narrow_row_highlight_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    GameGraphics();
    ~GameGraphics();
    static Windows::TRect GetListBoxBodyRect(GameGraphics *self, int item_count);
    static Windows::TRect GetListBoxFooterRect(GameGraphics *self, int footer);
    static Windows::TRect GetIconRect(GameGraphics *self, int icon);
    static Windows::TPoint GetPaperdollSlotOffset(GameGraphics *self, int item_size);
    void BlitClippedAlpha(int x, int y, TDx_DrawSurface *source, Windows::TRect source_rect, int alpha);
    void BlitClippedHalfBlend(int x, int y, TDx_DrawSurface *source, Windows::TRect source_rect);
    bool BlitClipped(int x, int y, TDx_DrawSurface *source, Windows::TRect source_rect);
    void DrawMessageBoxText();
    void DrawTextBoxText();
    void DrawListBox();
    void DrawInputBoxText();
    void DrawListBoxButtons();
    void DrawInputBoxButtons();
    void DrawMessageBoxButtons();
    void DrawTextBoxButtons();
    void DrawItemAmountText();
    void DrawChestWindow();
    void DrawBarberWindow();
    void DrawMusicianWindow();
    void DrawJukeboxWindow();
    void DrawStatWindow();
    void DrawBankWindow();
    void DrawLockerWindow();
    void DrawSocialWindow();
    void DrawGuildWindow();
    void DrawReportWindow();
    void DrawDialogWindow();
    void DrawMemoWindow();
    void DrawInnWindow();
    void DrawLawWindow();
    void DrawQuestWindow();
    void DrawMsgBoardWindow();
    void DrawShopWindow();
    void DrawLearnWindow();
    void DrawTradeWindow();
    void DrawBookWindow();
    void DrawPaperdollWindow();
    void DrawChestButtons();
    void DrawBarberButtons();
    void DrawMusicianButtons();
    void DrawJukeboxButtons();
    void DrawStatButtons();
    void DrawBankButtons();
    void DrawLockerButtons();
    void DrawQuestButtons();
    void DrawMsgBoardButtons();
    void DrawSocialButtons();
    void DrawGuildButtons();
    void DrawReportButtons();
    void DrawDialogButtons();
    void DrawMemoButtons();
    void DrawInnButtons();
    void DrawLawButtons();
    void DrawLearnButtons();
    void DrawShopButtons();
    void DrawTradeButtons();
    void DrawBookButtons();
    void DrawPaperdollButtons();
    void DrawItemAmountButtons();
    static Windows::TRect GetPaperdollFrameRect(GameGraphics *self, int frame);
    static Windows::TRect GetButtonRect(GameGraphics *self, int button, int state);
    static Windows::TRect ClipSourceRect(GameGraphics *self, int x, int y, Windows::TRect source_rect);
    void LoadButtonSheet(int library, int resource_id);
    void LoadPopupMenu(int library, int resource_id);
    void LoadListBoxBackground(int library, int resource_id);
    void LoadInputBoxBackground(int library, int resource_id);
    void LoadMessageBoxBackground(int library, int resource_id);
    void LoadTextBoxBackground(int library, int resource_id);
    void DrawListBoxText();
    void DrawChestText();
    void DrawBarberText();
    void DrawJukeboxText();
    void DrawStatText();
    void DrawBankText();
    void DrawQuestText();
    void DrawMsgBoardText();
    void DrawSocialText();
    void DrawLockerText();
    void DrawGuildText();
    void DrawReportText();
    void DrawDialogText();
    void DrawMemoText();
    void DrawInnText();
    void DrawLawText();
    void DrawLearnText();
    void DrawShopText();
    void DrawTradeText();
    void DrawBookText();
    void DrawPaperdollText();
    void LoadPaperdollBackground(int library, int resource_id);
    void LoadBookBackground(int library, int resource_id);
};

Windows::TRect Gfx_game_GetMenuRowRect(GameGraphics *self, int row);

#endif
