#ifndef GfxvaultH
#define GfxvaultH

struct GfxVault
{
    void *libraries[26];
    char keep_open[26];
    char is_open[26];
    short idle_ticks[26];
    int unused_values[26];

    GfxVault();
    ~GfxVault();
    int OpenLibrary(int file_number);
    int GetLibrary(int file_number);
    void AgeLibraries();
    void CloseLibrary(int file_number);
    void CloseAllLibraries();
};

#endif
