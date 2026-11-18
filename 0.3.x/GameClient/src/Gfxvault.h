#ifndef GfxvaultH
#define GfxvaultH

struct GfxVault
{
    String file_names[28];
    void *libraries[28];
    char keep_open[28];
    char is_open[28];
    short idle_ticks[28];
    int unused_values[28];

    GfxVault();
    ~GfxVault();
    int OpenLibrary(int file_number);
    String GetGfxFileName(int file_number);
    void SetGfxFileName(int file_number, String path);
    int GetLibrary(int file_number);
    void AgeLibraries();
    void CloseLibrary(int file_number);
    void CloseAllLibraries();
};

#endif
