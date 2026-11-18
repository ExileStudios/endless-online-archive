#ifndef MidiplayerH
#define MidiplayerH

// Plays the MIDI tunes through DirectMusic: one performance, one loader and the segment of the tune.
struct Dx_MidiPlayer
{
    char is_initialized;
    String file_path;
    char is_playing;

    Dx_MidiPlayer();
    ~Dx_MidiPlayer();
    char Initialize();
    int SetVolume(int volume);
    void Play(String music_path);
    void Stop();
    bool InitDirectMusic();
    void ReleaseDirectMusic();
    bool LoadSegment(char *file_name);
    void PlaySegment();
    void StopSegment();
    char IsSegmentPlaying();
};

#endif
