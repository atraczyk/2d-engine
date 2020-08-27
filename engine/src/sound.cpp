//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15)
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	sound file playing with fmod
//-----------------------------------------------------------------------------

#include "sound.h"
#include <iostream>

Sound::Sound()
{
#if defined(USE_FMOD)
    FMOD::System_Create(&system);
    system->init(32, FMOD_INIT_NORMAL, 0);
#endif
    masterVolume = DEFAULT_MASTER_VOLUME;
    musicVolume = DEFAULT_MUSIC_VOLUME;
    sfxVolume = DEFAULT_SFX_VOLUME;
    cantPlayGUISound = 0;
    cantPlaySfxSound = 0;
}

Sound::~Sound()
{
#if defined(USE_FMOD)
    for (int i = 0; i < NUM_SOUNDS; i++)
        sounds[i]->release();
    system->release();
#endif
}

bool Sound::load()
{
#if defined(USE_FMOD)
    system->createStream("sounds/smw_yio1.mp3", FMOD_SOFTWARE | FMOD_LOOP_NORMAL, 0, &sounds[SOUND_MUSIC1]);
    system->createStream("sounds/smw_ug1.mp3", FMOD_SOFTWARE | FMOD_LOOP_NORMAL, 0, &sounds[SOUND_MUSIC2]);
    system->createSound("sounds/smw_jump.wav", FMOD_HARDWARE, 0, &sounds[SOUND_JUMP]);
    system->createSound("sounds/smw_thud.wav", FMOD_HARDWARE, 0, &sounds[SOUND_THUD]);
    system->createSound("sounds/select1.wav", FMOD_HARDWARE, 0, &sounds[SOUND_SELECT1]);
    system->createSound("sounds/select2.wav", FMOD_HARDWARE, 0, &sounds[SOUND_SELECT2]);
    system->createSound("sounds/select3.wav", FMOD_HARDWARE, 0, &sounds[SOUND_SELECT3]);
    system->createSound("sounds/smw_coin.wav", FMOD_HARDWARE, 0, &sounds[SOUND_COIN]);
    system->createSound("sounds/smw_stomp.wav", FMOD_HARDWARE, 0, &sounds[SOUND_STOMP]);
    system->createSound("sounds/smb_shrink.wav", FMOD_HARDWARE, 0, &sounds[SOUND_HURT]);
    system->createSound("sounds/smw_swimming.wav", FMOD_HARDWARE, 0, &sounds[SOUND_SWIM]);
    system->getReverbProperties(&last_reverb_props);
#endif
    return true;
}

void Sound::PlayMusic(SOUND_ID sound_id)
{
#if defined(USE_FMOD)
    system->playSound(FMOD_CHANNEL_FREE, sounds[sound_id], false, &musicChannel);
    musicChannel->setVolume(masterVolume * musicVolume);
#endif
}

void Sound::PlaySfx(SOUND_ID sound_id)
{
#if defined(USE_FMOD)
    if (sound_id == SOUND_STOMP)
    {
        if (cantPlaySfxSound)
            return;
        else
            cantPlaySfxSound = 2;
    }
    system->playSound(FMOD_CHANNEL_FREE, sounds[sound_id], true, &sfxChannel);
    sfxChannel->setVolume(masterVolume * sfxVolume);
    sfxChannel->setPaused(false);
#endif
}

void Sound::PlayGUISfx(SOUND_ID sound_id)
{
#if defined(USE_FMOD)
    if (!cantPlayGUISound)
    {
        cantPlayGUISound = 5;
        system->playSound(FMOD_CHANNEL_FREE, sounds[sound_id], true, &guisfxChannel);
        guisfxChannel->setVolume(0.25f);
        guisfxChannel->setPaused(false);
    }
#endif
}

void Sound::changeMasterVolume(float amount)
{
#if defined(USE_FMOD)
    masterVolume += amount;
    masterVolume = masterVolume > 1.0f ? 1.0f : (masterVolume < 0.0f ? 0.0f : masterVolume);
    musicChannel->setVolume(masterVolume * musicVolume);
    sfxChannel->setVolume(masterVolume * sfxVolume);
#endif
}

void Sound::changeMusicVolume(float amount)
{
#if defined(USE_FMOD)
    musicVolume += amount;
    musicVolume = musicVolume > 1.0f ? 1.0f : (musicVolume < 0.0f ? 0.0f : musicVolume);
    musicChannel->setVolume(masterVolume * musicVolume);
#endif
}

void Sound::changeSfxVolume(float amount)
{
#if defined(USE_FMOD)
    sfxVolume += amount;
    sfxVolume = sfxVolume > 1.0f ? 1.0f : (sfxVolume < 0.0f ? 0.0f : sfxVolume);
    sfxChannel->setVolume(masterVolume * sfxVolume);
#endif
}

void Sound::StopAll()
{
#if defined(USE_FMOD)
    musicChannel->stop();
    sfxChannel->stop();
#endif
}

void Sound::setReverb(bool state)
{
#if defined(USE_FMOD)
    FMOD_REVERB_PROPERTIES props;
    props = state ? FMOD_PRESET_CAVE : FMOD_PRESET_OFF;
    system->setReverbProperties(&props);
#endif
}

void Sound::cacheReverb(bool state)
{
#if defined(USE_FMOD)
    if (state)
    {
        system->getReverbProperties(&last_reverb_props);
        setReverb(false);
    }
    else
    {
        system->setReverbProperties(&last_reverb_props);
    }
#endif
}

void Sound::update()
{
#if defined(USE_FMOD)
    if (cantPlaySfxSound) cantPlaySfxSound--;
    if (cantPlayGUISound) cantPlayGUISound--;
    system->update();
#endif
}