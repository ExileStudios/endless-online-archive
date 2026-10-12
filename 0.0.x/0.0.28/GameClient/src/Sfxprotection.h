#ifndef SfxprotectionH
#define SfxprotectionH

struct Dx_MidiPlayer;

// Sound effects on six sound buffers, the looping ambient sound of a map spot, and the music.
struct SfxController
{
    Dx_MidiPlayer *midi_player;
    char sound_enabled;
    bool play_from_jukebox;
    int ambient_sound_id;
    int ambient_volume;
    int ambient_target_volume;
    bool fast_fade;
    char pad_0x15[11];
    int unread_int_at_32;
    bool music_pending;
    int queued_music_id;
    int music_retry_countdown;

    SfxController(char sound_on);
    ~SfxController();
    void CreateMidiPlayer();
    void PlayInstrumentNote(int instrument_id, int note_id);
    void PlaySfx(int sound_id);
    void PlaySoundFile(String file_path);
    void PlayMusic(int music_id);
    void QueueJukeboxSong(int song_id);
    void QueueMusic(int music_id);
    char TakeMusicPending();
    int GetQueuedMusic();
    char IsMusicPlaying();
    void StopMusic();
    void PlayAmbientSound(int sound_id, int distance);
    void FadeOutAmbientSound();
    void UpdateSounds();
    bool IsMusicRetryReady();
    void StartMusicRetryDelay();
    void SetSoundEnabled(char enabled);
};

#endif
