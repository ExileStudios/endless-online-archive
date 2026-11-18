#include <vcl.h>
#pragma hdrstop

#include "Gfxprotection.h"
#include "Gfx_bank.h"
#include "Gfx_barber.h"
#include "Gfx_chest.h"
#include "Gfx_dialog.h"
#include "Gfx_game.h"
#include "Gfx_interface.h"
#include "Gfx_intro.h"
#include "Gfx_jukebox.h"
#include "Gfx_mapblends.h"
#include "Gfx_mapeffects.h"
#include "Gfx_mapicons.h"
#include "Gfx_mapitems.h"
#include "Gfx_mapmasks.h"
#include "Gfx_mapobjects.h"
#include "Gfx_maptiles.h"
#include "Gfx_maptops.h"
#include "Gfx_mapwalls.h"
#include "Gfx_menu.h"
#include "Gfx_msgboard.h"
#include "Gfx_musician.h"
#include "Gfx_npcs.h"
#include "Gfx_playername.h"
#include "Gfx_shop.h"
#include "Gfx_social.h"
#include "Gfx_sprites.h"
#include "Gfx_stats.h"
#include "Gfx_trade.h"
#include <stdlib.h>

#pragma package(smart_init)

// Makes the intro, sprite and npc graphics and every map graphics list; the screen and window graphics come later.
GfxController::GfxController(char preload_tiles, char preload_map_graphics)
{
    intro = new IntroGraphics();
    sprites = new SpriteGraphics();
    npc_cache = new NpcGraphics();
    objects = new ObjectGraphics(preload_map_graphics);
    tiles = new TileGraphics(preload_tiles);
    walls = new WallGraphics(preload_map_graphics);
    masks = new MaskGraphics(preload_map_graphics);
    tops = new TopGraphics(preload_map_graphics);
    blends = new BlendGraphics(preload_map_graphics);
    items = new ItemGraphics(preload_map_graphics);
    effects = new EffectGraphics(preload_map_graphics);
    icons = new IconGraphics(preload_map_graphics);
    menu_created = false;
    game_created = false;
    hover_label_created = false;
    trade_created = false;
    shop_created = false;
    dialog_created = false;
    bank_created = false;
    jukebox_created = false;
    musician_created = false;
    barber_created = false;
    chest_created = false;
    social_created = false;
    stat_created = false;
    message_board_created = false;
    intro_created = true;
}

GfxController::~GfxController()
{
    FreeMenuGraphics();
    FreeHoverLabel();
    FreeGameGraphics();
    FreeTradeGraphics();
    FreeShopGraphics();
    FreeBankGraphics();
    FreeJukeboxGraphics();
    FreeMusicianGraphics();
    FreeBarberGraphics();
    FreeChestGraphics();
    FreeSocialGraphics();
    if (intro)
        delete intro;
    if (sprites)
        delete sprites;
    if (npc_cache)
        delete npc_cache;
    if (objects)
        delete objects;
    if (walls)
        delete walls;
    if (tiles)
        delete tiles;
    if (masks)
        delete masks;
    if (tops)
        delete tops;
    if (blends)
        delete blends;
    if (items)
        delete items;
    if (effects)
        delete effects;
    if (icons)
        delete icons;
}

void GfxController::FreeIntroGraphics()
{
    if (intro_created)
        delete intro;
    intro_created = false;
}

void GfxController::CreateMenuGraphics()
{
    if (!menu_created)
        menu = new MenuGraphics();
    menu_created = true;
}

void GfxController::FreeMenuGraphics()
{
    if (menu_created)
        delete menu;
    menu_created = false;
}

void GfxController::CreateTradeGraphics()
{
    if (!trade_created)
        trade = new TradeGraphics();
    trade_created = true;
}

void GfxController::CreateShopGraphics()
{
    if (!shop_created)
        shop = new ShopGraphics();
    shop_created = true;
}

void GfxController::CreateDialogGraphics()
{
    if (!dialog_created)
        dialog = new DialogGraphics();
    dialog_created = true;
}

void GfxController::CreateMsgBoardGraphics()
{
    if (!message_board_created)
        message_board = new MsgBoardGraphics();
    message_board_created = true;
}

void GfxController::CreateStatGraphics()
{
    if (!stat_created)
        stat = new StatGraphics();
    stat_created = true;
}

void GfxController::CreateSocialGraphics()
{
    if (!social_created)
        social = new SocialGraphics();
    social_created = true;
}

void GfxController::CreateBankGraphics()
{
    if (!bank_created)
        bank = new BankGraphics();
    bank_created = true;
}

void GfxController::CreateJukeboxGraphics()
{
    if (!jukebox_created)
        jukebox = new JukeboxGraphics();
    jukebox_created = true;
}

void GfxController::CreateMusicianGraphics()
{
    if (!musician_created)
        musician = new MusicianGraphics();
    musician_created = true;
}

void GfxController::CreateBarberGraphics()
{
    if (!barber_created)
        barber = new BarberGraphics();
    barber_created = true;
}

void GfxController::FreeTradeGraphics()
{
    if (trade_created)
        delete trade;
    trade_created = false;
}

void GfxController::FreeDialogGraphics()
{
    if (dialog_created)
        delete dialog;
    dialog_created = false;
}

void GfxController::FreeShopGraphics()
{
    if (shop_created)
        delete shop;
    shop_created = false;
}

void GfxController::FreeMsgBoardGraphics()
{
    if (message_board_created)
        delete message_board;
    message_board_created = false;
}

void GfxController::FreeSocialGraphics()
{
    if (social_created)
        delete social;
    social_created = false;
}

void GfxController::FreeBankGraphics()
{
    if (bank_created)
        delete bank;
    bank_created = false;
}

void GfxController::FreeStatGraphics()
{
    if (stat_created)
        delete stat;
    stat_created = false;
}

void GfxController::FreeJukeboxGraphics()
{
    if (jukebox_created)
        delete jukebox;
    jukebox_created = false;
}

void GfxController::FreeMusicianGraphics()
{
    if (musician_created)
        delete musician;
    musician_created = false;
}

void GfxController::FreeBarberGraphics()
{
    if (barber_created)
        delete barber;
    barber_created = false;
}

void GfxController::CreateChestGraphics()
{
    if (!chest_created)
        chest = new ChestGraphics();
    chest_created = true;
}

void GfxController::FreeChestGraphics()
{
    if (chest_created)
        delete chest;
    chest_created = false;
}

// Replaces the hover label with a new one for text, in the colour that label_style picks.
void GfxController::CreateHoverLabel(int text_width, int height, int label_style, String text)
{
    if (hover_label_created)
    {
        FreeHoverLabel();
        Sleep(1);
    }
    hover_label = new PlayerNameGraphics(text_width, height, label_style, text);
    hover_label_created = true;
}

void GfxController::FreeHoverLabel()
{
    if (hover_label_created)
    {
        hover_label_created = false;
        delete hover_label;
    }
}

// Makes the game screen's interface and game graphics together, once.
void GfxController::CreateGameGraphics()
{
    if (!game_created)
    {
        interface_graphics = new InterfaceGraphics();
        game = new GameGraphics();
    }
    game_created = true;
}

void GfxController::FreeGameGraphics()
{
    if (game_created)
    {
        delete interface_graphics;
        delete game;
    }
    game_created = false;
}

// Loads the trade window background and the item slot frame into the trade graphics.
void GfxController::LoadTradeGraphics()
{
    HMODULE interface_library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (interface_library)
    {
        HMODULE interface_library_copy = interface_library;
        if (trade_created)
            trade->LoadBackground(interface_library_copy, InterfaceBitmap_TradeWindow);
    }
    HMODULE slot_library = LoadLibraryA(GetGfxFileName(SLOT_GFX_FILE).c_str());
    if (slot_library)
    {
        HMODULE slot_library_copy = slot_library;
        if (trade_created)
            trade->LoadItemSlot(slot_library_copy, TileBitmap_ItemSlot);
    }
}

// Loads the shop window background and the item slot frame into the shop graphics.
void GfxController::LoadShopGraphics()
{
    HMODULE interface_library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (interface_library)
    {
        HMODULE interface_library_copy = interface_library;
        if (shop_created)
            shop->LoadBackground(interface_library_copy, InterfaceBitmap_ShopWindow);
    }
    HMODULE slot_library = LoadLibraryA(GetGfxFileName(SLOT_GFX_FILE).c_str());
    if (slot_library)
    {
        HMODULE slot_library_copy = slot_library;
        if (shop_created)
            shop->LoadItemSlot(slot_library_copy, TileBitmap_ItemSlot);
    }
}

// Loads the npc quest dialog window background.
void GfxController::LoadDialogGraphics()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (library)
    {
        HMODULE library_copy = library;
        if (dialog_created)
            dialog->LoadSurface(library_copy, InterfaceBitmap_DialogWindow);
    }
}

// The report window reuses the shop graphics: its own background with the shop item slot frame.
void GfxController::LoadReportGraphics()
{
    HMODULE interface_library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (interface_library)
    {
        HMODULE interface_library_copy = interface_library;
        if (shop_created)
            shop->LoadBackground(interface_library_copy, InterfaceBitmap_ReportWindow);
    }
    HMODULE slot_library = LoadLibraryA(GetGfxFileName(SLOT_GFX_FILE).c_str());
    if (slot_library)
    {
        HMODULE slot_library_copy = slot_library;
        if (shop_created)
            shop->LoadItemSlot(slot_library_copy, TileBitmap_ItemSlot);
    }
}

// Loads the character stats window background.
void GfxController::LoadStatGraphics()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (library)
    {
        HMODULE library_copy = library;
        if (stat_created)
            stat->LoadSurface(library_copy, InterfaceBitmap_StatWindow);
    }
}

// Loads the message board sheet; the quest window loads it too.
void GfxController::LoadMsgBoardGraphics()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (library)
    {
        HMODULE library_copy = library;
        if (message_board_created)
            message_board->LoadSurface(library_copy, InterfaceBitmap_MsgBoardWindow);
    }
}

// Loads the friends and ignore window background.
void GfxController::LoadSocialGraphics()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (library)
    {
        HMODULE library_copy = library;
        if (social_created)
            social->LoadSurface(library_copy, InterfaceBitmap_ShopWindow);
    }
}

// Loads the bank window background.
void GfxController::LoadBankGraphics()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (library)
    {
        HMODULE library_copy = library;
        if (bank_created)
            bank->LoadSurface(library_copy, InterfaceBitmap_BankWindow);
    }
}

// Loads the jukebox window background.
void GfxController::LoadJukeboxGraphics()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (library)
    {
        HMODULE library_copy = library;
        if (jukebox_created)
            jukebox->LoadSurface(library_copy, InterfaceBitmap_JukeboxWindow);
    }
}

// Loads the musician window background and its cell sheet.
void GfxController::LoadMusicianGraphics()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (library && musician_created)
    {
        musician->LoadBackground(library, InterfaceBitmap_MusicianWindow);
        musician->LoadCellSheet(library, InterfaceBitmap_CellSheet);
    }
}

// Loads the barber window background.
void GfxController::LoadBarberGraphics()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (library)
    {
        HMODULE library_copy = library;
        if (barber_created)
            barber->LoadSurface(library_copy, InterfaceBitmap_BarberWindow);
    }
}

// Loads the chest window background and the item slot frame into the chest graphics.
void GfxController::LoadChestGraphics()
{
    HMODULE interface_library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (interface_library)
    {
        HMODULE interface_library_copy = interface_library;
        if (chest_created)
            chest->LoadBackground(interface_library_copy, InterfaceBitmap_ChestWindow);
    }
    HMODULE slot_library = LoadLibraryA(GetGfxFileName(SLOT_GFX_FILE).c_str());
    if (slot_library)
    {
        HMODULE slot_library_copy = slot_library;
        if (chest_created)
            chest->LoadItemSlot(slot_library_copy, TileBitmap_ItemSlot);
    }
}

// Loads the menu screens' panels, buttons and sheets from the menu graphics file, the intro picture while it is
// still kept, and the animation sheet and scroll icons from the interface file.
void GfxController::LoadMenuGraphics(char single_panel)
{
    int menu_library_copy;
    int interface_library_copy;
    HMODULE menu_library = LoadLibraryA(GetGfxFileName(GfxLibrary_MenuInterface).c_str());
    if (menu_library)
    {
        menu_library_copy = (int)menu_library;
        if (intro_created)
            intro->LoadSurface(menu_library_copy, MenuBitmap_Intro);
        // A single-panel layout loads one lower panel of its own instead of both panels and the key sheet.
        if (single_panel)
            menu->LoadLowerPanel(menu_library_copy, MenuBitmap_SingleLowerPanel);
        else
        {
            menu->LoadLowerPanel(menu_library_copy, MenuBitmap_LowerPanel);
            menu->LoadUpperPanel(menu_library_copy, MenuBitmap_UpperPanel);
            menu->LoadKeySheet(menu_library_copy, MenuBitmap_KeySheet);
        }
        menu->LoadSlotFrame(menu_library_copy, MenuBitmap_SlotFrame);
        menu->LoadLabelSheet(menu_library_copy, MenuBitmap_LabelSheet);
        menu->LoadMainMenuButtons(menu_library_copy, MenuBitmap_MainMenuButtons);
        menu->LoadWideButtons(menu_library_copy, MenuBitmap_WideButtons);
        menu->LoadSmallButtons(menu_library_copy, MenuBitmap_SmallButtons);
        menu->LoadMessageBoxBackground(menu_library_copy, MenuBitmap_MessageBox);
        menu->LoadListBoxBackground(menu_library_copy, MenuBitmap_ListBoxBackground);
        menu->LoadProgressBar(menu_library_copy, MenuBitmap_ProgressBar);
        menu->LoadSlotIcons(menu_library_copy, MenuBitmap_SlotIcons);
        menu->LoadCornerButton(menu_library_copy, MenuBitmap_CornerButton);
        FreeLibrary(menu_library);
    }
    HMODULE interface_library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (interface_library)
    {
        interface_library_copy = (int)interface_library;
        menu->LoadAnimationSheet(interface_library_copy, InterfaceBitmap_AnimationSheet);
        menu->LoadScrollIcons(interface_library_copy, InterfaceBitmap_ScrollIcons);
        FreeLibrary(interface_library);
    }
}

// Loads the game screen's interface pictures, from the bars and chat to the icons, cursors and effects, and warns
// that the client needs patching when the last of them is missing.
void GfxController::LoadGameGraphics()
{
    int interface_library_copy;
    int menu_library_copy;
    HMODULE menu_library = LoadLibraryA(GetGfxFileName(GfxLibrary_MenuInterface).c_str());
    if (menu_library)
    {
        menu_library_copy = (int)menu_library;
        game->LoadButtonSheet(menu_library_copy, MenuBitmap_SmallButtons);
        game->LoadMessageBoxBackground(menu_library_copy, MenuBitmap_GameMessageBox);
        game->LoadTextBoxBackground(menu_library_copy, MenuBitmap_TextBoxBackground);
        FreeLibrary(menu_library);
    }
    HMODULE interface_library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (menu_library)
    {
        interface_library_copy = (int)interface_library;
        interface_graphics->LoadHudBackground(interface_library_copy, InterfaceBitmap_HudBackground);
        interface_graphics->LoadClockBackground(interface_library_copy, InterfaceBitmap_ClockBackground);
        interface_graphics->LoadTooltipBackground(interface_library_copy, InterfaceBitmap_TooltipBackground);
        interface_graphics->ClearChatPanel();
        interface_graphics->LoadChatInputBackground(interface_library_copy, InterfaceBitmap_ChatInputBackground);
        interface_graphics->LoadSmallIcons(interface_library_copy, InterfaceBitmap_SmallIcons);
        interface_graphics->LoadChatTabs(interface_library_copy, InterfaceBitmap_ChatTabs);
        interface_graphics->LoadCornerButton(interface_library_copy, InterfaceBitmap_CornerButton);
        interface_graphics->LoadChatModeIcons(interface_library_copy, InterfaceBitmap_ChatModeIcons);
        interface_graphics->LoadTabButtons(interface_library_copy, InterfaceBitmap_TabButtons);
        interface_graphics->LoadListIcons(interface_library_copy, InterfaceBitmap_ListIcons);
        interface_graphics->LoadHudFrame(interface_library_copy, InterfaceBitmap_HudFrame);
        interface_graphics->LoadHudLeftFrame(interface_library_copy, InterfaceBitmap_HudLeftFrame);
        interface_graphics->LoadHudTopFrame(interface_library_copy, InterfaceBitmap_HudTopFrame);
        interface_graphics->LoadTileCursor(interface_library_copy, InterfaceBitmap_TileCursor);
        interface_graphics->LoadControlSheet(interface_library_copy, InterfaceBitmap_ControlSheet);
        interface_graphics->LoadScrollIcons(interface_library_copy, InterfaceBitmap_ScrollIcons);
        interface_graphics->LoadMapLoadingBox(interface_library_copy, InterfaceBitmap_MapLoadingBox);
        interface_graphics->LoadEmoteSheet(interface_library_copy, InterfaceBitmap_EmoteSheet);
        interface_graphics->LoadWeaponEffects(interface_library_copy, InterfaceBitmap_WeaponEffects);
        interface_graphics->LoadGroupIcons(interface_library_copy, InterfaceBitmap_GroupIcons);
        interface_graphics->LoadMinimapTiles(interface_library_copy, InterfaceBitmap_MinimapTiles);
        interface_graphics->LoadStatusIcons(interface_library_copy, InterfaceBitmap_StatusIcons);
        interface_graphics->LoadHudSheet(interface_library_copy, InterfaceBitmap_HudSheet);
        interface_graphics->LoadPanelBackground(interface_library_copy, PanelBackground_Status);
        game->LoadPopupMenu(interface_library_copy, InterfaceBitmap_PopupMenu);
        game->LoadPaperdollBackground(interface_library_copy, InterfaceBitmap_PaperdollBackground);
        game->LoadInputBoxBackground(interface_library_copy, InterfaceBitmap_InputBoxBackground);
        game->LoadListBoxBackground(interface_library_copy, InterfaceBitmap_ListBoxBackground);
        game->LoadBookBackground(interface_library_copy, InterfaceBitmap_BookBackground);
        FreeLibrary(interface_library);
    }
}

// Loads the skills panel background into the shared panel surface.
void GfxController::LoadSkillPanel()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (library)
    {
        int library_copy = (int)library;
        interface_graphics->LoadPanelBackground(library_copy, PanelBackground_Skills);
        FreeLibrary(library);
    }
}

// Loads the spells panel background into the shared panel surface; the skills panel uses the same bitmap.
void GfxController::LoadSpellPanel()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (library)
    {
        int library_copy = (int)library;
        interface_graphics->LoadPanelBackground(library_copy, PanelBackground_Skills);
        FreeLibrary(library);
    }
}

// Loads the help panel background into the shared panel surface.
void GfxController::LoadHelpPanel()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (library)
    {
        int library_copy = (int)library;
        interface_graphics->LoadPanelBackground(library_copy, PanelBackground_Help);
        FreeLibrary(library);
    }
}

// Loads the chat panel background into the shared panel surface.
void GfxController::LoadChatPanel()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (library)
    {
        int library_copy = (int)library;
        interface_graphics->LoadPanelBackground(library_copy, PanelBackground_Chat);
        FreeLibrary(library);
    }
}

// Loads the status panel background into the shared panel surface.
void GfxController::LoadStatusPanel()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (library)
    {
        int library_copy = (int)library;
        interface_graphics->LoadPanelBackground(library_copy, PanelBackground_Status);
        FreeLibrary(library);
    }
}

// Loads the player list panel background into the shared panel surface.
void GfxController::LoadPlayerlistPanel()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (library)
    {
        int library_copy = (int)library;
        interface_graphics->LoadPanelBackground(library_copy, PanelBackground_PlayerList);
        FreeLibrary(library);
    }
}

// Loads the group panel background into the shared panel surface.
void GfxController::LoadGroupPanel()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (library)
    {
        int library_copy = (int)library;
        interface_graphics->LoadPanelBackground(library_copy, PanelBackground_Group);
        FreeLibrary(library);
    }
}

// Loads the inventory panel background into the shared panel surface.
void GfxController::LoadInventoryPanel()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (library)
    {
        int library_copy = (int)library;
        interface_graphics->LoadPanelBackground(library_copy, PanelBackground_Inventory);
        FreeLibrary(library);
    }
}

// Loads the settings panel background into the shared panel surface.
void GfxController::LoadSettingsPanel()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (library)
    {
        int library_copy = (int)library;
        interface_graphics->LoadPanelBackground(library_copy, PanelBackground_Settings);
        FreeLibrary(library);
    }
}

// Loads the news panel background into the shared panel surface.
void GfxController::LoadNewsPanel()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (library)
    {
        int library_copy = (int)library;
        interface_graphics->LoadPanelBackground(library_copy, PanelBackground_News);
        FreeLibrary(library);
    }
}

// Reloads the menu background picked by background_id.
void GfxController::LoadMenuBackground()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(GfxLibrary_MenuInterface).c_str());
    if (library)
    {
        int library_copy = (int)library;
        menu->LoadBackground(library_copy, menu->background_id);
        FreeLibrary(library);
    }
}

// Builds the main menu backdrop: the background with an artwork drawn on it.
// With random_artwork a new main menu artwork is picked at random first.
void GfxController::LoadMainMenuScreen(bool random_artwork)
{
    HMODULE library = LoadLibraryA(GetGfxFileName(GfxLibrary_MenuInterface).c_str());
    if (library)
    {
        int library_copy = (int)library;
        menu->LoadBackground(library_copy, menu->background_id);
        if (random_artwork)
            menu->artwork_id = random(4) + 141;
        menu->LoadArtwork(library_copy, menu->artwork_id);
        menu->DrawArtworkOnBackground(MENU_ARTWORK_X, MENU_ARTWORK_Y);
        FreeLibrary(library);
    }
}

// Builds the create-account backdrop: the background, the form labels and a random artwork.
void GfxController::LoadCreateAccountScreen()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(GfxLibrary_MenuInterface).c_str());
    if (library)
    {
        int library_copy = (int)library;
        menu->LoadBackground(library_copy, menu->background_id);
        menu->DrawFormLabels(0, 0);
        menu->LoadArtwork(library_copy, random(8) + 161);
        menu->DrawArtworkOnBackground(SIDE_ARTWORK_X, SIDE_ARTWORK_Y);
        FreeLibrary(library);
    }
}

// Builds the password entry backdrop, after the account name: the background, the current artwork and the upper panel.
void GfxController::LoadPasswordScreen()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(GfxLibrary_MenuInterface).c_str());
    if (library)
    {
        int library_copy = (int)library;
        menu->LoadBackground(library_copy, menu->background_id);
        menu->LoadArtwork(library_copy, menu->artwork_id);
        menu->DrawArtworkOnBackground(MENU_ARTWORK_X, MENU_ARTWORK_Y);
        menu->DrawUpperPanelOnBackground(266, 184);
        FreeLibrary(library);
    }
}

// Builds the login backdrop: the background, the current artwork and the lower panel.
void GfxController::LoadLoginScreen()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(GfxLibrary_MenuInterface).c_str());
    if (library)
    {
        int library_copy = (int)library;
        menu->LoadBackground(library_copy, menu->background_id);
        menu->LoadArtwork(library_copy, menu->artwork_id);
        menu->DrawArtworkOnBackground(MENU_ARTWORK_X, MENU_ARTWORK_Y);
        menu->DrawLowerPanelOnBackground(266, 284);
        FreeLibrary(library);
    }
}

// Builds the character selection backdrop: the background, three character slot frames and a random artwork.
void GfxController::LoadCharacterSelectScreen()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(GfxLibrary_MenuInterface).c_str());
    if (library)
    {
        int library_copy = (int)library;
        menu->LoadBackground(library_copy, menu->background_id);
        menu->DrawSlotFrameOnBackground(334, 36);
        menu->DrawSlotFrameOnBackground(334, 160);
        menu->DrawSlotFrameOnBackground(334, 284);
        menu->LoadArtwork(library_copy, random(8) + 161);
        menu->DrawArtworkOnBackground(SIDE_ARTWORK_X, SIDE_ARTWORK_Y);
        FreeLibrary(library);
    }
}

// Loads the shared player sheets from the player graphics file: stand, walk, attack, spell, sitting, ranged and face.
void GfxController::LoadSpriteGraphics()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(GfxLibrary_CharacterBodies).c_str());
    if (library)
    {
        int library_copy = (int)library;
        sprites->LoadStandSheet(library_copy, BodyBitmap_StandSheet);
        sprites->LoadWalkSheet(library_copy, BodyBitmap_WalkSheet);
        sprites->LoadAttackSheet(library_copy, BodyBitmap_AttackSheet);
        sprites->LoadSpellCastSheet(library_copy, BodyBitmap_SpellCastSheet);
        sprites->LoadChairSitSheet(library_copy, BodyBitmap_ChairSitSheet);
        sprites->LoadFloorSitSheet(library_copy, BodyBitmap_FloorSitSheet);
        sprites->LoadRangedAttackSheet(library_copy, BodyBitmap_RangedAttackSheet);
        sprites->LoadFaceSheet(library_copy, BodyBitmap_FaceSheet);
        FreeLibrary(library);
    }
}

// Loads the create-character panel background.
void GfxController::LoadCreateCharacterPanel()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(GfxLibrary_MenuInterface).c_str());
    if (library)
    {
        int library_copy = (int)library;
        menu->LoadCreateCharacterBackground(library_copy, MenuBitmap_CreateCharacterPanel);
        FreeLibrary(library);
    }
}

// Loads the change-password panel into the surface the create-character panel uses.
void GfxController::LoadChangePasswordPanel()
{
    HMODULE library = LoadLibraryA(GetGfxFileName(GfxLibrary_MenuInterface).c_str());
    if (library)
    {
        int library_copy = (int)library;
        menu->LoadCreateCharacterBackground(library_copy, MenuBitmap_ChangePasswordPanel);
        FreeLibrary(library);
    }
}

// Builds a graphics file path by number, padded to three digits, from a scrambled path text.
String GfxController::GetGfxFileName(int file_number)
{
    String path = DecodePath(".!G!G.e");
    path.Insert(IntToStr(file_number), 10);
    if (file_number <= 9)
        path.Insert("0", 10);
    if (file_number <= 99)
        path.Insert("0", 10);
    return path;
}

// Unscrambles a path: each marker letter gets the letters that follow it, and each marker sign becomes a backslash.
String GfxController::DecodePath(String encoded_path)
{
    int pos = encoded_path.Length();
    for (; pos > 0; pos--)
    {
        if (encoded_path[pos] == 'G')
            encoded_path.Insert("fx", pos + 1);
        if (encoded_path[pos] == 'e')
            encoded_path.Insert("gf", pos + 1);
        if (encoded_path[pos] == '!')
        {
            encoded_path.Delete(pos, 1);
            encoded_path.Insert("\\", pos);
        }
    }
    return encoded_path;
}
