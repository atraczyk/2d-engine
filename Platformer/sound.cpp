//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) 
//	http://atraczyk.co.nf/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	sound file playing with fmod
//-----------------------------------------------------------------------------

#include "sound.h"
#include <iostream>

Sound::Sound()
{
	masterVolume = DEFAULT_MASTER_VOLUME;
	musicVolume = DEFAULT_MUSIC_VOLUME;
	sfxVolume = DEFAULT_SFX_VOLUME;
	FMOD::System_Create(&system);
	system->init(32, FMOD_INIT_NORMAL, 0);
	cantPlayGUISound = 0;
	cantPlaySfxSound = 0;
}

Sound::~Sound()
{
	for(int i=0; i<NUM_SOUNDS; i++) 
		sounds[i]->release();
    system->release();
}

bool Sound::load()
{
	system->createStream("sounds/smw_yio1.mp3", FMOD_SOFTWARE | FMOD_LOOP_NORMAL, 0, &sounds[SOUND_MUSIC1]);
	system->createStream("sounds/smw_ug1.mp3", FMOD_SOFTWARE | FMOD_LOOP_NORMAL, 0, &sounds[SOUND_MUSIC2]);
	system->createSound("sounds/smw_jump.wav",  FMOD_HARDWARE, 0, &sounds[SOUND_JUMP]);
	system->createSound("sounds/smw_thud.wav",  FMOD_HARDWARE, 0, &sounds[SOUND_THUD]);
	system->createSound("sounds/select1.wav",  FMOD_HARDWARE, 0, &sounds[SOUND_SELECT1]);
	system->createSound("sounds/select2.wav",  FMOD_HARDWARE, 0, &sounds[SOUND_SELECT2]);
	system->createSound("sounds/select3.wav",  FMOD_HARDWARE, 0, &sounds[SOUND_SELECT3]);
	system->createSound("sounds/smw_coin.wav",  FMOD_HARDWARE, 0, &sounds[SOUND_COIN]);
	system->createSound("sounds/smw_stomp.wav",  FMOD_HARDWARE, 0, &sounds[SOUND_STOMP]);
	system->createSound("sounds/smb_shrink.wav",  FMOD_HARDWARE, 0, &sounds[SOUND_HURT]);
	system->createSound("sounds/smw_swimming.wav",  FMOD_HARDWARE, 0, &sounds[SOUND_SWIM]);
	system->getReverbProperties(&last_reverb_props);
	return true;
}

void Sound::PlayMusic(SOUND_ID sound_id)
{
	system->playSound(FMOD_CHANNEL_FREE, sounds[sound_id], false, &musicChannel);
	musicChannel->setVolume(masterVolume * musicVolume);
}

void Sound::PlaySfx(SOUND_ID sound_id)
{
	if(sound_id == SOUND_STOMP)
	{
		if(cantPlaySfxSound)
			return;
		else
			cantPlaySfxSound = 2;
	}
	system->playSound(FMOD_CHANNEL_FREE,sounds[sound_id], true, &sfxChannel);
	sfxChannel->setVolume(masterVolume * sfxVolume);
	sfxChannel->setPaused(false);
}

void Sound::PlayGUISfx(SOUND_ID sound_id)
{
	if(!cantPlayGUISound)
	{
		cantPlayGUISound = 5;
		system->playSound(FMOD_CHANNEL_FREE,sounds[sound_id], true, &guisfxChannel);
		guisfxChannel->setVolume( 0.25f);
		guisfxChannel->setPaused(false);
	}
}

void Sound::changeMasterVolume(float amount)
{
	masterVolume += amount;
	masterVolume = masterVolume>1.0f?1.0f:(masterVolume<0.0f?0.0f:masterVolume);
	musicChannel->setVolume(masterVolume * musicVolume);
	sfxChannel->setVolume(masterVolume * sfxVolume);
}

void Sound::changeMusicVolume(float amount)
{
	musicVolume += amount;
	musicVolume = musicVolume>1.0f?1.0f:(musicVolume<0.0f?0.0f:musicVolume);
	musicChannel->setVolume(masterVolume * musicVolume);
}

void Sound::changeSfxVolume(float amount)
{
	sfxVolume += amount;
	sfxVolume = sfxVolume>1.0f?1.0f:(sfxVolume<0.0f?0.0f:sfxVolume);
	sfxChannel->setVolume(masterVolume * sfxVolume);
}

void Sound::StopAll()
{
	musicChannel->stop();
	sfxChannel->stop();
}

void Sound::update()
{
	if(cantPlaySfxSound) cantPlaySfxSound--;
	if(cantPlayGUISound) cantPlayGUISound--;
	system->update();
}