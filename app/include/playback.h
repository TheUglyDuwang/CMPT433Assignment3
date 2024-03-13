#ifndef PLAYBACK_H
#define PLAYBACK_H

#include <alsa/asoundlib.h>

// Store data of a single wave file read into memory.
// Space is dynamically allocated; must be freed correctly!
typedef struct {
	int numSamples;
	short *pData;
} wavedata_t;

void Audio_readWaveFileIntoMemory(char *fileName, wavedata_t *pWaveStruct);

void Audio_playfile(snd_pcm_t *handle, wavedata_t *pWaveData);

#endif