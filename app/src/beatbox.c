#include <alsa/asoundlib.h>
#include <time.h>
#include <math.h>
#include "mixer.h"
#include "audio.h"
#include "playback.h"
#include "accel.h"
#include "joystick.h"
#include <alsa/asoundlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <limits.h>
#include <alloca.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

static int bpm = 120;
static int pattern = 3;

static bool pattern0 = false;
static bool pattern1 = false;
static bool pattern2 = false;
static bool play = false;



int getBPM(){
	return bpm;
}
int setBPM(int newBPM){
	return bpm = newBPM;
}

void changeBPM(int change){
	int newBPM = getBPM() + change;

	if(newBPM>300){
		newBPM = 300;
	}
	if(newBPM < 40){
		newBPM = 40;
	}
	setBPM(newBPM);
	
}

static void sleepBPM(){
	long long delay = floor(60000/getBPM());
	sleepForMs(delay);
}


int getPattern(){
	return pattern;
}

int setPattern(int n){
	return pattern = n;
}

void *playPattern(void *arg){
	while(play){
		while(pattern0){

		}


		while(pattern1){
			wavedata_t hiHat;
			wavedata_t base;
			wavedata_t snare;

			// Configure Output Device
			AudioMixer_readWaveFileIntoMemory(HI_HAT, &hiHat);
			AudioMixer_readWaveFileIntoMemory(SNARE, &snare);
			AudioMixer_readWaveFileIntoMemory(BASE, &base);
			int beat = 0;
			while (pattern1){
				if(beat == 0){
					beat = 1;
					AudioMixer_queueSound(&hiHat);
					AudioMixer_queueSound(&base);
				} else if(beat == 1){
					beat = 2;
					AudioMixer_queueSound(&hiHat);
					
					
				}else if(beat == 2){
					beat = 3;
					AudioMixer_queueSound(&hiHat);
					AudioMixer_queueSound(&snare);
				} else{
					beat = 0;
					AudioMixer_queueSound(&hiHat);

				}
				sleepBPM();
			}

			AudioMixer_freeWaveFileData(&hiHat);
			AudioMixer_freeWaveFileData(&base);
			AudioMixer_freeWaveFileData(&snare);
		}



		while(pattern2){
			wavedata_t hiHat;
			wavedata_t base;
			wavedata_t snare;

			// Configure Output Device
			AudioMixer_readWaveFileIntoMemory(HI_HAT, &hiHat);
			AudioMixer_readWaveFileIntoMemory(SNARE, &snare);
			AudioMixer_readWaveFileIntoMemory(BASE, &base);
			int beat = 0;
			while (pattern2){
				if(beat == 0){
					beat = 1;
					AudioMixer_queueSound(&hiHat);
					AudioMixer_queueSound(&base);
				} else if(beat == 1){
					beat = 2;
					AudioMixer_queueSound(&hiHat);
					AudioMixer_queueSound(&base);
				}else if(beat == 2){
					beat = 3;
					AudioMixer_queueSound(&hiHat);
					AudioMixer_queueSound(&snare);
				} else{
					beat = 0;
					AudioMixer_queueSound(&hiHat);

				}
				sleepBPM();
			}

			AudioMixer_freeWaveFileData(&hiHat);
			AudioMixer_freeWaveFileData(&base);
			AudioMixer_freeWaveFileData(&snare);
		}


	}
	return NULL;
}

void setBoolPattern(){
	if(getPattern() == 0){
		play = true;
		pattern0 = true;
		pattern1 = false;
		pattern2 = false;
	}
	if(getPattern() == 1){
		play = true;
		pattern0 = false;
		pattern1 = true;
		pattern2 = false;
	}
	if(getPattern() == 2){
		play = true;
		pattern0 = false;
		pattern1 = false;
		pattern2 = true;
	}
	if(getPattern() == 3){
		play = false;
		pattern0 = false;
		pattern1 = false;
		pattern2 = false;
	}
}


void setVolume(int volume){
    volume = AudioMixer_getVolume() + volume;
    if(volume > 100){
        volume = 100;
    }
    
    if(volume<0){
        volume = 0;
    }

    AudioMixer_setVolume(volume);
    return;
}

void* playAccel(void* arg){

	while(play){
		if(getPattern() == 0){

		}
		else if (getPattern() == 1){
			wavedata_t hiHatA1;
			wavedata_t baseA1;
			wavedata_t snareA1;

			// Configure Output Device
			AudioMixer_readWaveFileIntoMemory(HI_HATA1, &hiHatA1);
			AudioMixer_readWaveFileIntoMemory(SNAREA1, &snareA1);
			AudioMixer_readWaveFileIntoMemory(BASEA1, &baseA1);

			while(getPattern() == 1){
				int *a = getAccel();

				if(a[0] == -1 || a[0] ==1){

					AudioMixer_queueSound(&hiHatA1);
					sleepBPM();
				}
				else if(a[1]== -1 || a[1] ==1){

					AudioMixer_queueSound(&snareA1);
					sleepBPM();
				}else if(a[2] == -1 || a[2] ==0){

					AudioMixer_queueSound(&baseA1);
					sleepBPM();
				}
			}
			AudioMixer_freeWaveFileData(&hiHatA1);
			AudioMixer_freeWaveFileData(&baseA1);
			AudioMixer_freeWaveFileData(&snareA1);
		}
		else if(getPattern() == 2){
			wavedata_t hiHatA2;
			wavedata_t baseA2;
			wavedata_t snareA2;

			// Configure Output Device
			AudioMixer_readWaveFileIntoMemory(HI_HATA2, &hiHatA2);
			AudioMixer_readWaveFileIntoMemory(SNAREA2, &snareA2);
			AudioMixer_readWaveFileIntoMemory(BASEA2, &baseA2);
			while(getPattern() == 2){
				int *a = getAccel();

				if(a[0] == -1 || a[0] ==1){

					AudioMixer_queueSound(&hiHatA2);
					sleepBPM();
				}
				else if(a[1]== -1 || a[1] ==1){


					AudioMixer_queueSound(&snareA2);
					sleepBPM();
				}else if(a[2] == -1 || a[2] ==0){
					AudioMixer_queueSound(&baseA2);
					sleepBPM();
				}
			}
			AudioMixer_freeWaveFileData(&hiHatA2);
			AudioMixer_freeWaveFileData(&baseA2);
			AudioMixer_freeWaveFileData(&snareA2);
		}
	}
	return NULL;
}




void *terminalOutput(void* args){

	while(play){
		printf("M%d %dbpm vol:%d \n",getPattern(), getBPM(), AudioMixer_getVolume() );
		sleepForMs(1000);
	}

	return NULL;
}
//main

int main(void){
	setVolume(80);
	setBPM(120);





	AudioMixer_init();
	AccelStart();

	//sleepForMs(1000);

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