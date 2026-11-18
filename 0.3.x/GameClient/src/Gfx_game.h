#ifndef Gfx_gameH
#define Gfx_gameH

#include <TDDBltFX.H>
#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#define BOOK_ROW_HEIGHT 16
#define BUTTON_COLUMN_PITCH 91
#define BUTTON_ROW_PITCH 29
#define INPUT_FORM_ROW_HEIGHT 23
#define ITEM_AMOUNT_ROW_HEIGHT 37
#define ITEM_NAME_ROW_HEIGHT 37
#define ITEM_TYPE_GENDERED 12
#define LIST_HIGHLIGHT_ROW_HEIGHT 16
#define LIST_ROW_HEIGHT 16
#define MENU_HIGHLIGHT_ROW_HEIGHT 37
#define MSGBOARD_MODE_WRITE 2

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
    TDx_DrawSurface *captcha_surface;
    TDx_DrawSurface *visual_doc_surface;
    TDx_DrawSurface *paperdoll_surface;
    TDx_DrawSurface *spare_surface;
    TDx_DrawSurface *book_surface;
    TDDBltFX *black_fill_fx;
    TDDBltFX *view_fill_fx;
    TRect view_clip_rect;
    TRect view_rect;
    TRect message_box_rect;
    TRect text_box_rect;
    TRect input_box_rect;
    TRect player_menu_shadow_rect;
    TRect player_menu_rect;
    TRect hotkeys_rect;
    TRect book_rect;
    TRect list_scrollbar_rect;
    TRect captcha_rect;
    TRect visual_doc_rect;
    TRect amount_box_rect;
    TRect junk_title_rect;
    TRect amount_title_rect;
    TRect trade_title_rect;
    TRect shop_title_rect;
    TRect bank_title_rect;
    TRect cell_highlight_rect;
    TRect tall_row_highlight_rect;
    TRect item_row_highlight_rect;
    TRect short_highlight_rect;
    TRect row_highlight_rect;
    TRect narrow_row_highlight_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    GameGraphics();
    ~GameGraphics();
    TRect GetListBoxBodyRect(int item_count);
    TRect GetListBoxFooterRect(int footer);
    TRect GetIconRect(int icon);
    TPoint GetPaperdollSlotOffset(int item_size);
    void BlitClippedAlpha(int x, int y, TDx_DrawSurface *source, TRect source_rect, int alpha);
    void BlitClippedHalfBlend(int x, int y, TDx_DrawSurface *source, TRect source_rect);
    bool IsInView(int x, int y, TRect *source_rect);
    bool BlitClipped(int x, int y, TDx_DrawSurface *source, TRect source_rect);
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
    void DrawCrafterWindow();
    void DrawShopWindow();
    void DrawLearnWindow();
    void DrawTradeWindow();
    void DrawBookWindow();
    void DrawSubItem(int x, int y, int sub_item_id);
    void DrawPaperdollWindow();
    void DrawHotkeys();
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
    void DrawCrafterButtons();
    void DrawLearnButtons();
    void DrawShopButtons();
    void DrawTradeButtons();
    void DrawBookButtons();
    void DrawPaperdollButtons();
    void DrawItemAmountButtons();
    TRect GetPaperdollFrameRect(int frame);
    TRect GetPopupItemRect(int row);
    TRect GetButtonRect(int button, int state);
    TRect ClipSourceRect(int x, int y, TRect source_rect);
    char LoadSurface(TDx_DrawSurface *surface, int library, int resource_id);
    char LoadButtonSheet(int library, int resource_id);
    char LoadPopupMenu(int library, int resource_id);
    void LoadCaptchaBackground(int library, int resource_id);
    void LoadVisualDocBackground(int library, int resource_id);
    void LoadListBoxBackground(int library, int resource_id);
    void LoadInputBoxBackground(int library, int resource_id);
    void LoadMessageBoxBackground(int library, int resource_id);
    void LoadTextBoxBackground(int library, int resource_id);
    void DrawFormText();
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
    void DrawCrafterText();
    void DrawShopText();
    void DrawTradeText();
    void DrawBookText();
    void DrawPaperdollText();
    void DrawNothing();
    void DrawInfoLine(int window_x, int window_y, int line_y, String text);
    void LoadPaperdollBackground(int library, int resource_id);
    char LoadSpareSurface(int library, int resource_id);
    void LoadSpareSurface2(int library, int resource_id);
    void DrawVisualDocWindow();
    void DrawVisualDocButtons();
    void DrawCaptchaWindow();
    void DrawCaptchaButtons();
    void DrawCaptchaText();
};

#endif
