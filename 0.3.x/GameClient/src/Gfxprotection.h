#ifndef GfxprotectionH
#define GfxprotectionH

enum TileBitmap
{
    TileBitmap_ItemSlot = 100
};

enum MenuBitmap
{
    MenuBitmap_Intro = 101,
    MenuBitmap_SingleLowerPanel = 102,
    MenuBitmap_CheckboxSheet = 103,
    MenuBitmap_FormSheet = 104,
    MenuBitmap_SlotFrame = 111,
    MenuBitmap_LabelSheet = 112,
    MenuBitmap_MainMenuButtons = 113,
    MenuBitmap_WideButtons = 114,
    MenuBitmap_SmallButtons = 115,
    MenuBitmap_MessageBox = 118,
    MenuBitmap_ProgressBar = 119,
    MenuBitmap_CreateCharacterPanel = 120,
    MenuBitmap_ChangePasswordPanel = 121,
    MenuBitmap_SlotIcons = 122,
    MenuBitmap_GameMessageBox = 123,
    MenuBitmap_CornerButton = 124,
    MenuBitmap_TextBoxBackground = 125,
    MenuBitmap_ListBoxBackground = 140,
    MenuBitmap_LowerPanel = 200,
    MenuBitmap_UpperPanel = 201,
    MenuBitmap_KeySheet = 202
};

enum InterfaceBitmap
{
    InterfaceBitmap_HudBackground = 101,
    InterfaceBitmap_ClockBackground = 120,
    InterfaceBitmap_HudFrame = 121,
    InterfaceBitmap_SmallButtons = 122,
    InterfaceBitmap_InventoryButtons = 123,
    InterfaceBitmap_TileCursor = 124,
    InterfaceBitmap_TabButtons = 125,
    InterfaceBitmap_TooltipBackground = 126,
    InterfaceBitmap_ControlSheet = 127,
    InterfaceBitmap_ScrollIcons = 129,
    InterfaceBitmap_ChatInputBackground = 130,
    InterfaceBitmap_ChatModeIcons = 131,
    InterfaceBitmap_SmallIcons = 132,
    InterfaceBitmap_AnimationSheet = 133,
    InterfaceBitmap_ChatTabs = 135,
    InterfaceBitmap_MapLoadingBox = 137,
    InterfaceBitmap_EmoteSheet = 138,
    InterfaceBitmap_CornerButton = 139,
    InterfaceBitmap_WeaponEffects = 140,
    InterfaceBitmap_PopupMenu = 141,
    InterfaceBitmap_GroupIcons = 143,
    InterfaceBitmap_MinimapTiles = 145,
    InterfaceBitmap_StatusIcons = 146,
    InterfaceBitmap_PaperdollBackground = 149,
    InterfaceBitmap_TradeWindow = 150,
    InterfaceBitmap_ChestWindow = 151,
    InterfaceBitmap_ShopWindow = 152,
    InterfaceBitmap_BankWindow = 153,
    InterfaceBitmap_InputBoxBackground = 154,
    InterfaceBitmap_HudIcons = 155,
    InterfaceBitmap_BarberWindow = 156,
    InterfaceBitmap_ListBoxBackground = 157,
    InterfaceBitmap_HudSheet = 158,
    InterfaceBitmap_MsgBoardWindow = 159,
    InterfaceBitmap_JukeboxWindow = 160,
    InterfaceBitmap_StatWindow = 161,
    InterfaceBitmap_ReportWindow = 164,
    InterfaceBitmap_MusicianWindow = 165,
    InterfaceBitmap_CellSheet = 166,
    InterfaceBitmap_DialogWindow = 167,
    InterfaceBitmap_ListIcons = 168,
    InterfaceBitmap_BookBackground = 169,
    InterfaceBitmap_ElementIcons = 170,
    InterfaceBitmap_PopupIcons = 171,
    InterfaceBitmap_SpareSurface = 172,
    InterfaceBitmap_ResourceBitmap = 173,
    InterfaceBitmap_CrafterWindow = 174,
    InterfaceBitmap_VisualDocBackground = 176,
    InterfaceBitmap_CaptchaBackground = 177
};

enum BodyBitmap
{
    BodyBitmap_StandSheet = 101,
    BodyBitmap_WalkSheet = 102,
    BodyBitmap_AttackSheet = 103,
    BodyBitmap_SpellCastSheet = 104,
    BodyBitmap_ChairSitSheet = 105,
    BodyBitmap_FloorSitSheet = 106,
    BodyBitmap_RangedAttackSheet = 107,
    BodyBitmap_FaceSheet = 108,
    BodyBitmap_KindSheet = 109
};

#include <vector.h>

struct BankGraphics;
struct BarberGraphics;
struct BlendGraphics;
struct BlendWrapper;
struct ChestGraphics;
struct CrafterGraphics;
struct DialogGraphics;
struct EffectGraphics;
struct GameGraphics;
struct IconGraphics;
struct InterfaceGraphics;
struct IntroGraphics;
struct ItemGraphics;
struct JukeboxGraphics;
struct MaskGraphics;
struct MenuGraphics;
struct MsgBoardGraphics;
struct MusicianGraphics;
struct NpcGlobalGraphics;
struct NpcGraphics;
struct NpcWrapper;
struct ObjectGraphics;
struct ObjectWrapper;
struct PlayerNameGraphics;
struct ShopGraphics;
struct SocialGraphics;
struct SpriteGlobalGraphics;
struct StatGraphics;
struct TileGraphics;
struct TopGraphics;
struct TradeGraphics;
struct WallGraphics;
struct WallWrapper;

#include "Gfx_useritem.h"

#define INTERFACE_GFX_FILE 2
#define SLOT_GFX_FILE 3

// Owns every graphics object: the map lists made at start-up, the screen and window graphics made and freed as they
// open and close, and the loaders that fill them from the graphics files.
struct GfxController
{
    IntroGraphics *intro;
    MenuGraphics *menu;
    InterfaceGraphics *interface_graphics;
    GameGraphics *game;
    PlayerNameGraphics *hover_label;
    TradeGraphics *trade;
    CrafterGraphics *crafter;
    ShopGraphics *shop;
    DialogGraphics *dialog;
    BankGraphics *bank;
    JukeboxGraphics *jukebox;
    MusicianGraphics *musician;
    BarberGraphics *barber;
    ChestGraphics *chest;
    SocialGraphics *social;
    StatGraphics *stat;
    MsgBoardGraphics *message_board;
    SpriteGlobalGraphics *sprites;
    NpcGlobalGraphics *npcs;
    NpcGraphics *npc_cache;
    TileGraphics *tiles;
    ObjectGraphics *objects;
    WallGraphics *walls;
    MaskGraphics *masks;
    TopGraphics *tops;
    BlendGraphics *blends;
    ItemGraphics *items;
    EffectGraphics *effects;
    IconGraphics *icons;
    bool menu_created;
    bool game_created;
    bool hover_label_created;
    bool intro_created;
    bool trade_created;
    bool crafter_created;
    bool shop_created;
    bool dialog_created;
    bool bank_created;
    bool jukebox_created;
    bool musician_created;
    bool barber_created;
    bool chest_created;
    bool social_created;
    bool stat_created;
    bool message_board_created;
    int side_artwork_id;

    GfxController(char preload_tiles, char preload_map_graphics);
    ~GfxController();
    void FreeIntroGraphics();
    void CreateMenuGraphics();
    void FreeMenuGraphics();
    void CreateTradeGraphics();
    void CreateCrafterGraphics();
    void CreateShopGraphics();
    void CreateDialogGraphics();
    void CreateMsgBoardGraphics();
    void CreateStatGraphics();
    void CreateSocialGraphics();
    void CreateBankGraphics();
    void CreateJukeboxGraphics();
    void CreateMusicianGraphics();
    void CreateBarberGraphics();
    void FreeTradeGraphics();
    void FreeDialogGraphics();
    void FreeCrafterGraphics();
    void FreeShopGraphics();
    void FreeMsgBoardGraphics();
    void FreeSocialGraphics();
    void FreeBankGraphics();
    void FreeStatGraphics();
    void FreeJukeboxGraphics();
    void FreeMusicianGraphics();
    void FreeBarberGraphics();
    void CreateChestGraphics();
    void FreeChestGraphics();
    void CreateHoverLabel(int text_width, int height, int label_style, int element, String text);
    void FreeHoverLabel();
    void CreateGameGraphics();
    void FreeGameGraphics();
    void LoadTradeGraphics();
    void LoadCrafterGraphics();
    void LoadShopGraphics();
    void LoadDialogGraphics();
    void LoadReportGraphics();
    void LoadStatGraphics();
    void LoadMsgBoardGraphics();
    void LoadSocialGraphics();
    void LoadBankGraphics();
    void LoadJukeboxGraphics();
    void LoadMusicianGraphics();
    void LoadBarberGraphics();
    void LoadChestGraphics();
    void LoadMenuGraphics(char single_panel);
    void LoadGameGraphics();
    String GetGfxFileName(int file_number);
    String DecodePath(String encoded_path);
    void LoadSkillPanel();
    void LoadSpellPanel();
    void LoadHelpPanel();
    void LoadChatPanel();
    void LoadStatusPanel();
    void LoadPlayerlistPanel();
    void LoadGroupPanel();
    void LoadInventoryPanel();
    void LoadSettingsPanel();
    void LoadNewsPanel();
    void LoadMenuBackground();
    void NextMenuBackground();
    void LoadMainMenuScreen(bool random_artwork);
    void LoadCreateAccountScreen();
    void LoadSignupCodeScreen(int style, bool random_artwork);
    void LoadPasswordResetScreen(int step, bool random_artwork);
    void LoadPasswordScreen();
    void LoadLoginScreen();
    void LoadCharacterSelectScreen();
    void LoadSpriteGraphics();
    void LoadCreateCharacterPanel();
    void LoadChangePasswordPanel();
};

#endif
