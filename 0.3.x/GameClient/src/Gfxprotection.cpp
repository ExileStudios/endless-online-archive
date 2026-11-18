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
#include "Gfx_playername.h"
#include "Gfx_shop.h"
#include "Gfx_social.h"
#include "Gfx_stats.h"
#include "Gfx_trade.h"
#include <stdlib.h>
#include "Tmainform.h"
#include "Ctrl_menu.h"
#include "Gfx_global_npcs.h"
#include "Gfx_global_sprites.h"
#include "Gfx_mapnpcs.h"
#include "Gfx_crafter.h"

#pragma package(smart_init)

// Makes the intro, sprite and npc graphics and every map graphics list; the screen and window graphics come later.
GfxController::GfxController(char preload_tiles, char preload_map_graphics)
{
    intro = new IntroGraphics();
    sprites = new SpriteGlobalGraphics();
    npcs = new NpcGlobalGraphics();
    npc_cache = new NpcGraphics(preload_map_graphics);
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
    crafter_created = false;
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
    FreeCrafterGraphics();
    FreeShopGraphics();
    FreeBankGraphics();
    FreeJukeboxGraphics();
    FreeMusicianGraphics();
    FreeBarberGraphics();
    FreeChestGraphics();
    FreeSocialGraphics();
    if (sprites)
        delete sprites;
    if (npcs)
        delete npcs;
    if (intro)
        delete intro;
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

void GfxController::CreateCrafterGraphics()
{
    if (!crafter_created)
        crafter = new CrafterGraphics();
    crafter_created = true;
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

void GfxController::FreeCrafterGraphics()
{
    if (crafter_created)
        delete crafter;
    crafter_created = false;
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
void GfxController::CreateHoverLabel(int text_width, int height, int label_style, int element, String text)
{
    if (hover_label_created)
        FreeHoverLabel();
    hover_label = new PlayerNameGraphics(text_width, height, label_style, element, text);
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
    int slot_library;
    int interface_library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (interface_library && trade_created)
    {
        TPoint size = trade->LoadBackground(interface_library, InterfaceBitmap_TradeWindow);
    }
    slot_library = MainForm->gfx_vault->OpenLibrary(SLOT_GFX_FILE);
    if (slot_library && trade_created)
    {
        TPoint slot_size = trade->LoadItemSlot(slot_library, TileBitmap_ItemSlot);
    }
}

// Loads the crafter window's background and its parts.
void GfxController::LoadCrafterGraphics()
{
    int parts_library;
    int crafter_library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_GameInterface);
    if (crafter_library && crafter_created)
    {
        TPoint size = crafter->LoadBackground(crafter_library, InterfaceBitmap_CrafterWindow);
    }
    parts_library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MapTiles);
    if (parts_library && crafter_created)
    {
        TPoint parts_size = crafter->LoadParts(parts_library, TileBitmap_ItemSlot);
    }
}

// Loads the shop window background and the item slot frame into the shop graphics.
void GfxController::LoadShopGraphics()
{
    int slot_library;
    int interface_library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (interface_library && shop_created)
    {
        TPoint size = shop->LoadBackground(interface_library, InterfaceBitmap_ShopWindow);
    }
    slot_library = MainForm->gfx_vault->OpenLibrary(SLOT_GFX_FILE);
    if (slot_library && shop_created)
    {
        TPoint slot_size = shop->LoadItemSlot(slot_library, TileBitmap_ItemSlot);
    }
}

// Loads the npc quest dialog window background.
void GfxController::LoadDialogGraphics()
{
    int library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (library && dialog_created)
    {
        TPoint size = dialog->LoadSurface(library, InterfaceBitmap_DialogWindow);
    }
}

// The report window reuses the shop graphics: its own background with the shop item slot frame.
void GfxController::LoadReportGraphics()
{
    int slot_library;
    int interface_library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (interface_library && shop_created)
    {
        TPoint size = shop->LoadBackground(interface_library, InterfaceBitmap_ReportWindow);
    }
    slot_library = MainForm->gfx_vault->OpenLibrary(SLOT_GFX_FILE);
    if (slot_library && shop_created)
    {
        TPoint slot_size = shop->LoadItemSlot(slot_library, TileBitmap_ItemSlot);
    }
}

// Loads the character stats window background.
void GfxController::LoadStatGraphics()
{
    int library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (library && stat_created)
    {
        TPoint size = stat->LoadSurface(library, InterfaceBitmap_StatWindow);
    }
}

// Loads the message board sheet.
void GfxController::LoadMsgBoardGraphics()
{
    int library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (library && message_board_created)
    {
        TPoint size = message_board->LoadSurface(library, InterfaceBitmap_MsgBoardWindow);
    }
}

// Loads the friends and ignore window background.
void GfxController::LoadSocialGraphics()
{
    int library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (library && social_created)
    {
        TPoint size = social->LoadSurface(library, InterfaceBitmap_ShopWindow);
    }
}

// Loads the bank window background.
void GfxController::LoadBankGraphics()
{
    int library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (library && bank_created)
    {
        TPoint size = bank->LoadSurface(library, InterfaceBitmap_BankWindow);
    }
}

// Loads the jukebox window background.
void GfxController::LoadJukeboxGraphics()
{
    int library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (library && jukebox_created)
    {
        TPoint size = jukebox->LoadSurface(library, InterfaceBitmap_JukeboxWindow);
    }
}

// Loads the musician window background and its cell sheet.
void GfxController::LoadMusicianGraphics()
{
    int library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (library)
    {
        int library_copy = library;
        if (musician_created)
        {
            TPoint size = musician->LoadBackground(library_copy, InterfaceBitmap_MusicianWindow);
            TPoint cell_size = musician->LoadCellSheet(library_copy, InterfaceBitmap_CellSheet);
        }
    }
}

// Loads the barber window background.
void GfxController::LoadBarberGraphics()
{
    int library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (library && barber_created)
    {
        TPoint size = barber->LoadSurface(library, InterfaceBitmap_BarberWindow);
    }
}

// Loads the chest window background and the item slot frame into the chest graphics.
void GfxController::LoadChestGraphics()
{
    int slot_library;
    int interface_library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (interface_library && chest_created)
    {
        TPoint size = chest->LoadBackground(interface_library, InterfaceBitmap_ChestWindow);
    }
    slot_library = MainForm->gfx_vault->OpenLibrary(SLOT_GFX_FILE);
    if (slot_library && chest_created)
    {
        TPoint slot_size = chest->LoadItemSlot(slot_library, TileBitmap_ItemSlot);
    }
}

// Loads the menu screens' pictures from the menu and interface files: the panels, form and label sheets, buttons,
// boxes, icons and the animation sheet.
void GfxController::LoadMenuGraphics(char single_panel)
{
    int interface_library_copy;
    int menu_library_copy;
    int menu_library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MenuInterface);
    if (menu_library)
    {
        menu_library_copy = menu_library;
        if (intro_created)
            intro->LoadSurface(menu_library_copy, MenuBitmap_Intro);
        // A single-panel layout loads one lower panel of its own instead of both panels and the key sheet.
        if (single_panel)
        {
            menu->LoadLowerPanel(menu_library_copy, MenuBitmap_SingleLowerPanel);
            menu->LoadCheckboxSheet(menu_library_copy, MenuBitmap_CheckboxSheet);
            menu->LoadFormSheet(menu_library_copy, MenuBitmap_FormSheet);
        }
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
    }
    int interface_library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (interface_library)
    {
        interface_library_copy = interface_library;
        menu->LoadAnimationSheet(interface_library_copy, InterfaceBitmap_AnimationSheet);
        menu->LoadScrollIcons(interface_library_copy, InterfaceBitmap_ScrollIcons);
    }
}

// Loads the game screen's interface pictures, from the bars and chat to the icons, cursors and effects, and warns
// that the client needs patching when the last of them is missing.
void GfxController::LoadGameGraphics()
{
    int interface_library_copy;
    int interface_library;
    int menu_library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MenuInterface);
    if (menu_library)
    {
        int menu_library_copy = menu_library;
        game->LoadButtonSheet(menu_library_copy, MenuBitmap_SmallButtons);
        game->LoadMessageBoxBackground(menu_library_copy, MenuBitmap_GameMessageBox);
        game->LoadTextBoxBackground(menu_library_copy, MenuBitmap_TextBoxBackground);
    }
    interface_library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (menu_library)
    {
        interface_library_copy = interface_library;
        interface_graphics->LoadHudBackground(interface_library_copy, InterfaceBitmap_HudBackground);
        interface_graphics->LoadClockBackground(interface_library_copy, InterfaceBitmap_ClockBackground);
        interface_graphics->LoadTooltipBackground(interface_library_copy, InterfaceBitmap_TooltipBackground);
        interface_graphics->ClearChatPanel();
        interface_graphics->LoadChatInputBackground(interface_library_copy, InterfaceBitmap_ChatInputBackground);
        interface_graphics->LoadSmallIcons(interface_library_copy, InterfaceBitmap_SmallIcons);
        interface_graphics->LoadChatTabs(interface_library_copy, InterfaceBitmap_ChatTabs);
        interface_graphics->LoadCornerButton(interface_library_copy, InterfaceBitmap_CornerButton);
        interface_graphics->LoadSmallButtons(interface_library_copy, InterfaceBitmap_SmallButtons);
        interface_graphics->LoadInventoryButtons(interface_library_copy, InterfaceBitmap_InventoryButtons);
        interface_graphics->LoadChatModeIcons(interface_library_copy, InterfaceBitmap_ChatModeIcons);
        interface_graphics->LoadTabButtons(interface_library_copy, InterfaceBitmap_TabButtons);
        interface_graphics->LoadListIcons(interface_library_copy, InterfaceBitmap_ListIcons);
        interface_graphics->LoadTileCursor(interface_library_copy, InterfaceBitmap_TileCursor);
        interface_graphics->LoadControlSheet(interface_library_copy, InterfaceBitmap_ControlSheet);
        interface_graphics->LoadScrollIcons(interface_library_copy, InterfaceBitmap_ScrollIcons);
        interface_graphics->LoadMapLoadingBox(interface_library_copy, InterfaceBitmap_MapLoadingBox);
        interface_graphics->LoadEmoteSheet(interface_library_copy, InterfaceBitmap_EmoteSheet);
        interface_graphics->LoadWeaponEffects(interface_library_copy, InterfaceBitmap_WeaponEffects);
        interface_graphics->LoadGroupIcons(interface_library_copy, InterfaceBitmap_GroupIcons);
        interface_graphics->LoadMinimapTiles(interface_library_copy, InterfaceBitmap_MinimapTiles);
        interface_graphics->LoadStatusIcons(interface_library_copy, InterfaceBitmap_StatusIcons);
        interface_graphics->LoadHudIcons(interface_library_copy, InterfaceBitmap_HudIcons);
        interface_graphics->LoadHudSheet(interface_library_copy, InterfaceBitmap_HudSheet);
        interface_graphics->LoadElementIcons(interface_library_copy, InterfaceBitmap_ElementIcons);
        interface_graphics->LoadPopupIcons(interface_library_copy, InterfaceBitmap_PopupIcons);
        interface_graphics->LoadPanelBackground(interface_library_copy, PanelBackground_Status);
        game->LoadPopupMenu(interface_library_copy, InterfaceBitmap_PopupMenu);
        game->LoadPaperdollBackground(interface_library_copy, InterfaceBitmap_PaperdollBackground);
        game->LoadInputBoxBackground(interface_library_copy, InterfaceBitmap_InputBoxBackground);
        game->LoadListBoxBackground(interface_library_copy, InterfaceBitmap_ListBoxBackground);
        game->LoadSpareSurface2(interface_library_copy, InterfaceBitmap_BookBackground);
        if (!game->LoadSpareSurface(interface_library_copy, InterfaceBitmap_SpareSurface))
            MainForm->animation->menu->loading_tip =
                "Warning! Missing .egf resources, errors may occure during gameplay. Please patch your client";
    }
}

// The file name of a graphics library, gfx\gfxNNN.egf.
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

// Loads the skills panel background into the shared panel surface.
void GfxController::LoadSkillPanel()
{
    int library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (library)
        interface_graphics->LoadPanelBackground(library, PanelBackground_Skills);
}

// Loads the spells panel background into the shared panel surface; the skills panel uses the same bitmap.
void GfxController::LoadSpellPanel()
{
    int library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (library)
        interface_graphics->LoadPanelBackground(library, PanelBackground_Skills);
}

// Loads the help panel background into the shared panel surface.
void GfxController::LoadHelpPanel()
{
    int library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (library)
        interface_graphics->LoadPanelBackground(library, PanelBackground_Help);
}

// Loads the chat panel background into the shared panel surface.
void GfxController::LoadChatPanel()
{
    int library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (library)
        interface_graphics->LoadPanelBackground(library, PanelBackground_Chat);
}

// Loads the status panel background into the shared panel surface.
void GfxController::LoadStatusPanel()
{
    int library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (library)
        interface_graphics->LoadPanelBackground(library, PanelBackground_Status);
}

// Loads the player list panel background into the shared panel surface.
void GfxController::LoadPlayerlistPanel()
{
    int library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (library)
        interface_graphics->LoadPanelBackground(library, PanelBackground_PlayerList);
}

// Loads the group panel background into the shared panel surface.
void GfxController::LoadGroupPanel()
{
    int library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (library)
        interface_graphics->LoadPanelBackground(library, PanelBackground_Group);
}

// Loads the inventory panel background into the shared panel surface.
void GfxController::LoadInventoryPanel()
{
    int library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (library)
        interface_graphics->LoadPanelBackground(library, PanelBackground_Inventory);
}

// Loads the settings panel background into the shared panel surface.
void GfxController::LoadSettingsPanel()
{
    int library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (library)
        interface_graphics->LoadPanelBackground(library, PanelBackground_Settings);
}

// Loads the news panel background into the shared panel surface.
void GfxController::LoadNewsPanel()
{
    int library = MainForm->gfx_vault->OpenLibrary(INTERFACE_GFX_FILE);
    if (library)
        interface_graphics->LoadPanelBackground(library, PanelBackground_News);
}

// Reloads the menu background picked by background_id.
void GfxController::LoadMenuBackground()
{
    int library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MenuInterface);
    if (library)
        menu->LoadBackground(library, menu->background_id);
}

// Shows the next menu background, 130 to 136 in turn, with the artwork on it.
void GfxController::NextMenuBackground()
{
    int library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MenuInterface);
    if (library)
    {
        menu->background_id++;
        if (menu->background_id > 136)
            menu->background_id = 130;
        menu->LoadBackground(library, menu->background_id);
        menu->DrawArtworkOnBackground(MENU_ARTWORK_X, MENU_ARTWORK_Y);
    }
}

// Loads the menu background and artwork, a random one of four when asked, and draws the artwork on it.
void GfxController::LoadMainMenuScreen(bool random_artwork)
{
    int library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MenuInterface);
    if (library)
    {
        int library_copy = library;
        menu->LoadBackground(library_copy, menu->background_id);
        if (random_artwork)
            menu->artwork_id = random(4) + 141;
        menu->LoadArtwork(library_copy, menu->artwork_id);
        menu->DrawArtworkOnBackground(MENU_ARTWORK_X, MENU_ARTWORK_Y);
    }
}

// Draws the create account screen's background: the menu backdrop, the form labels and a random side artwork.
void GfxController::LoadCreateAccountScreen()
{
    int library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MenuInterface);
    if (library)
    {
        int library_copy = library;
        menu->LoadBackground(library_copy, menu->background_id);
        menu->DrawFormLabels(0, 0);
        int artwork_id = random(8) + 161;
        side_artwork_id = artwork_id;
        menu->LoadArtwork(library_copy, artwork_id);
        menu->DrawArtworkOnBackground(SIDE_ARTWORK_X, SIDE_ARTWORK_Y);
    }
}

// Draws the sign-up code screen's background for the style, with a new random side artwork or the last one.
void GfxController::LoadSignupCodeScreen(int style, bool random_artwork)
{
    int artwork_id;
    int library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MenuInterface);
    if (library)
    {
        int library_copy = library;
        menu->LoadBackground(library_copy, menu->background_id);
        menu->DrawSignupFormBackground(0, 0, style);
        if (random_artwork)
        {
            artwork_id = random(8) + 161;
            side_artwork_id = artwork_id;
        }
        else
            artwork_id = side_artwork_id;
        menu->LoadArtwork(library_copy, artwork_id);
        menu->DrawArtworkOnBackground(SIDE_ARTWORK_X, SIDE_ARTWORK_Y);
    }
}

// Draws the password reset screen's background for the step, with a new random side artwork or the last one.
void GfxController::LoadPasswordResetScreen(int step, bool random_artwork)
{
    int artwork_id;
    int library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MenuInterface);
    if (library)
    {
        int library_copy = library;
        menu->LoadBackground(library_copy, menu->background_id);
        menu->DrawPasswordResetBackground(0, 0, step);
        if (random_artwork)
        {
            artwork_id = random(8) + 161;
            side_artwork_id = artwork_id;
        }
        else
            artwork_id = side_artwork_id;
        menu->LoadArtwork(library_copy, artwork_id);
        menu->DrawArtworkOnBackground(SIDE_ARTWORK_X, SIDE_ARTWORK_Y);
    }
}

// Loads the menu background and artwork with the upper panel drawn on it.
void GfxController::LoadPasswordScreen()
{
    int menu_library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MenuInterface);
    if (menu_library)
    {
        int menu_library_copy = menu_library;
        menu->LoadBackground(menu_library_copy, menu->background_id);
        menu->LoadArtwork(menu_library_copy, menu->artwork_id);
        menu->DrawArtworkOnBackground(MENU_ARTWORK_X, MENU_ARTWORK_Y);
        menu->DrawUpperPanelOnBackground(266, 184);
    }
}

// Loads the menu background and artwork with the lower panel drawn on it.
void GfxController::LoadLoginScreen()
{
    int menu_library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MenuInterface);
    if (menu_library)
    {
        int menu_library_copy = menu_library;
        menu->LoadBackground(menu_library_copy, menu->background_id);
        menu->LoadArtwork(menu_library_copy, menu->artwork_id);
        menu->DrawArtworkOnBackground(MENU_ARTWORK_X, MENU_ARTWORK_Y);
        menu->DrawLowerPanelOnBackground(266, 284);
    }
}

// Draws the character selection backdrop: the three slot frames and a random side artwork.
void GfxController::LoadCharacterSelectScreen()
{
    int menu_library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MenuInterface);
    if (menu_library)
    {
        int menu_library_copy = menu_library;
        menu->LoadBackground(menu_library_copy, menu->background_id);
        menu->DrawSlotFrameOnBackground(334, 36);
        menu->DrawSlotFrameOnBackground(334, 160);
        menu->DrawSlotFrameOnBackground(334, 284);
        menu->LoadArtwork(menu_library_copy, random(8) + 161);
        menu->DrawArtworkOnBackground(SIDE_ARTWORK_X, SIDE_ARTWORK_Y);
    }
}

// Loads the character sprite sheets, from standing to the kind sheet, from their library.
void GfxController::LoadSpriteGraphics()
{
    int library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_CharacterBodies);
    if (library)
    {
        int library_copy = library;
        sprites->LoadStandSheet(library_copy, BodyBitmap_StandSheet);
        sprites->LoadWalkSheet(library_copy, BodyBitmap_WalkSheet);
        sprites->LoadAttackSheet(library_copy, BodyBitmap_AttackSheet);
        sprites->LoadSpellCastSheet(library_copy, BodyBitmap_SpellCastSheet);
        sprites->LoadChairSitSheet(library_copy, BodyBitmap_ChairSitSheet);
        sprites->LoadFloorSitSheet(library_copy, BodyBitmap_FloorSitSheet);
        sprites->LoadRangedAttackSheet(library_copy, BodyBitmap_RangedAttackSheet);
        sprites->LoadFaceSheet(library_copy, BodyBitmap_FaceSheet);
        sprites->LoadKindSheet(library_copy, BodyBitmap_KindSheet);
    }
}

// Loads the create-character panel background.
void GfxController::LoadCreateCharacterPanel()
{
    int library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MenuInterface);
    if (library)
        menu->LoadCreateCharacterBackground(library, MenuBitmap_CreateCharacterPanel);
}

// Loads the change-password panel into the surface the create-character panel uses.
void GfxController::LoadChangePasswordPanel()
{
    int library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MenuInterface);
    if (library)
        menu->LoadCreateCharacterBackground(library, MenuBitmap_ChangePasswordPanel);
}
