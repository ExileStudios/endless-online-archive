#ifndef SfxprotectionH
#define SfxprotectionH

enum Music
{
    Music_TitleTheme = 1
};

enum EffectSound
{
    EffectSound_Flames = 1,
    EffectSound_Warp = 2,
    EffectSound_Pentagram = 3,
    EffectSound_Confetti = 4,
    EffectSound_Sparkles = 5,
    EffectSound_Learn = 6,
    EffectSound_Heal = 8,
    EffectSound_Thunder = 9,
    EffectSound_UltimaBlast = 10,
    EffectSound_MagicShield = 11,
    EffectSound_RingOfFire = 12,
    EffectSound_IceBlast = 13,
    EffectSound_EnergyBall = 14,
    EffectSound_Whirl = 15,
    EffectSound_Boulders = 16,
    EffectSound_Aura = 17,
    EffectSound_Heaven = 18,
    EffectSound_IceBall = 19,
    EffectSound_Vanish = 20,
    EffectSound_AdminInteractReply = 21,
    EffectSound_DarkHand = 24,
    EffectSound_Tentacles = 25,
    EffectSound_MagicWhirl = 26,
    EffectSound_PowerWind = 27,
    EffectSound_FireBlast = 28
};

enum ControlSound
{
    ControlSound_PunchAttack = 1,
    ControlSound_MeleeAttack = 2,
    ControlSound_HeavyWeaponAttack = 3,
    ControlSound_Harp = 4,
    ControlSound_Guitar = 5,
    ControlSound_GunAttack = 6,
    ControlSound_BowAttack = 7
};

#include <TDx_SoundBuffer.H>

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
    TDx_SoundBuffer *playing_buffer;
    unsigned char buffer_index;

    SfxController(int sound_level, int music_level);
    ~SfxController();
    void InitMidiPlayer();
    void PlaySfxAt(int sound_id, short volume);
    void PlayInstrumentNote(int instrument_id, int note_id);
    void PlayEffectSoundAt(int sound_id, short volume);
    void PlayEffectSound(int sound_id);
    int PlaySfx(int sound_id);
    void PlayControlSoundAt(int sound_id, short volume);
    void PlayGameSoundAt(int sound_id, short volume);
    void PlayGameSound(int sound_id);
    void PlayInstrumentNoteAt(int instrument_id, int note_id, short volume);
    void PlaySoundFile(String file_path, int volume);
    bool PlayOnBuffer(TDx_SoundBuffer *buffer, String file_path, int volume);
    void SetPlayingFrequency(int pitch);
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
