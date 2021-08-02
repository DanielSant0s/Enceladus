#include <stdlib.h>
#include <malloc.h>
#include "mikmod.h"

#include "sound.h"

#define MAX_MUSIC_CHAN 128      
#define MAX_SFX_CHAN 32
static  MODULE *musichandle = NULL;

//Internals. Implement this somewhere else to get error reporting lua-style.
void my_mikmod_error_handler(void)
{
	printf("LuaPlayer's Mikmod has a critical error:\n");
	printf("_mm_critical %d\n", MikMod_critical);
	printf("_mm_errno %d\n", MikMod_errno);
	printf("%s\n", MikMod_strerror(MikMod_errno));
	return;
}
extern void my_error_handler();

// For internal Lua use

void initMikmod() {
	MikMod_RegisterErrorHandler(my_mikmod_error_handler);
	MikMod_RegisterAllDrivers();
	MikMod_RegisterAllLoaders();
	md_mode = DMODE_16BITS|DMODE_STEREO|DMODE_SOFT_SNDFX|DMODE_SOFT_MUSIC; 
	md_reverb = 0;
	md_pansep = 128;
	md_volume = 128;
	md_musicvolume = 96;
	md_sndfxvolume = 128;
	musichandle = NULL;
	
	if (MikMod_Init(""))
  	{
    	   printf("Could not initialize sound, reason: %s\n", MikMod_strerror(MikMod_errno));
  	}
	MikMod_SetNumVoices(MAX_MUSIC_CHAN, MAX_SFX_CHAN);
	
	Player_Start((MODULE*) 0);
	MikMod_EnableOutput();
	
	printf ("initMikmod done\n");
}

void unloadMikmod() {
	if(musichandle) Player_Free(musichandle);
	Player_Stop();
	MikMod_Exit();
}

// For external Lua use
static unsigned oldvol = 0;

void loadAndPlayMusicFile(char* filename, BOOL loop) {
	if(musichandle) stopAndUnloadMusic();
	if(oldvol) md_musicvolume = oldvol;
	musichandle = Player_Load(filename, MAX_MUSIC_CHAN,0);
	musichandle->loop = (loop == true);
	Player_Start(musichandle);
}

void stopAndUnloadMusic() {
	Player_Stop();
	Player_Free(musichandle);
	musichandle = NULL;
}

void musicPause() {
	oldvol = md_musicvolume;
	md_musicvolume = 0;
	//MP_HandleTick(); not anymore present in libmikmod 3.1.11
	Player_Stop();
}
void musicResume() {
	Player_Start(musichandle);
	md_musicvolume = oldvol;
}


Sound* loadSound(char* filename) {
	return Sample_Load(filename);
}

void unloadSound(Sound* handle) {
	Sample_Free(handle);
}


Voice playSound(Sound* handle) {
	if(handle) { // Just for good measure, even if it's a precondition...
		Voice voice = Sample_Play(handle,0,0);
		setVoicePanning(voice, 127);
		setVoiceVolume(voice, 255);
		return voice;
	}
	return 0;
}

void stopSound(Voice handle) {
	Voice_Stop(handle);
}

void resumeSound(Voice handle, Sound* soundhandle) {
	printf("Sample resuming crashes. To spare you the pain, it has been disabled.\n");
	//Voice_Play(handle, soundhandle, Voice_GetPosition(handle));
}


void setSoundLooping(Sound *handle, int loopmode, unsigned long loopstart, unsigned long loopend) { // loopmode: 0 - no loop, 1 - loop start to end, 2 - take arguments
	if(!handle) return;
	if(loopmode == 0) {
		handle->loopstart = 0;
		handle->loopend = 0;
		return;
	} else if(loopmode == 1) {
		handle->loopstart = 0;
		handle->loopend = handle->length;
	} else if(loopmode == 2) {
		handle->loopstart = loopstart;
		handle->loopend = loopend;
	}
	handle->flags |= SF_LOOP | SFX_CRITICAL;
}

unsigned long getSoundLengthInSamples(Sound *handle) {
	if(!handle) return 0;
	return handle->length;
}

unsigned long getSoundSampleSpeed(Sound *handle) {
	if(!handle) return 0;
	return handle->speed;
}



void setVoiceVolume(Voice handle, UWORD vol) {
	if(vol>255) vol = 255;
	Voice_SetVolume(handle, vol);
}



void setVoicePanning(Voice handle, ULONG pan) {
	if(pan>255) pan = 255;
	Voice_SetPanning(handle, pan);
}


void setVoiceFrequency(Voice handle, ULONG freq) {
	if(freq>100000) freq = 100000;
	Voice_SetFrequency(handle, freq);
}

extern BOOL musicIsPlaying() {
	return Player_Active();
}

extern BOOL voiceIsPlaying(Voice handle) {
	return !Voice_Stopped(handle);
}


extern unsigned setMusicVolume(unsigned arg) {
	if(arg!= 9999) {
		if(arg>128) arg = 128;
		md_musicvolume = arg;
	}
	return md_musicvolume;
}
extern unsigned setSFXVolume(unsigned arg) {
	if(arg!= 9999) {
		if(arg>128) arg = 128;
		md_sndfxvolume = arg;
	}
	return md_sndfxvolume;
}
extern unsigned setReverb(unsigned arg) {
	if(arg!= 9999) {
		if(arg>15) arg = 15;
		md_reverb = arg;
	}
	return md_reverb;
}
extern unsigned setPanSep(unsigned arg) {
	if(arg!= 9999) {
		if(arg>128) arg = 128;
		md_pansep = arg;
	}
	return md_pansep;
}
