
#ifndef BEATBOX_H
#define BEATBOX_H

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

// Function to initialize audio playback
void initAudio();

// Function to generate different drum beats
void dumBeat1();
void drumBeat2();
void drumBeat3();

// Function to control tempo (BPM)
void setTempo(int bpm);

// Function to control volume
void setVolume(int volume);

// Function to play additional drum sounds
void playDrumSound(int soundIndex);
