#ifndef FileprotectionH
#define FileprotectionH

#include "Datafile.h"

#define GFX_LIBRARY_COUNT 25

#include <Classes.hpp>

// The start-up file check: the files the client needs, each with an expected size, and the error text of a failure.
struct FileController
{
    String error_message;
    TStringList *file_names;
    TStringList *file_sizes;

    FileController();
    ~FileController();
    void BuildFileList();
    char VerifyFiles();
    bool PrefixAppDirectory();
    String GetGfxPath(int file_number);
    String GetSoundPath(int file_number);
    String GetMusicPath(int file_number);
    String GetDataFilePath(int file_number);
    String RestorePathSeparators(String path);
    String UnscrambleString(int divisor, unsigned char *data_begin,
        unsigned char *data_end);
};

#endif
