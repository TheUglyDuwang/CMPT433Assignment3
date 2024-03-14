#include <stdbool.h>
#include "mixer.h"
#include "playback.h"
#include "audio.h"
#include "beatbox.h"
#include "joystick.h"


// Function to initialize audio playback
void initAudio(){
    audioMixer_init();
    return;
}

// Function to generate different drum beats
void drumBeat0(){
    return;
}

void drumBeat1(){
    return;
}

void drumBeat2(){
    return;
}

// Function to control tempo (BPM)
void setTempo(int bpm){

    if(bpm>300){
        bpm = 300;
    }

    if(bpm < 40){
        bpm = 40;
    }

    return;
}

// Function to control volume
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

// Function to play additional drum sounds
void playDrumSound(int soundIndex){
    return;
}


int main(){
    bool beatBoxOn = true;
    int beatNum = 0;//default for beat is 0


    while(beatBoxOn){
        if(getDirection() == up){
            setVolume(5);//change volume by 5
        } else if (getDirection() == down){
            setVolume(-5);//change volume by -5
        } else if (getDirection() == left){

        } else if (getDirection() == right){

        } else if (getDirection() == pushed){
            if ( beatNum = 0){
                drumBeat1();
                beatNum = 1;
            }
            if ( beatNum = 1){
                drumBeat2();
                beatNum = 2;
            }
            if ( beatNum = 2){
                drumBeat0();
                beatNum = 0;
            }
            
        }

    }


    return 0;
}