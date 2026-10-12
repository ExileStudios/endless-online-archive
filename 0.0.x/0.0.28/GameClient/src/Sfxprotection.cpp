#include <vcl.h>
#pragma hdrstop

#include "Sfxprotection.h"
#include "Tmainform.h"
#include "Midiplayer.h"
#pragma package(smart_init)

// Creates the midi player when music is on; music is switched off when the player does not start.
SfxController::SfxController(char sound_on)
{
    sound_enabled = sound_on;
    play_from_jukebox = false;
    music_pending = false;
    unread_int_at_32 = -1;
    music_retry_countdown = -1;
    ambient_sound_id = 0;
    fast_fade = false;
    if (MainForm->config->GetMusicEnabled())
    {
        midi_player = new Dx_MidiPlayer();
        if (!midi_player->is_initialized)
        {
            MainForm->config->SetMusicAvailable(0);
            MainForm->config->SetMusicEnabled(0);
        }
    }
}

SfxController::~SfxController()
{
}

void SfxController::CreateMidiPlayer()
{
    midi_player = new Dx_MidiPlayer();
}

void SfxController::PlayInstrumentNote(int instrument_id, int note_id)
{
    PlaySoundFile(MainForm->strings->GetInstrumentNotePath(instrument_id, note_id));
}

void SfxController::PlaySfx(int sound_id)
{
    PlaySoundFile(MainForm->strings->GetSoundPath(sound_id));
}

// Plays a sound file on the first of the six effect buffers that is free; with all six busy it is not played.
void SfxController::PlaySoundFile(String file_path)
{
    if (sound_enabled && !MainForm->minimized)
    {
        char is_played = 0;
        if (!is_played && !MainForm->Dx_SoundBuffer2->Created)
        {
            if (MainForm->Dx_SoundBuffer2->CreateFromFile(file_path.c_str(), MainForm->DSBufferDesc2,
                MainForm->Dx_Sound1))
            {
                MainForm->Dx_SoundBuffer2->Play(0, 0);
                is_played = 1;
            }
        }
        if (!is_played && !MainForm->Dx_SoundBuffer3->Created &&
            MainForm->Dx_SoundBuffer3->CreateFromFile(file_path, MainForm->DSBufferDesc3, MainForm->Dx_Sound1))
        {
            MainForm->Dx_SoundBuffer3->Play(0, 0);
            is_played = 1;
        }
        if (!is_played && !MainForm->Dx_SoundBuffer4->Created &&
            MainForm->Dx_SoundBuffer4->CreateFromFile(file_path, MainForm->DSBufferDesc4, MainForm->Dx_Sound1))
        {
            MainForm->Dx_SoundBuffer4->Play(0, 0);
            is_played = 1;
        }
        if (!is_played && !MainForm->Dx_SoundBuffer5->Created &&
            MainForm->Dx_SoundBuffer5->CreateFromFile(file_path, MainForm->DSBufferDesc5, MainForm->Dx_Sound1))
        {
            MainForm->Dx_SoundBuffer5->Play(0, 0);
            is_played = 1;
        }
        if (!is_played && !MainForm->Dx_SoundBuffer6->Created &&
            MainForm->Dx_SoundBuffer6->CreateFromFile(file_path, MainForm->DSBufferDesc6, MainForm->Dx_Sound1))
        {
            MainForm->Dx_SoundBuffer6->Play(0, 0);
            is_played = 1;
        }
        if (!is_played && !MainForm->Dx_SoundBuffer7->Created &&
            MainForm->Dx_SoundBuffer7->CreateFromFile(file_path, MainForm->DSBufferDesc7, MainForm->Dx_Sound1))
            MainForm->Dx_SoundBuffer7->Play(0, 0);
    }
}

// Plays a map tune, or the jukebox song when the tune was queued by the jukebox.
void SfxController::PlayMusic(int music_id)
{
    if (MainForm->config->GetMusicEnabled())
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

// Queues a jukebox song the same way, when music is on.
void SfxController::QueueJukeboxSong(int song_id)
{
    if (MainForm->config->GetMusicEnabled())
    {
        play_from_jukebox = true;
        queued_music_id = song_id;
        music_pending = true;
    }
}

// Queues a tune for the main loop to start (TakeMusicPending).
void SfxController::QueueMusic(int music_id)
{
    if (MainForm->config->GetMusicEnabled())
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
    char is_playing = 0;
    if (MainForm->config->GetMusicEnabled())
        is_playing = midi_player->IsSegmentPlaying();
    return is_playing;
}

// Stops the tune and forgets the map's music.
void SfxController::StopMusic()
{
    MainForm->map->ClearMusic();
    if (MainForm->config->GetMusicEnabled())
    {
        if (midi_player->IsSegmentPlaying())
            midi_player->Stop();
    }
}

// The looping sound of a map spot: 100 quieter per step of distance; a different spot's sound fades this one out.
void SfxController::PlayAmbientSound(int sound_id, int distance)
{
    if (sound_enabled && !MainForm->minimized)
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
                MainForm->Dx_SoundBuffer1->SetVolume(ambient_volume);
                MainForm->Dx_SoundBuffer1->Play(0, 1);
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
    if (sound_enabled)
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
    bool is_ready = 1;
    if (music_retry_countdown > 0)
        is_ready = 0;
    return is_ready;
}

void SfxController::StartMusicRetryDelay()
{
    music_retry_countdown = 30;
}

void SfxController::SetSoundEnabled(char enabled)
{
    sound_enabled = enabled;
}
