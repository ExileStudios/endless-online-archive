#ifndef ConfigurationH
#define ConfigurationH

#include <Classes.hpp>
#include <SysUtils.hpp>
#include <ScktComp.hpp>
#include <DBTables.hpp>
#include <string.h>
#include <stdio.h>
#include <vector.h>
#include <deque.h>
#include <list.h>
#include <stack.h>
#include <queue.h>



struct Config;

// The client's settings, read from config/setup.ini and written back to it.
struct Config
{
    bool fullscreen;
    bool skip_intro;
    bool page_flipping;
    bool block_transfer;
    bool music;
    bool sound;
    bool screen_file_found;
    bool stay_on_top;
    bool sizeable;
    bool static_sprites;
    bool static_objects;
    bool static_tiles;
    int language;
    int keyboard;
    bool filter;
    bool filter_all;
    bool log_chat;
    String log_file;
    bool hear_whisper;
    bool interaction;
    bool show_baloons;
    bool show_shadows;
    char loaded_music;
    char loaded_sound;
    String host;
    String port;
    String field_0x2c;
    TStringList *ini_lines;
    TStringList *ini_keys;

    Config();
    ~Config();
    void LoadConfig();
    void SetValue(String name, String value);
    void SaveConfig();
    bool GetFullscreen();
    bool GetPageFlipping();
    bool GetBlockTransfer();
    bool GetMusic();
    bool GetSound();
    bool GetStayOnTop();
    bool GetSizeable();
    bool GetStaticSprites();
    bool GetStaticObjects();
    bool GetStaticTiles();
    int GetLanguage();
    int GetKeyboard();
    String GetHost();
    bool GetFilter();
    bool GetFilterAll();
    bool GetLogChat();
    bool GetHearWhisper();
    bool GetInteraction();
    bool GetShowBaloons();
    bool GetShowShadows();
    String GetLogFile();
    int GetPort();
};


#endif
