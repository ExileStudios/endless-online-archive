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

enum InterfaceBitmap
{
    InterfaceBitmap_HudBackground = 101,
    InterfaceBitmap_ClockBackground = 120,
    InterfaceBitmap_HudFrame = 121,
    InterfaceBitmap_HudLeftFrame = 122,
    InterfaceBitmap_HudTopFrame = 123,
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

enum GfxLibrary
{
    GfxLibrary_MenuInterface = 1,
    GfxLibrary_GameInterface = 2,
    GfxLibrary_MapTiles = 3,
    GfxLibrary_MapObjects = 4,
    GfxLibrary_MapMasks = 5,
    GfxLibrary_MapWalls = 6,
    GfxLibrary_MapTops = 7,
    GfxLibrary_CharacterBodies = 8,
    GfxLibrary_MaleHair = 9,
    GfxLibrary_FemaleHair = 10,
    GfxLibrary_MaleBoots = 11,
    GfxLibrary_FemaleBoots = 12,
    GfxLibrary_MaleArmor = 13,
    GfxLibrary_FemaleArmor = 14,
    GfxLibrary_MaleHats = 15,
    GfxLibrary_FemaleHats = 16,
    GfxLibrary_MaleWeapons = 17,
    GfxLibrary_FemaleWeapons = 18,
    GfxLibrary_MaleShields = 19,
    GfxLibrary_FemaleShields = 20,
    GfxLibrary_Npcs = 21,
    GfxLibrary_Shadows = 22,
    GfxLibrary_MapItems = 23,
    GfxLibrary_Effects = 24,
    GfxLibrary_MapIcons = 25,
    GfxLibrary_VisualDocuments = 26
};

struct BankGraphics;
struct BarberGraphics;
struct BlendGraphics;
struct ChestGraphics;
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
struct NpcGraphics;
struct ObjectGraphics;
struct PlayerNameGraphics;
struct ShopGraphics;
struct SocialGraphics;
struct SpriteGraphics;
struct StatGraphics;
struct TileGraphics;
struct TopGraphics;
struct TradeGraphics;
struct WallGraphics;

// Owns every graphics object: the map lists made at start-up, the screen and window graphics made and freed as they
// open and close, and the loaders that fill them from the graphics files.
struct GfxController
{
    IntroGraphics *intro;
    MenuGraphics *menu;
    InterfaceGraphics *interface_graphics;
    GameGraphics *game;
    SpriteGraphics *sprites;
    NpcGraphics *npcs;
    PlayerNameGraphics *hover_label;
    TradeGraphics *trade;
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

    GfxController(char preload_tiles, char preload_map_graphics);
    ~GfxController();
    void FreeIntroGraphics();
    void CreateMenuGraphics();
    void FreeMenuGraphics();
    void CreateTradeGraphics();
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
    void CreateHoverLabel(int text_width, unsigned int height, int label_style, String text);
    void FreeHoverLabel();
    void CreateGameGraphics();
    void FreeGameGraphics();
    void LoadTradeGraphics();
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
    void LoadMainMenuScreen(bool random_artwork);
    void LoadCreateAccountScreen();
    void LoadPasswordScreen();
    void LoadLoginScreen();
    void LoadCharacterSelectScreen();
    void LoadSpriteGraphics();
    void LoadCreateCharacterPanel();
    void LoadChangePasswordPanel();
    String GetGfxFileName(int file_number);
    String DecodePath(String encoded_path);
};

#endif
