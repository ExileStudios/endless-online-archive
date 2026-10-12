#ifndef Ctrl_settingsH
#define Ctrl_settingsH

enum SettingOption
{
    SettingOption_None = 0,
    SettingOption_Sound = 1,
    SettingOption_Music = 2,
    SettingOption_Keyboard = 3,
    SettingOption_Language = 4,
    SettingOption_PrivateMessages = 5,
    SettingOption_Balloons = 6,
    SettingOption_Shadows = 7,
    SettingOption_Filter = 8,
    SettingOption_LogChat = 9,
    SettingOption_Requests = 10
};

// The settings panel of the game screen: ten switches in two columns.
struct SettingsController
{
    int hovered_option;
    char is_dirty;

    SettingsController();
    ~SettingsController();
    bool TestAndClearDirty();
    void MarkDirty();
    void MouseDown(int x, int y);
    void MouseMove(int x, int y);
};

#endif
