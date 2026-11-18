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

IDirectMusicPerformance *music_performance = NULL;
HWND music_window;
IDirectMusicSegment *music_segment;
IDirectMusicLoader *music_loader;

Dx_MidiPlayer::Dx_MidiPlayer()
{
    is_initialized = InitDirectMusic();
    is_playing = false;
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
    is_playing = true;
}

void Dx_MidiPlayer::Stop()
{
    is_playing = false;
    if (IsSegmentPlaying())
        StopSegment();
}

// Creates the DirectMusic performance with the default port and the loader; false when any step fails.
bool Dx_MidiPlayer::InitDirectMusic()
{
    CoInitialize(0);
    if (CoCreateInstance(CLSID_DirectMusicPerformance, 0, CLSCTX_INPROC, IID_IDirectMusicPerformance,
        (void **)&music_performance) < 0)
        return false;
    if (music_performance->Init(0, 0, music_window) < 0)
        return false;
    if (music_performance->AddPort(0) < 0)
        return false;
    if (CoCreateInstance(CLSID_DirectMusicLoader, 0, CLSCTX_INPROC, IID_IDirectMusicLoader, (void **)&music_loader) < 0)
        return false;
    return true;
}

// Unloads and releases the segment, then the loader and the performance.
void Dx_MidiPlayer::ReleaseDirectMusic()
{
    if (music_segment)
    {
        music_segment->SetParam(GUID_Unload, -1, 0, 0, music_performance);
        music_segment->Release();
        music_segment = 0;
    }
    if (music_loader)
    {
        music_loader->Release();
        music_loader = 0;
    }
    if (music_performance)
    {
        music_performance->CloseDown();
        music_performance->Release();
        music_performance = 0;
    }
    CoUninitialize();
}

// Loads a MIDI file as the segment, searching from the current folder, and downloads its instruments.
bool Dx_MidiPlayer::LoadSegment(LPCSTR file_name)
{
    DMUS_OBJECTDESC object_desc;
    char Buffer[MAX_PATH];
    WCHAR WideCharStr[MAX_PATH];
    WCHAR Source[MAX_PATH];
    if (!GetCurrentDirectoryA(MAX_PATH, Buffer))
        return false;
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, Buffer, -1, WideCharStr, MAX_PATH);
    if (music_loader->SetSearchDirectory(GUID_DirectMusicAllTypes, WideCharStr, 0) < 0)
        return false;
    memset(&object_desc, 0, sizeof(object_desc));
    object_desc.dwSize = 848;
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, file_name, -1, Source, MAX_PATH);
    object_desc.guidClass = CLSID_DirectMusicSegment;
    wcscpy(object_desc.wszFileName, Source);
    object_desc.dwValidData = 18;
    if (music_loader->GetObject(&object_desc, IID_IDirectMusicSegment, (void **)&music_segment) < 0)
        return false;
    if (music_segment->SetParam(GUID_StandardMIDIFile, -1, 0, 0, music_performance) < 0)
        return false;
    if (music_segment->SetParam(GUID_Download, -1, 0, 0, music_performance) < 0)
        return false;
    return true;
}

void Dx_MidiPlayer::PlaySegment()
{
    if (music_performance->PlaySegment(music_segment, 0, 0, 0) < 0)
        return;
}

void Dx_MidiPlayer::StopSegment()
{
    if (music_performance->Stop(music_segment, 0, 0, 0) < 0)
        return;
}

// True while the segment plays.
char Dx_MidiPlayer::IsSegmentPlaying()
{
    if (music_performance->IsPlaying(music_segment, 0) == 0)
        return true;
    return false;
}
