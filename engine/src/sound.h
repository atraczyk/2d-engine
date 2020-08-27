//-----------------------------------------------------------------------------
//  2d platform game engine and level editor by Andreas Traczyk (2014-15)
//  http://andreastraczyk.com/  email: andreastraczyk@gmail.com
//
//  DESCRIPTION:    sound file player
//-----------------------------------------------------------------------------

#ifndef SOUND_H
#define SOUND_H

#include "shared.h"

#if defined(USE_FMOD)
#pragma comment(lib, "fmod/fmodex_vc.lib" )
#include "fmod/fmod.hpp"
#endif

#define NUM_SOUNDS              11

enum SOUND_ID
{
    SOUND_MUSIC1,
    SOUND_MUSIC2,
    SOUND_JUMP,
    SOUND_THUD,
    SOUND_SELECT1,
    SOUND_SELECT2,
    SOUND_SELECT3,
    SOUND_COIN,
    SOUND_STOMP,
    SOUND_HURT,
    SOUND_SWIM
};

class Sound
{
public:
    float masterVolume;
    float musicVolume;
    float sfxVolume;
    int cantPlayGUISound;
    int cantPlaySfxSound;

    Sound();
    ~Sound();

    bool load();
    void PlayMusic(SOUND_ID sound_id);
    void PlaySfx(SOUND_ID sound_id);
    void PlayGUISfx(SOUND_ID sound_id);
    void changeMasterVolume(float amount);
    void changeMusicVolume(float amount);
    void changeSfxVolume(float amount);
    void StopAll();
    void setReverb(bool state);
    void cacheReverb(bool state);
    void update();

#if defined(USE_FMOD)
    FMOD_REVERB_PROPERTIES last_reverb_props;
    FMOD::System*     system;
    FMOD::Sound*      sounds[NUM_SOUNDS];
    FMOD::Channel*    musicChannel;
    FMOD::Channel*    sfxChannel;
    FMOD::Channel*    guisfxChannel;
#endif
};

#endif