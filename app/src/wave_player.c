#include "mixer.h"
#include "playback.h"
#include "audio.h"

#define HI_HAT "wave-files/100053__menegass__gui-drum-cc.wav"
#define SNARE "wave-files/100059__menegass__gui-drum-snare-soft.wav"
#define BASE "wave-files/100051__menegass__gui-drum-bd-hard.wav"

#define BPM 60


int main(void){
	AudioMixer_init();
	wavedata_t hiHat;
	wavedata_t base;
	wavedata_t snare;
	wavedata_t hiHatSnare;
	wavedata_t hiHatBase;

	int delay = floor((60/(BPM)/8)*1000);
	// Configure Output Device
	AudioMixer_readWaveFileIntoMemory(HI_HAT, &hiHat);
	AudioMixer_readWaveFileIntoMemory(SNARE, &snare);
	AudioMixer_readWaveFileIntoMemory(BASE, &base);

	hiHatBase = &hiHat + &base;
	hiHatSnare = *hiHat + * snare;

	int beat = 0;
	for (int i = 0 ; i < 40; i++){

		// Play Audio
		if(beat == 0){
			beat = 1;
			// AudioMixer_queueSound(&hiHat);
			// sleepForMs(100);
			AudioMixer_queueSound(&base);
		} else if(beat == 1){
			beat = 2;
			AudioMixer_queueSound(&hiHat);
			sleepForMs(100);
		}else if(beat == 2){
			beat = 3;
			// AudioMixer_queueSound(&hiHat);
			// sleepForMs(100);
			AudioMixer_queueSound(&snare);
		} else{
			beat = 0;
			AudioMixer_queueSound(&hiHat);

		}
		sleepForMs(delay);
	}
	sleepForMs(delay);
	
	AudioMixer_freeWaveFileData(&hiHat);
	AudioMixer_freeWaveFileData(&base);
	AudioMixer_freeWaveFileData(&snare);

	AudioMixer_cleanup();

	return 0;
}
