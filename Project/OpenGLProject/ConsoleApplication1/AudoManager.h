#pragma once

#include <SDL_mixer.h>

// oh ms believer is 264 beats

/*
if (!testArrow1->checkArrow("down") && doingGood)
{ Mix_VolumeMusic(0); doingGood = false; }
else if (testArrow1->checkArrow("down") && !doingGood)
{ Mix_VolumeMusic(MIX_MAX_VOLUME); doingGood = true;}
}
if (keys.find(SDLK_d) != keys.end())
{
if (!testArrow1->checkArrow("right") && doingGood)
{
Mix_VolumeMusic(0); doingGood = false;
}
else if (testArrow1->checkArrow("right") && !doingGood)
{
Mix_VolumeMusic(MIX_MAX_VOLUME); doingGood = true;
}
}
*/

class AudioManager
{

public:
	Mix_Music *music = NULL;
	Mix_Chunk *muffledMusic = NULL;
	bool isMuffled = false;
	string curSongName;
	string curMuffleName;
	
	AudioManager(string fileName, string fileName2) 
	{
		//Load the music 
		curSongName = fileName;
		curMuffleName = fileName2;
		music = Mix_LoadMUS(fileName.c_str());
		muffledMusic = Mix_LoadWAV(fileName2.c_str());
	}
	void play() 
	{
		
		if (!Mix_Playing(-1))// this functon returns true if there is somthing playing on this channel.  
		{
			Mix_PlayChannel(-1, muffledMusic, 0);
		} // the -1 is the channel that wiil be used
		Mix_PlayMusic(music, 1);// the 1 here is how many times the song will repeat.
		
	}
	void pause() {}
	void muffle() 
	{
		if (isMuffled) 
		{
			Mix_VolumeMusic(MIX_MAX_VOLUME);
			isMuffled = false;
		}
		else
		{
			Mix_VolumeMusic(0);
			isMuffled = true;
		}
	}
	void changeSong(string fileName, string fileName2)
	{
		//Load the music 
		curSongName = fileName;
		curMuffleName = fileName2;
		music = Mix_LoadMUS(fileName.c_str());
		muffledMusic = Mix_LoadWAV(fileName2.c_str());
	}
	void reset() 
	{
		music = Mix_LoadMUS(curSongName.c_str());
		muffledMusic = Mix_LoadWAV(curMuffleName.c_str());
	}
	

};
