#include <vcl.h>
#pragma hdrstop

#include "Sfxprotection.h"
#include "Tmainform.h"
#include "Midiplayer.h"

#pragma package(smart_init)

// Creates the midi player when music is on; music is switched off when the player does not start.
SfxController::SfxController(int sound_level, int music_level)
{
    sound_volume = sound_level;
    music_volume = music_level;
    play_from_jukebox = false;
    music_pending = false;
    unused_value = -1;
    music_retry_countdown = -1;
    ambient_sound_id = 0;
    fast_fade = false;
    midi_player = new Dx_MidiPlayer();
    if (MainForm->config->GetMusicVolume() > 0)
    {
        midi_player->Initialize();
        if (!midi_player->is_initialized)
        {
            MainForm->config->SetMusicAvailable(0);
            MainForm->config->SetMusicEnabled(0);
            return;
        }
        midi_player->SetVolume(music_level);
    }
}

SfxController::~SfxController()
{
}

void SfxController::InitMidiPlayer()
{
    midi_player->Initialize();
}

void SfxController::PlayInstrumentNote(int instrument_id, int note_id)
{
    PlaySoundFile(MainForm->strings->GetInstrumentNotePath(instrument_id, note_id), 0);
}

void SfxController::PlayEffectSoundAt(int sound_id, short volume)
{
    PlaySoundFile(MainForm->strings->GetSoundPath(sound_id), volume);
}

void SfxController::PlaySfx(int sound_id)
{
    PlaySoundFile(MainForm->strings->GetSoundPath(sound_id), 0);
}

// Plays a sound file on the next free effect buffer, as many as the sound buffers setting allows; nothing while the
// sound is off, the window is minimized or the volume is too low.
void SfxController::PlaySoundFile(String file_path, int volume)
{
    volume = ScaleVolume(volume, sound_volume);
    if (volume < -3000)
        return;
    if (sound_volume > 0 && !MainForm->minimized)
    {
        char is_played = false;
        if (!is_played && !MainForm->Dx_SoundBuffer2->Created)
        {
            if (MainForm->Dx_SoundBuffer2->CreateFromFile(file_path.c_str(), MainForm->DSBufferDesc2,
                MainForm->Dx_Sound1))
            {
                MainForm->Dx_SoundBuffer2->SetVolume(volume);
                try
                {
                    MainForm->Dx_SoundBuffer2->Play(0, 0);
                }
                catch (...)
                {
                }
                is_played = true;
            }
        }
        if (!is_played && !MainForm->Dx_SoundBuffer3->Created &&
            MainForm->Dx_SoundBuffer3->CreateFromFile(file_path, MainForm->DSBufferDesc3, MainForm->Dx_Sound1))
        {
            MainForm->Dx_SoundBuffer3->SetVolume(volume);
            try
            {
                MainForm->Dx_SoundBuffer3->Play(0, 0);
            }
            catch (...)
            {
            }
            is_played = true;
        }
        if (!is_played && !MainForm->Dx_SoundBuffer4->Created &&
            MainForm->Dx_SoundBuffer4->CreateFromFile(file_path, MainForm->DSBufferDesc4, MainForm->Dx_Sound1))
        {
            MainForm->Dx_SoundBuffer4->SetVolume(volume);
            try
            {
                MainForm->Dx_SoundBuffer4->Play(0, 0);
            }
            catch (...)
            {
            }
            is_played = true;
        }
        if (!is_played && !MainForm->Dx_SoundBuffer5->Created &&
            MainForm->Dx_SoundBuffer5->CreateFromFile(file_path, MainForm->DSBufferDesc5, MainForm->Dx_Sound1))
        {
            MainForm->Dx_SoundBuffer5->SetVolume(volume);
            try
            {
                MainForm->Dx_SoundBuffer5->Play(0, 0);
            }
            catch (...)
            {
            }
            is_played = true;
        }
        if (!is_played && !MainForm->Dx_SoundBuffer6->Created &&
            MainForm->Dx_SoundBuffer6->CreateFromFile(file_path, MainForm->DSBufferDesc6, MainForm->Dx_Sound1))
        {
            MainForm->Dx_SoundBuffer6->SetVolume(volume);
            try
            {
                MainForm->Dx_SoundBuffer6->Play(0, 0);
            }
            catch (...)
            {
            }
            is_played = true;
        }
        if (!is_played && !MainForm->Dx_SoundBuffer7->Created &&
            MainForm->Dx_SoundBuffer7->CreateFromFile(file_path, MainForm->DSBufferDesc7, MainForm->Dx_Sound1))
        {
            MainForm->Dx_SoundBuffer7->SetVolume(volume);
            try
            {
                MainForm->Dx_SoundBuffer7->Play(0, 0);
            }
            catch (...)
            {
            }
        }
    }
}

// Scales a sound's volume by the sound setting's level, from silent at 0 to the full volume at 100, at least 20.
int SfxController::ScaleVolume(int volume, int level)
{
    int floor;
    if (volume <= -3000)
        return -3000;
    if (level <= 0)
        return -3000;
    if (level >= 100)
        return volume;
    else
    {
        if (level < 20)
            level = 20;
        float quiet = (100 - level) / 100.0;
        floor = -3000.0 * quiet;
        return (int)(volume * (1.0F - quiet)) + floor;
    }
}

// Plays a map tune, or the jukebox song when the tune was queued by the jukebox.
void SfxController::PlayMusic(int music_id)
{
    if (music_volume <= 0)
        return;
    if (midi_player->is_initialized)
    {
        if (midi_player->IsSegmentPlaying())
            midi_player->Stop();
        if (!play_from_jukebox)
            midi_player->Play(MainForm->strings->GetMusicPath(music_id));
        else
        {
            midi_player->Play(MainForm->strings->GetJukeboxMusicPath(music_id));
            play_from_jukebox = false;
        }
    }
}

// Queues the jukebox song to play next, when music is on.
void SfxController::QueueJukeboxSong(int song_id)
{
    if (music_volume > 0 && midi_player->is_initialized)
    {
        play_from_jukebox = true;
        queued_music_id = song_id;
        music_pending = true;
    }
}

// Queues the map tune to play next, when music is on.
void SfxController::QueueMusic(int music_id)
{
    if (music_volume > 0 && midi_player->is_initialized)
    {
        play_from_jukebox = false;
        queued_music_id = music_id;
        music_pending = true;
    }
}

char SfxController::TakeMusicPending()
{
    char was_pending = music_pending;
    music_pending = false;
    return was_pending;
}

int SfxController::GetQueuedMusic()
{
    return queued_music_id;
}

char SfxController::IsMusicPlaying()
{
    char is_playing = false;
    if (midi_player->is_initialized)
        is_playing = midi_player->IsSegmentPlaying();
    return is_playing;
}

// Stops the tune and forgets the map's music.
void SfxController::StopMusic()
{
    MainForm->map->ClearMusic();
    if (midi_player->is_initialized && midi_player->IsSegmentPlaying())
        midi_player->Stop();
}

// The looping sound of a map spot: 100 quieter per step of distance; a different spot's sound fades this one out.
void SfxController::PlayAmbientSound(int sound_id, short distance)
{
    if (sound_volume > 0 && !MainForm->minimized)
    {
        if (MainForm->animation->GetScreenMode() != ScreenMode_Game)
            return;
        if (sound_id == ambient_sound_id)
            ambient_target_volume = -100 * distance;
        if (!ambient_sound_id)
        {
            String sound_path = MainForm->strings->GetSoundPath(sound_id);
            if (!MainForm->Dx_SoundBuffer1->Created &&
                MainForm->Dx_SoundBuffer1->CreateFromFile(sound_path, MainForm->DSBufferDesc1, MainForm->Dx_Sound1))
            {
                ambient_sound_id = sound_id;
                ambient_target_volume = -100 * distance;
                ambient_volume = -100 * distance;
                fast_fade = false;
                ambient_volume = ScaleVolume(ambient_volume, sound_volume);
                MainForm->Dx_SoundBuffer1->SetVolume(ambient_volume);
                MainForm->Dx_SoundBuffer1->Play(0, DSBPLAY_LOOPING);
                MainForm->Dx_SoundBuffer1->SetVolume(ambient_volume);
            }
        }
        if (sound_id != ambient_sound_id)
            ambient_target_volume = -2600;
    }
}

// Fades the ambient sound out quickly.
void SfxController::FadeOutAmbientSound()
{
    if (ambient_sound_id > 0)
    {
        ambient_target_volume = -3000;
        fast_fade = true;
    }
}

// Frees finished effect buffers and moves the ambient volume 50 a frame toward its target; at -2600 it stops.
void SfxController::UpdateSounds()
{
    if (music_retry_countdown > 0)
        music_retry_countdown--;
    if (volume_countdown > 0)
    {
        if (volume_countdown == 1)
            StopMusic();
        volume_countdown--;
    }
    if (sound_volume > 0)
    {
        if (!MainForm->Dx_SoundBuffer7->Playing && MainForm->Dx_SoundBuffer7->Created)
            MainForm->Dx_SoundBuffer7->Destroy();
        if (!MainForm->Dx_SoundBuffer6->Playing && MainForm->Dx_SoundBuffer6->Created)
            MainForm->Dx_SoundBuffer6->Destroy();
        if (!MainForm->Dx_SoundBuffer5->Playing && MainForm->Dx_SoundBuffer5->Created)
            MainForm->Dx_SoundBuffer5->Destroy();
        if (!MainForm->Dx_SoundBuffer4->Playing && MainForm->Dx_SoundBuffer4->Created)
            MainForm->Dx_SoundBuffer4->Destroy();
        if (!MainForm->Dx_SoundBuffer3->Playing && MainForm->Dx_SoundBuffer3->Created)
            MainForm->Dx_SoundBuffer3->Destroy();
        if (!MainForm->Dx_SoundBuffer2->Playing && MainForm->Dx_SoundBuffer2->Created)
            MainForm->Dx_SoundBuffer2->Destroy();
        if (ambient_sound_id > 0)
        {
            if (ambient_volume > ambient_target_volume)
            {
                if (fast_fade)
                    ambient_volume -= 200;
                else
                    ambient_volume -= 50;
                MainForm->Dx_SoundBuffer1->SetVolume(ambient_volume);
            }
            if (ambient_volume < ambient_target_volume)
            {
                ambient_volume += 50;
                MainForm->Dx_SoundBuffer1->SetVolume(ambient_volume);
            }
            if (ambient_volume <= -2600)
            {
                if (MainForm->Dx_SoundBuffer1->Created)
                    MainForm->Dx_SoundBuffer1->Destroy();
                ambient_sound_id = 0;
            }
        }
    }
    else if (ambient_sound_id > 0)
    {
        ambient_volume -= 200;
        MainForm->Dx_SoundBuffer1->SetVolume(ambient_volume);
        if (ambient_volume <= -2600)
        {
            if (MainForm->Dx_SoundBuffer1->Created)
                MainForm->Dx_SoundBuffer1->Destroy();
            ambient_sound_id = 0;
        }
    }
}

bool SfxController::IsMusicRetryReady()
{
    bool is_ready = true;
    if (music_retry_countdown > 0)
        is_ready = false;
    return is_ready;
}

void SfxController::StartMusicRetryDelay()
{
    music_retry_countdown = 30;
}

void SfxController::SetSoundEnabled(int enabled)
{
    sound_volume = enabled;
}

// Sets the music volume; turning it off starts a 30 tick countdown.
void SfxController::SetMusicVolume(int volume)
{
    music_volume = volume;
    midi_player->SetVolume(volume);
    if (volume > 0)
        volume_countdown = 0;
    else
        volume_countdown = 30;
}

// A volume for sounds at the tile, 200 lower for each step from the own character.
int SfxController::GetDistanceVolume(short x, short y)
{
    int player_x = MainForm->players->GetX(MainForm->net_client->GetPlayerId());
    int player_y = MainForm->players->GetY(MainForm->net_client->GetPlayerId());
    int distance = abs(x - player_x);
    distance += abs(y - player_y);
    return -200 * distance;
}
