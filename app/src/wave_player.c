#include "mixer.h"
#include "playback.h"
#include "audio.h"
#include "beatbox.h"
#include "accel.h"

// File used for play-back:
// If cross-compiling, must have this file available, via this relative path,
// on the target when the application is run. This example's Makefile copies the wave-files/
// folder along with the executable to ensure both are present.
#define SOURCE_FILE "wave-files/100060__menegass__gui-drum-splash-hard.wav"
#define HI_HAT "wave-files/100053__menegass__gui-drum-cc.wav"
#define SNARE "wave-files/100059__menegass__gui-drum-snare-soft.wav"
#define BASE "wave-files/100051__menegass__gui-drum-bd-hard.wav"

#define HI_HATA1 "wave-files/100056__menegass__gui-drum-cyn-hard.wav"
#define SNAREA1 "wave-files/100060__menegass__gui-drum-splash-hard.wav"
#define BASEA1 "wave-files/100066__menegass__gui-drum-tom-mid-hard.wav"

#define HI_HATA2 "wave-files/100055__menegass__gui-drum-co.wav"
#define SNAREA2 "wave-files/100058__menegass__gui-drum-snare-hard.wav"
#define BASEA2 "wave-files/100067__menegass__gui-drum-tom-mid-soft.wav"
#define BPM 60


//main

int main(void){
	setVolume(80);
	setBPM(120);

	AudioMixer_init();
	AccelStart();

	setPattern(1);
	setBoolPattern();

    pthread_t playPatternThread;

	pthread_t accelThread;

	pthread_t terminalThread;

    if (pthread_create(&playPatternThread, NULL, playPattern, NULL) != 0) {
        fprintf(stderr, "Error creating playPattern thread\n");
        return 1; // Return an error code if thread creation fails
    }
	
	if (pthread_create(&accelThread, NULL, playAccel, NULL) != 0) {
        fprintf(stderr, "Error creating playPattern thread\n");
        return 1; // Return an error code if thread creation fails
    }

	if (pthread_create(&terminalThread, NULL, terminalOutput, NULL) != 0) {
        fprintf(stderr, "Error creating playPattern thread\n");
        return 1; // Return an error code if thread creation fails
    }
	

	bool terminate = false;

	while(!terminate){
		if(terminate){
			setPattern(3);
			sleepForMs(400);
			setBoolPattern();
			break;
		}
		else if(getDirection() == up){
			setVolume(5);
			printf("Volume: %d\n", AudioMixer_getVolume());
			sleepForMs(400);
		} 
		else if(getDirection() == down){
			setVolume(-5);
			printf("Volume: %d\n", AudioMixer_getVolume());
			sleepForMs(400);
		}
		else if(getDirection() == left){
			changeBPM(-5);
			printf("BPM: %d\n", getBPM());
			sleepForMs(400);
		}
		else if(getDirection() == right){
			changeBPM(5);
			printf("BPM: %d\n", getBPM());
			sleepForMs(400);
		}
		else if(getDirection() == pushed){
			if(getPattern() == 0){
				setPattern(1);
				sleepForMs(400);
				setBoolPattern();
				printf("Pattern: %d : rock\n", getPattern());
				sleepForMs(400);
			}
			else if(getPattern() == 1){
				setPattern(2);
				sleepForMs(400);
				setBoolPattern();
				printf("Pattern: %d : custom\n", getPattern());
				
			}
			else if(getPattern() == 2){
				setPattern(0);
				sleepForMs(400);
				setBoolPattern();
				printf("Pattern: %d none\n", getPattern());
			}
		}
	}

    if (pthread_join(playPatternThread, NULL) != 0) {
        fprintf(stderr, "Error joining playPattern thread\n");
        return 1; 
    }

	if (pthread_join(accelThread, NULL) != 0) {
        fprintf(stderr, "Error joining accelThread thread\n");
        return 1; 
    }

	if (pthread_join(terminalThread, NULL) != 0) {
        fprintf(stderr, "Error joining terminalThread thread\n");
        return 1; 
    }

	AccelStop();
	AudioMixer_cleanup();
	

	return 0;
}
