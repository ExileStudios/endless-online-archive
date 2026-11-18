#ifndef SfxprotectionH
#define SfxprotectionH

enum Music
{
    Music_TitleTheme = 1
};

struct Dx_MidiPlayer;

// Sound effects on six sound buffers, the looping ambient sound of a map spot, and the music.
struct SfxController
{
    Dx_MidiPlayer *midi_player;
    int sound_volume;
    int music_volume;
    bool play_from_jukebox;
    int ambient_sound_id;
    int ambient_volume;
    int ambient_target_volume;
    bool fast_fade;
    char unused_bytes[11];
    int unused_value;
    bool music_pending;
    int queued_music_id;
    int music_retry_countdown;
    short volume_countdown;

    SfxController(int sound_level, int music_level);
    ~SfxController();
    void InitMidiPlayer();
    void PlayInstrumentNote(int instrument_id, int note_id);
    void PlayEffectSoundAt(int sound_id, short volume);
    void PlaySfx(int sound_id);
    void PlaySoundFile(String file_path, int volume);
    int ScaleVolume(int volume, int level);
    void PlayMusic(int music_id);
    void QueueJukeboxSong(int song_id);
    void QueueMusic(int music_id);
    char TakeMusicPending();
    int GetQueuedMusic();
    char IsMusicPlaying();
    void StopMusic();
    void PlayAmbientSound(int sound_id, short distance);
    void FadeOutAmbientSound();
    void UpdateSounds();
    bool IsMusicRetryReady();
    void StartMusicRetryDelay();
    void SetSoundEnabled(int enabled);
    void SetMusicVolume(int volume);
    int GetDistanceVolume(short x, short y);
};

#endif
