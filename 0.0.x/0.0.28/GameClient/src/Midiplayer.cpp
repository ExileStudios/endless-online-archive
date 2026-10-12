#include <vcl.h>
#pragma hdrstop

#include "Midiplayer.h"
#include <mem.h>
#include <string.h>
#include <dmusici.h>
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

IDirectMusicPerformance *g_658380 = NULL;
HWND g_6CB468;
IDirectMusicSegment *g_6CB46C;
IDirectMusicLoader *g_6CB470;

Dx_MidiPlayer::Dx_MidiPlayer()
{
    is_initialized = InitDirectMusic();
    is_playing = 0;
}

Dx_MidiPlayer::~Dx_MidiPlayer()
{
    ReleaseDirectMusic();
}

// Stops the tune that plays, then loads and starts the tune at music_path.
void Dx_MidiPlayer::Play(String music_path)
{
    if (IsSegmentPlaying())
        StopSegment();
    file_path = music_path;
    LoadSegment(file_path.c_str());
    PlaySegment();
    is_playing = 1;
}

void Dx_MidiPlayer::Stop()
{
    is_playing = 0;
    if (IsSegmentPlaying())
        StopSegment();
}

// Creates the DirectMusic performance with the default port and the loader; false when any step fails.
bool Dx_MidiPlayer::InitDirectMusic()
{
    CoInitialize(0);
    if (CoCreateInstance(CLSID_DirectMusicPerformance, 0, 3, IID_IDirectMusicPerformance, (void **)&g_658380) < 0)
        return false;
    if (g_658380->Init(0, 0, g_6CB468) < 0)
        return false;
    if (g_658380->AddPort(0) < 0)
        return false;
    if (CoCreateInstance(CLSID_DirectMusicLoader, 0, 3, IID_IDirectMusicLoader, (void **)&g_6CB470) < 0)
        return false;
    return true;
}

// Unloads and releases the segment, then the loader and the performance.
void Dx_MidiPlayer::ReleaseDirectMusic()
{
    if (g_6CB46C)
    {
        g_6CB46C->SetParam(GUID_Unload, -1, 0, 0, g_658380);
        g_6CB46C->Release();
        g_6CB46C = 0;
    }
    if (g_6CB470)
    {
        g_6CB470->Release();
        g_6CB470 = 0;
    }
    if (g_658380)
    {
        g_658380->CloseDown();
        g_658380->Release();
        g_658380 = 0;
    }
    CoUninitialize();
}

// Loads a MIDI file as the segment, searching from the current folder, and downloads its instruments.
bool Dx_MidiPlayer::LoadSegment(LPCSTR file_name)
{
    DMUS_OBJECTDESC object_desc;
    char Buffer[260];
    WCHAR WideCharStr[260];
    WCHAR Source[260];
    if (!GetCurrentDirectoryA(260, Buffer))
        return false;
    MultiByteToWideChar(0, 1, Buffer, -1, WideCharStr, 260);
    if (g_6CB470->SetSearchDirectory(GUID_DirectMusicAllTypes, WideCharStr, 0) < 0)
        return false;
    memset(&object_desc, 0, sizeof(object_desc));
    object_desc.dwSize = 848;
    MultiByteToWideChar(0, 1, file_name, -1, Source, 260);
    object_desc.guidClass = CLSID_DirectMusicSegment;
    wcscpy(object_desc.wszFileName, Source);
    object_desc.dwValidData = 18;
    if (g_6CB470->GetObject(&object_desc, IID_IDirectMusicSegment, (void **)&g_6CB46C) < 0)
        return false;
    if (g_6CB46C->SetParam(GUID_StandardMIDIFile, -1, 0, 0, g_658380) < 0)
        return false;
    if (g_6CB46C->SetParam(GUID_Download, -1, 0, 0, g_658380) < 0)
        return false;
    return true;
}

void Dx_MidiPlayer::PlaySegment()
{
    if (g_658380->PlaySegment(g_6CB46C, 0, 0, 0) < 0)
        return;
}

void Dx_MidiPlayer::StopSegment()
{
    if (g_658380->Stop(g_6CB46C, 0, 0, 0) < 0)
        return;
}

// True while the segment plays.
char Dx_MidiPlayer::IsSegmentPlaying()
{
    if (g_658380->IsPlaying(g_6CB46C, 0) == 0)
        return 1;
    return 0;
}
