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
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)
#define INTERFACE_GFX_FILE 2
#define MENU_ARTWORK_X 229
#define MENU_ARTWORK_Y 70
#define SIDE_ARTWORK_X 42
#define SIDE_ARTWORK_Y 140
#define SLOT_GFX_FILE 3

// Makes the intro, sprite and npc graphics and every map graphics list; the screen and window graphics come later.
GfxController::GfxController(char preload_tiles, char preload_map_graphics)
{
    intro = new IntroGraphics();
    sprites = new SpriteGraphics();
    npcs = new NpcGraphics();
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
    if (npcs)
        delete npcs;
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
void GfxController::CreateHoverLabel(int text_width, unsigned int height, int label_style, String text)
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
    HMODULE h;
    HMODULE h2;
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        h = LibraryA;
        if (trade_created)
            GraphicSize background_size = trade->LoadBackground(h, 150);
    }
    HMODULE slot_library = LoadLibraryA(GetGfxFileName(SLOT_GFX_FILE).c_str());
    if (slot_library)
    {
        h2 = slot_library;
        if (trade_created)
            trade->LoadItemSlot(h2, 100);
    }
}

// Loads the shop window background and the item slot frame into the shop graphics.
void GfxController::LoadShopGraphics()
{
    HMODULE h;
    HMODULE h2;
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        h = LibraryA;
        if (shop_created)
            GraphicSize background_size = shop->LoadBackground(h, 152);
    }
    HMODULE slot_library = LoadLibraryA(GetGfxFileName(SLOT_GFX_FILE).c_str());
    if (slot_library)
    {
        h2 = slot_library;
        if (shop_created)
            shop->LoadItemSlot(h2, 100);
    }
}

// Loads the npc quest dialog window background.
void GfxController::LoadDialogGraphics()
{
    HMODULE h;
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        h = LibraryA;
        if (dialog_created)
            dialog->LoadSurface(h, 167);
    }
}

// The report window reuses the shop graphics: its own background with the shop item slot frame.
void GfxController::LoadReportGraphics()
{
    HMODULE h;
    HMODULE h2;
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        h = LibraryA;
        if (shop_created)
            GraphicSize background_size = shop->LoadBackground(h, 164);
    }
    HMODULE slot_library = LoadLibraryA(GetGfxFileName(SLOT_GFX_FILE).c_str());
    if (slot_library)
    {
        h2 = slot_library;
        if (shop_created)
            shop->LoadItemSlot(h2, 100);
    }
}

// Loads the character stats window background.
void GfxController::LoadStatGraphics()
{
    HMODULE h;
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        h = LibraryA;
        if (stat_created)
            stat->LoadSurface(h, 161);
    }
}

// Loads the message board sheet; the quest window loads it too.
void GfxController::LoadMsgBoardGraphics()
{
    HMODULE h;
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        h = LibraryA;
        if (message_board_created)
            message_board->LoadSurface(h, 159);
    }
}

// Loads the friends and ignore window background.
void GfxController::LoadSocialGraphics()
{
    HMODULE h;
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        h = LibraryA;
        if (social_created)
            social->LoadSurface(h, 152);
    }
}

// Loads the bank window background.
void GfxController::LoadBankGraphics()
{
    HMODULE h;
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        h = LibraryA;
        if (bank_created)
            bank->LoadSurface(h, 153);
    }
}

// Loads the jukebox window background.
void GfxController::LoadJukeboxGraphics()
{
    HMODULE h;
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        h = LibraryA;
        if (jukebox_created)
            jukebox->LoadSurface(h, 160);
    }
}

// Loads the musician window background and its cell sheet.
void GfxController::LoadMusicianGraphics()
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        if (musician_created)
        {
            GraphicSize background_size = musician->LoadBackground(LibraryA, 165);
            GraphicSize cell_sheet_size = musician->LoadCellSheet(LibraryA, 166);
        }
    }
}

// Loads the barber window background.
void GfxController::LoadBarberGraphics()
{
    HMODULE h;
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        h = LibraryA;
        if (barber_created)
            barber->LoadSurface(h, 156);
    }
}

// Loads the chest window background and the item slot frame into the chest graphics.
void GfxController::LoadChestGraphics()
{
    HMODULE h;
    HMODULE h2;
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        h = LibraryA;
        if (chest_created)
            GraphicSize background_size = chest->LoadBackground(h, 151);
    }
    HMODULE slot_library = LoadLibraryA(GetGfxFileName(SLOT_GFX_FILE).c_str());
    if (slot_library)
    {
        h2 = slot_library;
        if (chest_created)
            chest->LoadItemSlot(h2, 100);
    }
}

// Loads the menu screens' panels, buttons and sheets from the menu graphics file, the intro picture while it is
// still kept, and the animation sheet and scroll icons from the interface file.
void GfxController::LoadMenuGraphics(char single_panel)
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(1).c_str());
    if (LibraryA)
    {
        int h = (int)LibraryA;
        if (intro_created)
            intro->LoadSurface(h, 101);
        // A single-panel layout loads one lower panel of its own instead of both panels and the key sheet.
        if (single_panel)
            menu->LoadLowerPanel(h, 102);
        else
        {
            menu->LoadLowerPanel(h, 200);
            menu->LoadUpperPanel(h, 201);
            menu->LoadKeySheet(h, 202);
        }
        menu->LoadSlotFrame(h, 111);
        menu->LoadLabelSheet(h, 112);
        menu->LoadMainMenuButtons(h, 113);
        menu->LoadWideButtons(h, 114);
        menu->LoadSmallButtons(h, 115);
        menu->LoadMessageBoxBackground(h, 118);
        menu->LoadListBoxBackground(h, 140);
        menu->LoadProgressBar(h, 119);
        menu->LoadSlotIcons(h, 122);
        menu->LoadCornerButton(h, 124);
        FreeLibrary(LibraryA);
    }
    HMODULE interface_library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (interface_library)
    {
        int h2 = (int)interface_library;
        menu->LoadAnimationSheet(h2, 133);
        menu->LoadScrollIcons(h2, 129);
        FreeLibrary(interface_library);
    }
}

// Loads the game screen: buttons and boxes from the menu graphics file, then every interface sheet and background
// from the interface file.
void GfxController::LoadGameGraphics()
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(1).c_str());
    if (LibraryA)
    {
        int h = (int)LibraryA;
        game->LoadButtonSheet(h, 115);
        game->LoadMessageBoxBackground(h, 123);
        game->LoadTextBoxBackground(h, 125);
        FreeLibrary(LibraryA);
    }
    HMODULE interface_library = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        int h2 = (int)interface_library;
        interface_graphics->LoadHudBackground(h2, 101);
        interface_graphics->LoadClockBackground(h2, 120);
        interface_graphics->LoadTooltipBackground(h2, 126);
        interface_graphics->LoadChatInputBackground(h2, 130);
        interface_graphics->LoadSmallIcons(h2, 132);
        interface_graphics->LoadChatTabs(h2, 135);
        interface_graphics->LoadCornerButton(h2, 139);
        interface_graphics->LoadChatModeIcons(h2, 131);
        interface_graphics->LoadTabButtons(h2, 125);
        interface_graphics->LoadListIcons(h2, 168);
        interface_graphics->LoadHudFrame(h2, 121);
        interface_graphics->LoadHudLeftFrame(h2, 122);
        interface_graphics->LoadHudTopFrame(h2, 123);
        interface_graphics->LoadTileCursor(h2, 124);
        interface_graphics->LoadControlSheet(h2, 127);
        interface_graphics->LoadScrollIcons(h2, 129);
        interface_graphics->LoadMapLoadingBox(h2, 137);
        interface_graphics->LoadEmoteSheet(h2, 138);
        interface_graphics->LoadWeaponEffects(h2, 140);
        interface_graphics->LoadGroupIcons(h2, 143);
        interface_graphics->LoadMinimapTiles(h2, 145);
        interface_graphics->LoadStatusIcons(h2, 146);
        interface_graphics->LoadHudSheet(h2, 158);
        interface_graphics->LoadPanelBackground(h2, PanelBackground_Status);
        game->LoadPopupMenu(h2, 141);
        game->LoadPaperdollBackground(h2, 149);
        game->LoadInputBoxBackground(h2, 154);
        game->LoadListBoxBackground(h2, 157);
        game->LoadBookBackground(h2, 169);
        FreeLibrary(interface_library);
    }
}

// Loads the skills panel background into the shared panel surface.
void GfxController::LoadSkillPanel()
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        int LibraryA_int = (int)LibraryA;
        interface_graphics->LoadPanelBackground(LibraryA_int, PanelBackground_Skills);
        FreeLibrary(LibraryA);
    }
}

// Loads the spells panel background into the shared panel surface; the skills panel uses the same bitmap.
void GfxController::LoadSpellPanel()
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        int LibraryA_int = (int)LibraryA;
        interface_graphics->LoadPanelBackground(LibraryA_int, PanelBackground_Skills);
        FreeLibrary(LibraryA);
    }
}

// Loads the help panel background into the shared panel surface.
void GfxController::LoadHelpPanel()
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        int LibraryA_int = (int)LibraryA;
        interface_graphics->LoadPanelBackground(LibraryA_int, PanelBackground_Help);
        FreeLibrary(LibraryA);
    }
}

// Loads the chat panel background into the shared panel surface.
void GfxController::LoadChatPanel()
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        int LibraryA_int = (int)LibraryA;
        interface_graphics->LoadPanelBackground(LibraryA_int, PanelBackground_Chat);
        FreeLibrary(LibraryA);
    }
}

// Loads the status panel background into the shared panel surface.
void GfxController::LoadStatusPanel()
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        int LibraryA_int = (int)LibraryA;
        interface_graphics->LoadPanelBackground(LibraryA_int, PanelBackground_Status);
        FreeLibrary(LibraryA);
    }
}

// Loads the player list panel background into the shared panel surface.
void GfxController::LoadPlayerlistPanel()
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        int LibraryA_int = (int)LibraryA;
        interface_graphics->LoadPanelBackground(LibraryA_int, PanelBackground_PlayerList);
        FreeLibrary(LibraryA);
    }
}

// Loads the group panel background into the shared panel surface.
void GfxController::LoadGroupPanel()
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        int LibraryA_int = (int)LibraryA;
        interface_graphics->LoadPanelBackground(LibraryA_int, PanelBackground_Group);
        FreeLibrary(LibraryA);
    }
}

// Loads the inventory panel background into the shared panel surface.
void GfxController::LoadInventoryPanel()
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        int LibraryA_int = (int)LibraryA;
        interface_graphics->LoadPanelBackground(LibraryA_int, PanelBackground_Inventory);
        FreeLibrary(LibraryA);
    }
}

// Loads the settings panel background into the shared panel surface.
void GfxController::LoadSettingsPanel()
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        int LibraryA_int = (int)LibraryA;
        interface_graphics->LoadPanelBackground(LibraryA_int, PanelBackground_Settings);
        FreeLibrary(LibraryA);
    }
}

// Loads the news panel background into the shared panel surface.
void GfxController::LoadNewsPanel()
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(INTERFACE_GFX_FILE).c_str());
    if (LibraryA)
    {
        int LibraryA_int = (int)LibraryA;
        interface_graphics->LoadPanelBackground(LibraryA_int, PanelBackground_News);
        FreeLibrary(LibraryA);
    }
}

// Reloads the menu background picked by background_id.
void GfxController::LoadMenuBackground()
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(1).c_str());
    if (LibraryA)
    {
        int LibraryA_int = (int)LibraryA;
        menu->LoadBackground(LibraryA_int, menu->background_id);
        FreeLibrary(LibraryA);
    }
}

// Builds the main menu backdrop: the background with an artwork drawn on it.
// With random_artwork a new main menu artwork is picked at random first.
void GfxController::LoadMainMenuScreen(bool random_artwork)
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(1).c_str());
    if (LibraryA)
    {
        int LibraryA_int = (int)LibraryA;
        this->menu->LoadBackground(LibraryA_int, this->menu->background_id);
        if (random_artwork)
            menu->artwork_id = random(4) + 141;
        this->menu->LoadArtwork(LibraryA_int, this->menu->artwork_id);
        menu->DrawArtworkOnBackground(MENU_ARTWORK_X, MENU_ARTWORK_Y);
        FreeLibrary(LibraryA);
    }
}

// Builds the create-account backdrop: the background, the form labels and a random artwork.
void GfxController::LoadCreateAccountScreen()
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(1).c_str());
    if (LibraryA)
    {
        int LibraryA_int = (int)LibraryA;
        this->menu->LoadBackground(LibraryA_int, this->menu->background_id);
        menu->DrawFormLabels(0, 0);
        this->menu->LoadArtwork(LibraryA_int, random(8) + 161);
        menu->DrawArtworkOnBackground(SIDE_ARTWORK_X, SIDE_ARTWORK_Y);
        FreeLibrary(LibraryA);
    }
}

// Builds the password entry backdrop, after the account name: the background, the current artwork and the upper panel.
void GfxController::LoadPasswordScreen()
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(1).c_str());
    if (LibraryA)
    {
        int LibraryA_int = (int)LibraryA;
        this->menu->LoadBackground(LibraryA_int, this->menu->background_id);
        this->menu->LoadArtwork(LibraryA_int, this->menu->artwork_id);
        menu->DrawArtworkOnBackground(MENU_ARTWORK_X, MENU_ARTWORK_Y);
        menu->DrawUpperPanelOnBackground(266, 184);
        FreeLibrary(LibraryA);
    }
}

// Builds the login backdrop: the background, the current artwork and the lower panel.
void GfxController::LoadLoginScreen()
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(1).c_str());
    if (LibraryA)
    {
        int LibraryA_int = (int)LibraryA;
        this->menu->LoadBackground(LibraryA_int, this->menu->background_id);
        this->menu->LoadArtwork(LibraryA_int, this->menu->artwork_id);
        menu->DrawArtworkOnBackground(MENU_ARTWORK_X, MENU_ARTWORK_Y);
        menu->DrawLowerPanelOnBackground(266, 284);
        FreeLibrary(LibraryA);
    }
}

// Builds the character selection backdrop: the background, three character slot frames and a random artwork.
void GfxController::LoadCharacterSelectScreen()
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(1).c_str());
    if (LibraryA)
    {
        int LibraryA_int = (int)LibraryA;
        this->menu->LoadBackground(LibraryA_int, this->menu->background_id);
        menu->DrawSlotFrameOnBackground(334, 36);
        menu->DrawSlotFrameOnBackground(334, 160);
        menu->DrawSlotFrameOnBackground(334, 284);
        this->menu->LoadArtwork(LibraryA_int, random(8) + 161);
        menu->DrawArtworkOnBackground(SIDE_ARTWORK_X, SIDE_ARTWORK_Y);
        FreeLibrary(LibraryA);
    }
}

// Loads the shared player sheets from the player graphics file: stand, walk, attack, spell, sitting, ranged and face.
void GfxController::LoadSpriteGraphics()
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(8).c_str());
    if (LibraryA)
    {
        int h = (int)LibraryA;
        sprites->LoadStandSheet(h, 101);
        sprites->LoadWalkSheet(h, 102);
        sprites->LoadAttackSheet(h, 103);
        sprites->LoadSpellCastSheet(h, 104);
        sprites->LoadChairSitSheet(h, 105);
        sprites->LoadFloorSitSheet(h, 106);
        sprites->LoadRangedAttackSheet(h, 107);
        sprites->LoadFaceSheet(h, 108);
        FreeLibrary(LibraryA);
    }
}

// Loads the create-character panel background.
void GfxController::LoadCreateCharacterPanel()
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(1).c_str());
    if (LibraryA)
    {
        int LibraryA_int = (int)LibraryA;
        menu->LoadCreateCharacterBackground(LibraryA_int, 120);
        FreeLibrary(LibraryA);
    }
}

// Loads the change-password panel into the surface the create-character panel uses.
void GfxController::LoadChangePasswordPanel()
{
    HMODULE LibraryA = LoadLibraryA(GetGfxFileName(1).c_str());
    if (LibraryA)
    {
        int LibraryA_int = (int)LibraryA;
        menu->LoadCreateCharacterBackground(LibraryA_int, 121);
        FreeLibrary(LibraryA);
    }
}

// Builds a graphics file path by number, padded to three digits, from a scrambled path text.
String GfxController::GetGfxFileName(int file_number)
{
    String path = DecodePath(this, ".!G!G.e");
    path.Insert(IntToStr(file_number), 10);
    if (file_number <= 9)
        path.Insert("0", 10);
    if (file_number <= 99)
        path.Insert("0", 10);
    return path;
}

// Unscrambles a path: each marker letter gets the letters that follow it, and each marker sign becomes a backslash.
String GfxController::DecodePath(GfxController *self, String encoded_path)
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
