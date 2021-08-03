#ifndef SOUND__H
#define SOUND__H

// To stop a playing song, it must also be freed. That's why you can't
// preload it and /then/ play it, like with sounds.


#include "mikmod.h"


typedef SAMPLE Sound;
typedef int Voice;


extern UWORD md_mode;
extern UBYTE md_musicvolume;    /* volume of song */
extern UBYTE md_sndfxvolume;    /* volume of sound effects */
extern UBYTE md_reverb;         /* 0 = none;  15 = chaos */
extern UBYTE md_pansep;         /* 0 = mono;  128 == 100% (full left/right) */


/**
 * Initialize Mikmod (sound and music).
 */
extern void initMikmod();

/**
 * Deinitialize Mikmod (sound and music)
 */
extern void unloadMikmod();

/**
 * Load a module music file (mod, xm, s3m, it, ...) and play it.
 * Filename must be a full path ("ms0:/psp/...")
 *
 * @pre filename != NULL
 * @param filename - filename of the music file to load
 */
extern void loadAndPlayMusicFile(char* filename, BOOL loop);

/**
 * Stop music.
 */
extern void stopAndUnloadMusic();

/**
 * Load a module wav file. !!NOTE!!: MUST BE MONO!
 * Filename must be a full path ("ms0:/psp/...")
 *
 * @pre filename != NULL
 * @param filename - filename of the wav file to load
 * @return pointer to a new allocated Sound struct, or NULL on failure
 */
extern Sound* loadSound(char* filename);

/**
 * Unload the loaded sound
 *
 * @pre handle != NULL
 * @param handle - the loaded wav file
 */
extern void unloadSound(Sound* handle);


/**
 * Play sample with volume 255, pan 127.
 *
 * @pre handle != NULL
 * @param handle - Handle of the loaded wav file.
 * @return handle of the currently playing voice
 */
extern Voice playSound(Sound* handle);

/**
 * Stop sample.
 *
 * @pre handle != NULL
 * @param handle - Handle of the loaded wav file.
 */
extern void stopSound(Voice handle);

extern void resumeSound(Voice handle, Sound* soundhandle);

void setSoundLooping(Sound *handle, int loopmode, unsigned long loopstart, unsigned long loopend);
unsigned long getSoundLengthInSamples(Sound *handle);
unsigned long getSoundSampleSpeed(Sound *handle);


/**
 * Sets the volume of the specified voice.
 *
 * @pre handle != NULL && vol >= 0 && vol <= 255
 * @param handle - Handle of the currently playing voice.
 */
extern void setVoiceVolume(Voice handle, UWORD vol);

/**
 * Sets the panning of the specified voice. 127 is middle,
 * 0 is full left and 255 is full right.
 *
 * @pre handle != NULL && pan >= 0 && pan <= 255
 * @param handle - Handle of the currently playing voice.
 */
extern void setVoicePanning(Voice handle, ULONG pan);

/**
 * Sets the speed/frequency of the specified voice.
 *
 * @pre handle != NULL && freq >= 0
 * @param handle - Handle of the currently playing voice.
 */
extern void setVoiceFrequency(Voice handle, ULONG freq);


/**
 * Checks whether music is playing at the moment.
 * 
 * @return whether music is playing or not.
 */
extern BOOL musicIsPlaying();


/**
 * Checks whether a specific voice is playing at the moment.
 * 
 * @pre handle != NULL 
 * @param handle - Handle of the currently playing voice.
 * @return whether music is playing or not.
 */
extern BOOL voiceIsPlaying(Voice handle);
 
 
/**
 * Sets the music volume.
 * 
 * @pre (arg > 0 && arg < 128) || arg==9999
 * @param arg is the volume
 * @return new arg
 */
extern unsigned setMusicVolume(unsigned arg);

/**
 * Sets the sample/SFX/wav volume.
 * 
 * @pre (arg > 0 && arg < 128) || arg==9999
 * @param arg is the volume
 * @return new arg
 */
extern unsigned setSFXVolume(unsigned arg);

/**
 * Sets the reverb.
 * 
 * @pre (arg > 0 && arg < 15) || arg==9999
 * @param arg is the amount of reverb.
 * @return new arg
 */
extern unsigned setReverb(unsigned arg);

/**
 * Sets panoramic separation. 0: mono. 128: full separation.
 * 
 * @pre (arg > 0 && arg < 128) || arg==9999
 * @param arg is the separation
 * @return new arg
 */
extern unsigned setPanSep(unsigned arg);



extern void musicPause();
extern void musicResume();


#endif
